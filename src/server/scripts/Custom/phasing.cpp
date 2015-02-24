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
	if (IsMember == false)
	{
		ss << "INSERT INTO phase VALUES("
			<< "'" << player->GetGUID() << "',"
			<< "'" << player->GetName() << "',"
			<< "'" << phase << "',"
			<< "'" << phase << "',"
			<< "'" << phase << "',"
			<< "'" << "0" << "',"
			<< "'" << player->GetName() << "');";

	}
	else
	{
		ss << "INSERT INTO phase_members VALUES("
			<< "'" << player->GetGUID() << "',"
			<< "'" << player->GetName() << "',"
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
			{ "npcdelete", rbac::RBAC_PERM_COMMAND_PHASE_DeleteNPC, false, &HandlePhaseDeleteNpcCommand, "", NULL },
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

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 0.|r (Default Phase)");

			return true;
		}
		else if (phase == 1)
		{
			player->ClearPhases();
			player->SetInPhase(phase, true, !player->IsInPhase(phase));
			player->ToPlayer()->SendUpdatePhasing();

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 1.|r (Main Event Phase)");

			return true;
		}
		else if (phase == 2)
		{
			player->ClearPhases();
			player->SetInPhase(phase, true, !player->IsInPhase(phase));
			player->ToPlayer()->SendUpdatePhasing();

			CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
			chat->PSendSysMessage("|cff4169E1You are now entering phase 2.|r (Main Interior Phase)");

			return true;
		}

		QueryResult hasPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetGUID());
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

		QueryResult isCompleted = CharacterDatabase.PQuery("SELECT has_completed,guid,phase,phase_name FROM phase WHERE phase='%u'", phase);
		if (!isCompleted)
			return false;

		if (isCompleted)
		{
			do
			{
				Field * fields = isCompleted->Fetch();
				const char * phaseName = fields[3].GetCString();

				if (!phaseName) // if the phase is unnamed
				{
					phaseName = "Unnamed";
				}

				if (fields[0].GetUInt16() == 1) // if the phase is completed
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
					chat->PSendSysMessage("|cff4169E1You are now entering phase %u.|r (%s)", phase, phaseName);

					return true;
				}
				else if (player->GetGUID() == fields[1].GetUInt64() && fields[2].GetUInt32() == phase)
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
					chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r (%s)", phase, phaseName);

					return true;
				}
				else // if the phase isn't completed
				{
					QueryResult isMember = CharacterDatabase.PQuery("SELECT guid,phase FROM phase_members WHERE phase='%u'", phase);
					Field * members = isMember->Fetch();
					if (members[0].GetUInt16() == player->GetGUID()) // if the player is a member
					{
						player->ClearPhases();
						player->SetInPhase(phase, true, !player->IsInPhase(phase));
						player->ToPlayer()->SendUpdatePhasing();

						CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
						chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r (%s)", phase, phaseName);

						return true;
					}
					else if (!members[0].GetUInt16() == player->GetGUID()) // if the player is not a member
					{
						chat->PSendSysMessage("|cffFF0000This phase isn't completed yet!|r \n Phase: %u", phase);

						chat->SetSentErrorMessage(true);
						return false;
					}
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

		CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", 0, player->GetGUID());
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
		QueryResult check = CharacterDatabase.PQuery("SELECT phase_owned FROM phase WHERE guid='%u'", player->GetGUID());

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
		QueryResult res = CharacterDatabase.PQuery("SELECT * FROM phase WHERE guid='%u' LIMIT 1", player->GetGUID());
		if (!res)
			return false;

		CharacterDatabase.PExecute("DELETE FROM phase WHERE (guid='%u')", player->GetGUID());
		CharacterDatabase.PExecute("DELETE FROM phase_members WHERE (guid='%u')", player->GetGUID());
		chat->SendSysMessage("|cffFFFF00Your phase has now been deleted.|r");
		return true;
	};

	static bool HandlePhaseGetCommand(ChatHandler * chat, const char * args)
	{
		Player * player = chat->GetSession()->GetPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT COUNT(*),get_phase,phase,phase_name FROM phase WHERE guid='%u'", player->GetGUID());
		if (!getPhaseAndOwnedPhase)
			return false;

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
				chat->PSendSysMessage("|cffADD8E6Current Phase: %u \n Owned Phase: %u (%s)|r", fields[1].GetUInt32(), fields[2].GetUInt32(), fields[3].GetCString());
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

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetGUID());
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
			CharacterDatabase.PExecute("UPDATE phase SET has_completed='1' WHERE guid='%u'", player->GetGUID());
		}
		else if (argstr == "off")
		{
			handler->SendSysMessage("|cffFFA500Your phase is no longer public!|r");
			CharacterDatabase.PExecute("UPDATE phase SET has_completed='0' WHERE guid='%u'", player->GetGUID());
		}
		return true;
	};

	static bool HandlePhaseNameCommand(ChatHandler* handler, const char* args)
	{
		std::string argstr = (char*)args;

		Player * player = handler->GetSession()->GetPlayer();

		if (!*args)
			return false;

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE guid='%u'", player->GetGUID());
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
		CharacterDatabase.PExecute("UPDATE phase SET phase_name='%s' WHERE guid='%u'", args, player->GetGUID());
		return true;
	};

	static bool HandlePhaseAddMemberCommand(ChatHandler * handler, const char * /*args*/)
	{

		Player * player = handler->GetSession()->GetPlayer();
		Player * target = handler->getSelectedPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT get_phase, phase_owned FROM phase WHERE guid='%u'", player->GetGUID());
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

	static bool HandlePhaseAddNpcCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;
		char* charID = handler->extractKeyFromLink((char*)args, "Hcreature_entry");
		if (!charID)
			return false;

		char* team = strtok(NULL, " ");
		int32 teamval = 0;
		if (team) { teamval = atoi(team); }
		if (teamval < 0) { teamval = 0; }

		uint32 id = atoi(charID);

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
			uint32 phase = 0;

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
			if (phase == 0)
				return false;

			QueryResult res;
			res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", chr->GetGUID());
			if (res)
			{
				do
				{
					Field * fields = res->Fetch();
					uint32 val = fields[0].GetUInt32();
					if (val != phase)
					{
						handler->SendSysMessage("You are not in this phase!");
						return false;
					}

					QueryResult result;
					result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
						chr->GetGUID(), (uint32)val);

					if (result)
					{
						do
						{
							Field * fields = result->Fetch();
							if (fields[0].GetInt32() == 0)
							{
								handler->SendSysMessage("You must be added to this phase before you can build.");
								return false;
							}
						} while (result->NextRow());
					}
				} while (res->NextRow());
			}
		}

		//creature->CopyPhaseFrom(chr); // creature is not directly added to world, only to db, so this is useless here

		creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMask());

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
	};

	static bool HandlePhaseDeleteNpcCommand(ChatHandler * handler, const char * args)
	{
		Creature* unit = NULL;
		Player * pl = handler->GetSession()->GetPlayer();
		if (*args)
		{
			char * phase = strtok(NULL, " ");
			if (!phase)
				return false;

			if (phase)
			{
				uint32 value = atoi((char*)phase);
				if (value == 0)
					return false;

				QueryResult res;
				res = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE guid='%u'", pl->GetGUID());
				if (res)
				{
					do
					{
						Field * fields = res->Fetch();
						uint32 val = fields[0].GetUInt32();
						if (val != value)
						{
							handler->SendSysMessage("You are not in this phase!");
							return false;
						}

						QueryResult result;
						result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
							pl->GetGUID(), (uint32)val);

						if (result)
						{
							do
							{
								Field * fields = result->Fetch();
								if (fields[0].GetInt32() == 0)
								{
									handler->SendSysMessage("You must be added to this phase before you can target a go.");
									return false;
								}
							} while (result->NextRow());
						}
					} while (res->NextRow());
				}
			}
		}
		else
			unit = handler->getSelectedCreature();

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
	};
};

void AddSC_system_phase()
{
	new phasing_system;
}