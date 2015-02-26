#include "ScriptPCH.h"
#include "Chat.h"
#include <stdarg.h>
#include "GameObject.h"
#include "PoolMgr.h"
#include "ObjectAccessor.h"
#include "Transport.h"
#include "Language.h"
using namespace std;

void CreatePhase(Player * player, bool IsMember, uint32 phase)
{
	ostringstream ss;

	QueryResult accName = LoginDatabase.PQuery("SELECT username FROM account WHERE id='%u'", player->GetSession()->GetAccountId());

	Field * a_fields = accName->Fetch();

	std::string userName = a_fields[0].GetString();

	if (IsMember == false)
	{
		ss << "INSERT INTO phase VALUES("
			<< "'" << player->GetSession()->GetAccountId() << "',"
			<< "'" << userName << "',"
			<< "'" << phase << "',"
			<< "'" << phase << "',"
			<< "'" << phase << "',"
			<< "'" << "0" << "',"
			<< "'" << player->GetName() << "');";

	}
	else
	{
		ss << "INSERT INTO phase_members VALUES("
			<< "'" << player->GetSession()->GetAccountId() << "',"
			<< "'" << userName << "',"
			<< "'" << phase << "',"
			<< "'" << phase << "');";
	}

	SQLTransaction transat = CharacterDatabase.BeginTransaction();
	transat->Append(ss.str().c_str());
	CharacterDatabase.CommitTransaction(transat);
}

class phasing_system : public CommandScript
{
public:
	phasing_system() : CommandScript("phasing_system") { }

	ChatCommand * GetCommands() const
	{
		static ChatCommand phaseNpcTable[] =
		{
			{ "add", rbac::RBAC_PERM_COMMAND_PHASE_AddNPC, false, &HandlePhaseAddNpcCommand, "", NULL },
			{ "delete", rbac::RBAC_PERM_COMMAND_PHASE_DeleteNPC, false, &HandlePhaseDeleteNpcCommand, "", NULL },
			{ NULL, 0, false, NULL, "", NULL }
		};

		static ChatCommand phaseGoTable[] =
		{
			{ "add", rbac::RBAC_PERM_COMMAND_PHASE_AddObject, false, &HandlePhaseGoCommand, "", NULL },
			{ "delete", rbac::RBAC_PERM_COMMAND_PHASE_DeleteObject, false, &HandlePhaseGoDeleteCommand, "", NULL },
			{ NULL, 0, false, NULL, "", NULL }
		};

		static ChatCommand phaseCmdTable[] =
		{
			{ "create", rbac::RBAC_PERM_COMMAND_PHASE_Create, false, &HandlePhaseCreateCommand, "", NULL },
			{ "join", rbac::RBAC_PERM_COMMAND_PHASE_Join, false, &HandlePhaseJoinCommand, "", NULL },
			{ "leave", rbac::RBAC_PERM_COMMAND_PHASE_Leave, false, &HandlePhaseLeaveCommand, "", NULL },
			{ "delete", rbac::RBAC_PERM_COMMAND_PHASE_Delete, false, &HandlePhaseDeleteCommand, "", NULL },
			{ "get", rbac::RBAC_PERM_COMMAND_PHASE_Get, false, &HandlePhaseGetCommand, "", NULL },
			{ "complete", rbac::RBAC_PERM_COMMAND_PHASE_Complete, false, &HandlePhaseCompleteCommand, "", NULL },
			{ "name", rbac::RBAC_PERM_COMMAND_PHASE_Name, false, &HandlePhaseNameCommand, "", NULL },
			{ "kick", rbac::RBAC_PERM_COMMAND_PHASE_Kick, false, &HandlePhaseKickCommand, "", NULL },
			{ "addmember", rbac::RBAC_PERM_COMMAND_PHASE_AddMember, false, &HandlePhaseAddMemberCommand, "", NULL },
			{ "deletemember", rbac::RBAC_PERM_COMMAND_PHASE_DeleteMember, false, &HandlePhaseDeleteMemberCommand, "", NULL },
			{ "npc", rbac::RBAC_PERM_COMMAND_PHASE_AddNPC, false, NULL, "", phaseNpcTable },
			{ "gobj", rbac::RBAC_PERM_COMMAND_PHASE_AddObject, false, NULL, "", phaseGoTable },
			{ NULL, 0, false, NULL, "", NULL }
		};

		static ChatCommand phasecommandTable[] =
		{
			{ "phase", rbac::RBAC_PERM_COMMAND_PHASE, false, NULL, "", phaseCmdTable },
			{ NULL, 0, false, NULL, "", NULL }
		};
		return phasecommandTable;
	}

	static bool HandlePhaseJoinCommand(ChatHandler * chat, const char * args)
	{
		if (!*args)
			return false;

		Player * player = chat->GetSession()->GetPlayer();
		uint32 phase = (uint32)atoi((char*)args);

		if (phase == 0)
		{
			player->ClearPhases();
			player->SetInPhase(phase, true, !player->IsInPhase(phase));
			player->ToPlayer()->SendUpdatePhasing();

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetSession()->GetAccountId());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 0.|r (Default Phase)");

			return true;
		}
		else if (phase == 1)
		{
			player->ClearPhases();
			player->SetInPhase(phase, true, !player->IsInPhase(phase));
			player->ToPlayer()->SendUpdatePhasing();

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetSession()->GetAccountId());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 1.|r (Main Event Phase)");

			return true;
		}
		else if (phase == 2)
		{
			player->ClearPhases();
			player->SetInPhase(phase, true, !player->IsInPhase(phase));
			player->ToPlayer()->SendUpdatePhasing();

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetSession()->GetAccountId());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 2.|r (Main Interior Phase)");

			return true;
		}

		QueryResult hasPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		if (hasPhase)
		{
			do
			{
				Field * fields = hasPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					chat->PSendSysMessage("|cffFF0000You need to create a phase before joining one!|r");

					chat->SetSentErrorMessage(true);
					return false;
				}
			} while (hasPhase->NextRow());
		}

		QueryResult phaseExist = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE phase_owned='%u'", phase);
		if (phaseExist)
		{
			do
			{
				Field * fields = phaseExist->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					chat->PSendSysMessage("|cffFF0000The selected phase is not registered!|r");

					chat->SetSentErrorMessage(true);
					return false;
				}
			} while (hasPhase->NextRow());
		}

		QueryResult isCompleted = CharacterDatabase.PQuery("SELECT * FROM phase WHERE phase='%u'", phase);
		if (!isCompleted)
			return false;

		if (isCompleted)
		{
			do
			{
				Field * fields = isCompleted->Fetch();
				const char * phaseName = fields[6].GetCString();

				if (fields[5].GetUInt16() == 1) // if the phase is completed
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetSession()->GetAccountId());
					chat->PSendSysMessage("|cff4169E1You are now entering phase %u.|r (%s)", phase, phaseName);

					return true;
				}
				else if (player->GetSession()->GetAccountId() == fields[0].GetUInt64() && fields[2].GetUInt32() == phase)
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetSession()->GetAccountId());
					chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r (%s)", phase, phaseName);

					return true;
				}
				else // if the phase isn't completed
				{
					chat->PSendSysMessage("|cffFF0000This phase isn't completed yet!|r \n Phase: %u", phase);

					chat->SetSentErrorMessage(true);
					return false;
				}
			} while (isCompleted->NextRow());
		}
		return true;
	};

	static bool HandlePhaseLeaveCommand(ChatHandler* handler, char const* /*args*/)
	{
		Player * player = handler->GetSession()->GetPlayer();

		player->ClearPhases();
		player->ToPlayer()->SendUpdatePhasing();

		CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", 0, player->GetSession()->GetAccountId());
		handler->PSendSysMessage("|cff4169E1You are now entering phase 0.|r (Default Phase)");

		return true;
	}

	static bool HandlePhaseCreateCommand(ChatHandler * chat, const char * args)
	{
		if (!*args)
			return false;

		Player * player = chat->GetSession()->GetPlayer();

		uint32 phase = atoi((char*)args);

		if (phase == 0){ // Phase 0 is the main phase, ownership is denied.
			chat->SendSysMessage("|cffFF0000You cannot own a reserved phase!|r");

			chat->SetSentErrorMessage(true);
			return false;
		}
		else if (phase == 1){ // Phase 1 is reserved, ownership is denied.
			chat->SendSysMessage("|cffFF0000You cannot own a reserved phase!|r");

			chat->SetSentErrorMessage(true);
			return false;
		}
		else if (phase == 2){ // Phase 2 is reserved, ownership is denied.
			chat->SendSysMessage("|cffFF0000You cannot own a reserved phase!|r");

			chat->SetSentErrorMessage(true);
			return false;
		}

		QueryResult get_phase = CharacterDatabase.PQuery("SELECT phase_owned FROM phase WHERE phase='%u'", phase);
		QueryResult check = CharacterDatabase.PQuery("SELECT phase_owned FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());

		if (check)
		{
			do
			{
				Field * mfields = check->Fetch();
				if (mfields[0].GetInt32() != 0)
				{
					chat->SendSysMessage("|cffFF0000You already have a phase!|r");
					chat->SetSentErrorMessage(true);
					return false;
				}
				else
				{
					return true;
				}
			} while (check->NextRow());
		}

		if (get_phase)
		{
			do
			{
				Field * fields = get_phase->Fetch();
				if (phase == fields[0].GetInt32())
				{
					chat->SendSysMessage("|cffFF0000This phase has already been taken!|r");
					chat->SetSentErrorMessage(true);
					return false;
				}
				else
				{
					return true;
				}
			} while (get_phase->NextRow());
		}

		CreatePhase(player, false, phase);
		CreatePhase(player, true, phase);
		player->SetInPhase(phase, true, !player->IsInPhase(phase));
		player->ToPlayer()->SendUpdatePhasing();
		chat->SendSysMessage("|cff4169E1You have created your phase!|r \n |cffbbbbbb.phase join #|r - |cff00FF00is to join a phase.|r \n |cffbbbbbb.phase complete|r - |cff00FF00completes your phase for the public to join and see.|r");
		return true;
	};

	static bool HandlePhaseDeleteCommand(ChatHandler * chat, const char * args)
	{
		Player * player = chat->GetSession()->GetPlayer();

		player->SetPhaseMask(0, true);
		QueryResult res = CharacterDatabase.PQuery("SELECT * FROM phase WHERE guid='%u' LIMIT 1", player->GetSession()->GetAccountId());
		Field * fields = res->Fetch();
		if (!res)
			return false;

		QueryResult phaseObjects = WorldDatabase.PQuery("SELECT * FROM gameobject WHERE PhaseId='%u'", fields[3].GetInt32());

		CharacterDatabase.PExecute("DELETE FROM phase WHERE (guid='%u')", player->GetSession()->GetAccountId());
		CharacterDatabase.PExecute("DELETE FROM phase_members WHERE (guid='%u')", player->GetSession()->GetAccountId());

		if (phaseObjects)
		{
			do
			{
				Field * o_fields = phaseObjects->Fetch();

				char* id = chat->extractKeyFromLink((char*)o_fields[1].GetCString(), "Hgameobject");
				if (!id)
					return false;

				uint32 guidLow = o_fields[0].GetInt32();
				if (!guidLow)
					return false;

				GameObject* object = NULL;

				// by DB guid
				if (GameObjectData const* gameObjectData = sObjectMgr->GetGOData(guidLow))
					object = chat->GetObjectGlobalyWithGuidOrNearWithDbGuid(guidLow, gameObjectData->id);

				if (!object)
				{
					chat->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, guidLow);
					chat->SetSentErrorMessage(true);
					return false;
				}

				object->SetRespawnTime(0);                                 // not save respawn time
				object->Delete();
				object->DeleteFromDB();

				chat->PSendSysMessage("|cffADD8E6Phased Objects Have Been Cleared!|r");
			} while (phaseObjects->NextRow());
		}

		chat->SendSysMessage("|cffFFFF00Your phase has now been deleted.|r");
		return true;
	};

	static bool HandlePhaseGetCommand(ChatHandler * chat, const char * args)
	{
		Player * player = chat->GetSession()->GetPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT COUNT(*),get_phase,phase,phase_name FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		if (!getPhaseAndOwnedPhase)
			return false;

		std::stringstream phases;

		for (uint32 phase : player->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
			uint32 phase = 0;

		if (getPhaseAndOwnedPhase)
		{
			do
			{
				Field * fields = getPhaseAndOwnedPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					chat->SendSysMessage("|cffFF0000Could not get phase or owned phase.|r");
					chat->SetSentErrorMessage(true);
					return false;
				}
				chat->PSendSysMessage("|cffADD8E6Current Phase: %u \n Owned Phase: %u (%s)|r", phase, fields[2].GetUInt32(), fields[3].GetCString());
			} while (getPhaseAndOwnedPhase->NextRow());
		}
		return true;
	};

	static bool HandlePhaseCompleteCommand(ChatHandler* handler, const char* args)
	{
		std::string argstr = (char*)args;

		Player * player = handler->GetSession()->GetPlayer();

		if (!*args)
			return false;

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		if (isOwnerOfAPhase)
		{
			do
			{
				Field * fields = isOwnerOfAPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->SendSysMessage("|cffFF0000Could not complete phase.|r");
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwnerOfAPhase->NextRow());
		}
		if (argstr == "on")
		{
			handler->SendSysMessage("|cffFFA500You have now completed your phase! It is open for public.|r");
			CharacterDatabase.PExecute("UPDATE phase SET has_completed='1' WHERE guid='%u'", player->GetSession()->GetAccountId());
		}
		else if (argstr == "off")
		{
			handler->SendSysMessage("|cffFFA500Your phase is no longer public!|r");
			CharacterDatabase.PExecute("UPDATE phase SET has_completed='0' WHERE guid='%u'", player->GetSession()->GetAccountId());
		}
		return true;
	};

	static bool HandlePhaseNameCommand(ChatHandler* handler, const char* args)
	{
		std::string argstr = (char*)args;

		Player * player = handler->GetSession()->GetPlayer();

		if (!*args)
			return false;

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		if (isOwnerOfAPhase)
		{
			do
			{
				Field * fields = isOwnerOfAPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->SendSysMessage("|cffFF0000Could not name phase.|r");

					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwnerOfAPhase->NextRow());
		}
		handler->SendSysMessage("|cffFFA500You have renamed your phase.|r");
		CharacterDatabase.PExecute("UPDATE phase SET phase_name='%s' WHERE guid='%u'", args, player->GetSession()->GetAccountId());
		return true;
	};

	static bool HandlePhaseAddMemberCommand(ChatHandler * handler, const char * /*args*/)
	{

		Player * player = handler->GetSession()->GetPlayer();
		Player * target = handler->getSelectedPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT get_phase, phase_owned FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		Field * fields = getPhaseAndOwnedPhase->Fetch();
		uint32 phase_owned = fields[1].GetUInt32();

		if (!target)
		{
			handler->SendSysMessage("You must select a target!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (target == player)
		{
			handler->SendSysMessage("You cannot add yourself!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		QueryResult ownsPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE player_name='%s'", player->GetName().c_str());
		if (ownsPhase)
		{
			do
			{
				Field * fields = ownsPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->PSendSysMessage("|cffFF0000 You do not own a phase; therefore, you cannot add someone to your phase.|r");
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (ownsPhase->NextRow());
		}

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE player_name='%s'", target->GetName().c_str());
		if (isOwnerOfAPhase)
		{
			do
			{
				Field * fields = isOwnerOfAPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->PSendSysMessage("|cffFF0000 %s does not own a phase; therefore, he/she cannot be added to your phase.|r", target->GetName().c_str());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwnerOfAPhase->NextRow());
		}

		QueryResult isMember = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE player_name='%s' AND phase='%u'", target->GetName().c_str(), phase_owned);
		if (isMember)
		{
			do
			{
				Field * field = isMember->Fetch();
				if (field[0].GetInt32() == 1)
				{
					handler->PSendSysMessage("|cffFF0000 %s is already a member of your phase!|r", target->GetName().c_str());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isMember->NextRow());
		}

		handler->PSendSysMessage("|cffFFA500You successfully added %s to your phase %u.|r", target->GetName().c_str(), phase_owned);
		CreatePhase(target, true, phase_owned);
		return true;
	};


	static bool HandlePhaseDeleteMemberCommand(ChatHandler * handler, const char * /*args*/)
	{

		Player * player = handler->GetSession()->GetPlayer();
		Player * target = handler->getSelectedPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT get_phase, phase_owned FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
		Field * fields = getPhaseAndOwnedPhase->Fetch();
		uint32 phase_owned = fields[1].GetUInt32();

		if (!target)
		{
			handler->SendSysMessage("You must select a target!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (target == player)
		{
			handler->SendSysMessage("You cannot remove yourself!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		QueryResult ownsPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE player_name='%s'", player->GetName().c_str());
		if (ownsPhase)
		{
			do
			{
				Field * fields = ownsPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->PSendSysMessage("|cffFF0000 You do not own a phase; therefore, you cannot remove someone from your phase.|r");
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (ownsPhase->NextRow());
		}

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE player_name='%s'", target->GetName().c_str());
		if (isOwnerOfAPhase)
		{
			do
			{
				Field * fields = isOwnerOfAPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->PSendSysMessage("|cffFF0000 %s does not own a phase; therefore, he/she cannot be removed from your phase.|r", target->GetName().c_str());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwnerOfAPhase->NextRow());
		}

		QueryResult isMember = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE player_name='%s' AND phase='%u'", target->GetName().c_str(), phase_owned);
		if (!isMember)
		{
			do
			{
				Field * field = isMember->Fetch();
				if (field[0].GetInt32() == 1)
				{
					handler->PSendSysMessage("|cffFF0000 %s is not a member of your phase!|r", target->GetName().c_str());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isMember->NextRow());
		}

		handler->PSendSysMessage("|cffFFA500You successfully removed %s from your phase %u.|r", target->GetName().c_str(), phase_owned);
		CharacterDatabase.PExecute("DELETE FROM phase_members WHERE guid='%u' AND phase='%u'", target->GetGUID(), phase_owned);
		return true;
	}

	static bool HandlePhaseKickCommand(ChatHandler * handler, const char * /*args*/)
	{
		Player * target = handler->getSelectedPlayer();
		Player * player = handler->GetSession()->GetPlayer();

		std::stringstream phases;

		for (uint32 phase : target->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
		{
			handler->SendSysMessage("You target is not phased!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase == 0)
		{
			handler->SendSysMessage("You target is not phased!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase > 0)
		{
			QueryResult getplayer = CharacterDatabase.PQuery("SELECT phase_owned FROM phase WHERE player_name='%s'", player->GetName().c_str());
			if (!getplayer)
			{
				handler->SendSysMessage("Something went wrong. Please ensure that you are the owner of this phase.");
				handler->SetSentErrorMessage(true);
				return false;
			}

			char msg[255];

			Field * mfields = getplayer->Fetch();
			if (mfields[0].GetInt32() == phase)
			{
				target->ClearPhases();
				target->ToPlayer()->SendUpdatePhasing();
				snprintf(msg, 255, "|cffFF0000You were kicked from phase %u by %s!|r", phase, player->GetName().c_str());
				player->Whisper(msg, LANG_UNIVERSAL, target);
				CharacterDatabase.PExecute("UPDATE phase SET get_phase='0' WHERE (guid='%u')", target->GetGUID());
			}
			else
			{
				snprintf(msg, 255, "|cffFF0000%s is not currently in your phase!|r", target->GetName().c_str());
				handler->SendSysMessage(msg);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}
		return true;
	};

#ifndef _PHASE_NPC_COMMANDS_
	static bool HandlePhaseAddNpcCommand(ChatHandler* handler, const char* args)
	{
        if (!*args)
            return false;

        char* charID = handler->extractKeyFromLink((char*)args, "Hcreature_entry");
        if (!charID)
            return false;

        uint32 id  = atoi(charID);
        if (!sObjectMgr->GetCreatureTemplate(id))
            return false;

        Player* chr = handler->GetSession()->GetPlayer();
        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float o = chr->GetOrientation();
        Map* map = chr->GetMap();

		std::stringstream phases;

		for (uint32 phase : chr->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
		{
			handler->SendSysMessage("You cannot spawn creatures in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase == 0)
		{
			handler->SendSysMessage("You cannot spawn creatures in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (Transport* trans = chr->GetTransport())
		{
			uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT);
			CreatureData& data = sObjectMgr->NewOrExistCreatureData(guid);
			data.id = id;
			data.phaseMask = chr->GetPhaseMask();
			data.posX = chr->GetTransOffsetX();
			data.posY = chr->GetTransOffsetY();
			data.posZ = chr->GetTransOffsetZ();
			data.orientation = chr->GetTransOffsetO();

			Creature* creature = trans->CreateNPCPassenger(guid, &data);

			// creature->CopyPhaseFrom(chr); // will not be saved, and probably useless

			creature->SaveToDB(trans->GetGOInfo()->moTransport.mapID, 1 << map->GetSpawnMode(), chr->GetPhaseMask());

			sObjectMgr->AddCreatureToGrid(guid, &data);
			return true;
		}

		Creature* creature = new Creature();
		if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), map, chr->GetPhaseMask(), id, x, y, z, o))
		{
			delete creature;
			return false;
		}

		if (phase)
		{
			QueryResult res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", chr->GetGUID());
			if (res)
			{
				do
				{
					Field * fields = res->Fetch();
					uint32 val = fields[0].GetUInt32();
					if (val != phase)
					{
						handler->SendSysMessage("You are not in this phase!");
						handler->SetSentErrorMessage(true);
						return false;
					}

					QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='%u' LIMIT 1", chr->GetGUID(), (uint32)val);

					if (!result)
					{
						handler->SendSysMessage("You must be added to this phase before you can add creatures.");
						handler->SetSentErrorMessage(true);
						return false;
					}

					if (result)
					{
						do
						{
							if (val == 0)
							{
								handler->SendSysMessage("You cannot spawn creatures in the main phase!");
								handler->SetSentErrorMessage(true);
								return false;
							}

							Field * fields = result->Fetch();
							if (fields[0].GetInt32() == 0)
							{
								handler->SendSysMessage("You must be added to this phase before you can add creatures.");
								handler->SetSentErrorMessage(true);
								return false;
							}
						} while (result->NextRow());
					}
				} while (res->NextRow());
			}
		}

		//creature->CopyPhaseFrom(chr); // creature is not directly added to world, only to db, so this is useless here

		creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMask());

		WorldDatabase.PExecute("UPDATE creature SET PhaseId='%u' WHERE guid='%u'", phase, creature->GetGUID());

		creature->ClearPhases();
		creature->SetInPhase(phase, true, true);
		creature->SetDBPhase(phase);
		creature->SaveToDB();

		uint32 db_guid = creature->GetDBTableGUIDLow();

		// To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells()
		// current "creature" variable is deleted and created fresh new, otherwise old values might trigger asserts or cause undefined behavior
		creature->CleanupsBeforeDelete();
		delete creature;
		creature = new Creature();
		if (!creature->LoadCreatureFromDB(db_guid, map))
		{
			delete creature;
			return false;
		}

		sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
		return true;
	}

	static bool HandlePhaseDeleteNpcCommand(ChatHandler * handler, const char * /*args*/)
	{
		Creature* unit = NULL;

		Player* chr = handler->GetSession()->GetPlayer();

		unit = handler->getSelectedCreature();

		std::stringstream phases;

		for (uint32 phase : chr->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
		{
			handler->SendSysMessage("You cannot spawn creatures in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase == 0)
		{
			handler->SendSysMessage("You cannot spawn creatures in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase)
		{
			QueryResult res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", chr->GetGUID());
			if (res)
			{
				do
				{
					Field * fields = res->Fetch();
					uint32 val = fields[0].GetUInt32();
					if (val != phase)
					{
						handler->SendSysMessage("You are not in this phase!");
						handler->SetSentErrorMessage(true);
						return false;
					}

					QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='%u' LIMIT 1", chr->GetGUID(), (uint32)val);

					if (!result)
					{
						handler->SendSysMessage("You must be added to this phase before you can add creatures.");
						handler->SetSentErrorMessage(true);
						return false;
					}

					if (result)
					{
						do
						{
							if (val == 0)
							{
								handler->SendSysMessage("You cannot spawn creatures in the main phase!");
								handler->SetSentErrorMessage(true);
								return false;
							}

							Field * fields = result->Fetch();
							if (fields[0].GetInt32() == 0)
							{
								handler->SendSysMessage("You must be added to this phase before you can delete creatures.");
								handler->SetSentErrorMessage(true);
								return false;
							}
						} while (result->NextRow());
					}
				} while (res->NextRow());
			}
		}

		if (!unit || unit->IsPet() || unit->IsTotem())
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		// Delete the creature
		unit->CombatStop();
		unit->DeleteFromDB();
		unit->AddObjectToRemoveList();

		handler->SendSysMessage(LANG_COMMAND_DELCREATMESSAGE);

		return true;
	}
#endif

#ifndef _PHASE_GO_COMMANDS_
	static bool HandlePhaseGoCommand(ChatHandler * handler, const char * args)
	{
		if (!*args)
			return false;

		// number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
		char* id = handler->extractKeyFromLink((char*)args, "Hgameobject_entry");
		if (!id)
			return false;

		uint32 objectId = atol(id);
		if (!objectId)
			return false;

		char* spawntimeSecs = strtok(NULL, " ");

		const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

		if (!objectInfo)
		{
			handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, objectId);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
		{
			// report to DB errors log as in loading case
			TC_LOG_ERROR("sql.sql", "Gameobject (Entry %u GoType: %u) have invalid displayId (%u), not spawned.", objectId, objectInfo->type, objectInfo->displayId);
			handler->PSendSysMessage(LANG_GAMEOBJECT_HAVE_INVALID_DATA, objectId);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Player* player = handler->GetSession()->GetPlayer();
		float x = float(player->GetPositionX());
		float y = float(player->GetPositionY());
		float z = float(player->GetPositionZ());
		float o = float(player->GetOrientation());
		Map* map = player->GetMap();

		std::stringstream phases;

		for (uint32 phase : player->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
			uint32 phase = 0;

		if (!phase)
		{
			handler->SendSysMessage("You cannot spawn objects in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase == 0)
		{
			handler->SendSysMessage("You cannot spawn objects in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase)
		{
			QueryResult res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
			if (res)
			{
				do
				{
					Field * fields = res->Fetch();
					uint32 val = fields[0].GetUInt32();
					if (val != phase)
					{
						handler->SendSysMessage("You are not in this phase!");
						handler->SetSentErrorMessage(true);
						return false;
					}

					QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='%u' LIMIT 1", player->GetSession()->GetAccountId(), (uint32)val);

					if (!result)
					{
						handler->SendSysMessage("You must be added to this phase before you can add creatures.");
						handler->SetSentErrorMessage(true);
						return false;
					}

					if (result)
					{
						do
						{
							if (val == 0)
							{
								handler->SendSysMessage("You cannot spawn objects in the main phase!");
								handler->SetSentErrorMessage(true);
								return false;
							}

							Field * fields = result->Fetch();
							if (fields[0].GetInt32() == 0)
							{
								handler->SendSysMessage("You must be added to this phase before you can spawn objects.");
								handler->SetSentErrorMessage(true);
								return false;
							}
						} while (result->NextRow());
					}
				} while (res->NextRow());
			}
		}

		GameObject* object = new GameObject;
		uint32 guidLow = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

		if (!object->Create(guidLow, objectInfo->entry, map, 0, x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
		{
			delete object;
			return false;
		}

		/*object->CopyPhaseFrom(player);*/

		if (spawntimeSecs)
		{
			uint32 value = atoi((char*)spawntimeSecs);
			object->SetRespawnTime(value);
		}

		// fill the gameobject data and save to the db
		object->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), player->GetPhaseMask());

		// delete the old object and do a clean load from DB with a fresh new GameObject instance.
		// this is required to avoid weird behavior and memory leaks
		delete object;

		object = new GameObject();
		// this will generate a new guid if the object is in an instance
		if (!object->LoadGameObjectFromDB(guidLow, map))
		{
			delete object;
			return false;
		}

		/// @todo is it really necessary to add both the real and DB table guid here ?
		sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGOData(guidLow));

		handler->PSendSysMessage(LANG_GAMEOBJECT_ADD, objectId, objectInfo->name.c_str(), guidLow, x, y, z);

		WorldDatabase.PExecute("UPDATE gameobject SET PhaseId='%u' WHERE guid='%u'", phase, guidLow);

		object->ClearPhases();
		object->SetInPhase(phase, true, true);
		object->SetDBPhase(phase);

		object->SaveToDB();

		return true;
	}

	static bool HandlePhaseGoDeleteCommand(ChatHandler * handler, const char * args)
	{
		// number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
		char* id = handler->extractKeyFromLink((char*)args, "Hgameobject");
		if (!id)
			return false;

		uint32 guidLow = atoi(id);
		if (!guidLow)
			return false;

		GameObject* object = NULL;

		// by DB guid
		if (GameObjectData const* gameObjectData = sObjectMgr->GetGOData(guidLow))
			object = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(guidLow, gameObjectData->id);

		if (!object)
		{
			handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, guidLow);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Player* player = handler->GetSession()->GetPlayer();

		std::stringstream phases;

		for (uint32 phase : player->GetPhases())
		{
			phases << phase << " ";
		}

		const char* phasing = phases.str().c_str();

		uint32 phase = atoi(phasing);

		if (!phase)
			uint32 phase = 0;

		if (!phase)
		{
			handler->SendSysMessage("You cannot delete objects in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase == 0)
		{
			handler->SendSysMessage("You cannot delete objects in the main phase!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (phase)
		{
			QueryResult res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", player->GetSession()->GetAccountId());
			if (res)
			{
				do
				{
					Field * fields = res->Fetch();
					uint32 val = fields[0].GetUInt32();
					if (val != phase)
					{
						handler->SendSysMessage("You are not in this phase!");
						handler->SetSentErrorMessage(true);
						return false;
					}

					QueryResult objectPhase = WorldDatabase.PQuery("SELECT PhaseId FROM gameobject WHERE guid='%u'", object->GetGUID());

					QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='%u' LIMIT 1", player->GetSession()->GetAccountId(), (uint32)val);

					if (!result)
					{
						handler->SendSysMessage("You must be added to this phase before you can add creatures.");
						handler->SetSentErrorMessage(true);
						return false;
					}

					if (result)
					{
						do
						{
							if (val == 0)
							{
								handler->SendSysMessage("You cannot delete objects in the main phase!");
								handler->SetSentErrorMessage(true);
								return false;
							}

							Field * fields = result->Fetch();
							if (fields[0].GetInt32() == 0)
							{
								handler->SendSysMessage("You must be added to this phase before you can delete objects.");
								handler->SetSentErrorMessage(true);
								return false;
							}

							Field * objFields = result->Fetch();
							if (objFields[0].GetInt32() == (uint32)val)
							{
								handler->SendSysMessage("That object is not in your current phase!");
								handler->SetSentErrorMessage(true);
								return false;
							}
						} while (result->NextRow());
					}
				} while (res->NextRow());
			}
		}

		ObjectGuid ownerGuid = object->GetOwnerGUID();
		if (ownerGuid)
		{
			Unit* owner = ObjectAccessor::GetUnit(*handler->GetSession()->GetPlayer(), ownerGuid);
			if (!owner || !ownerGuid.IsPlayer())
			{
				handler->PSendSysMessage(LANG_COMMAND_DELOBJREFERCREATURE, ownerGuid.GetCounter(), object->GetGUIDLow());
				handler->SetSentErrorMessage(true);
				return false;
			}

			owner->RemoveGameObject(object, false);
		}

		object->SetRespawnTime(0);                                 // not save respawn time
		object->Delete();
		object->DeleteFromDB();

		handler->PSendSysMessage(LANG_COMMAND_DELOBJMESSAGE, object->GetGUIDLow());

		return true;
	};
#endif
};

void AddSC_system_phase()
{
	new phasing_system;
}
