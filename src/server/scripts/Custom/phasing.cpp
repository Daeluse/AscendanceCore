#include "ScriptPCH.h"
#include "Chat.h"
#include <stdarg.h>
#include "GameObject.h"
#include "PoolMgr.h"
#include "ObjectAccessor.h"
#include "Transport.h"
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
			<< "'" << "0" << "');";
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

		QueryResult isCompleted = CharacterDatabase.PQuery("SELECT has_completed,guid,phase FROM phase WHERE phase='%u'", phase);
		if (!isCompleted)
			return false;

		if (isCompleted)
		{
			do
			{
				Field * fields = isCompleted->Fetch();
				if (fields[0].GetUInt16() == 1) // if the phase is completed
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
					chat->PSendSysMessage("|cff4169E1You are now entering phase %u.|r", phase);

					return true;
				}
				else if (player->GetGUID() == fields[1].GetUInt64() && fields[2].GetUInt32() == phase)
				{
					player->ClearPhases();
					player->SetInPhase(phase, true, !player->IsInPhase(phase));
					player->ToPlayer()->SendUpdatePhasing();

					CharacterDatabase.PExecute("UPDATE phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
					chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r", phase);

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
						chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r", phase);

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
		player->SetPhaseMask(phase, true);
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
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT COUNT(*),get_phase,phase FROM phase WHERE guid='%u'", player->GetGUID());
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
				chat->PSendSysMessage("|cffADD8E6Current Phase: %u \n Owned Phase: %u|r", fields[1].GetUInt32(), fields[2].GetUInt32());
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
		CharacterDatabase.PExecute("UPDATE phase SET phase_name='%u' WHERE guid='%u'", args, player->GetGUID());
		return true;
	};

	static bool HandlePhaseAddMemberCommand(ChatHandler * handler, const char * /*args*/)
	{

		Player * player = handler->GetSession()->GetPlayer();
		Player * target = handler->getSelectedPlayer();
		QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT COUNT(*),get_phase,phase FROM phase WHERE guid='%u'", player->GetGUID());
		Field * fields = getPhaseAndOwnedPhase->Fetch();
		uint32 phase = fields[2].GetUInt32();

		if (!target)
		{
			handler->SendSysMessage("You must select a player!");
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (target == player)
			return false;

		QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase WHERE player_name='%s'", target->GetName());
		if (isOwnerOfAPhase)
		{
			do
			{
				Field * fields = isOwnerOfAPhase->Fetch();
				if (fields[0].GetInt32() == 0)
				{
					handler->PSendSysMessage("|cffFF0000 %s does not own a phase; therefore, he/she cannot be added to your phase.|r", target->GetName());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwnerOfAPhase->NextRow());
		}

		QueryResult isMember = CharacterDatabase.PQuery("SELECT COUNT(*) FROM phase_members WHERE player_name='%s' AND phase='%u'", target->GetName(), phase);
		if (isMember)
		{
			do
			{
				Field * field = isMember->Fetch();
				if (field[0].GetInt32() == 1)
				{
					handler->PSendSysMessage("|cffFF0000 %s is already a member of your phase!|r", target->GetName());
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isMember->NextRow());
		}

		QueryResult isOwner = CharacterDatabase.PQuery("SELECT phase_owned FROM phase WHERE guid='%u'", player->GetGUID());
		if (isOwner)
		{
			do
			{
				Field * mfield = isOwner->Fetch();
				if (phase != mfield[0].GetUInt32())
				{
					handler->PSendSysMessage("|cffFF0000You cannot add yourself or anyone else to someone elses phase.|r");
					handler->SetSentErrorMessage(true);
					return false;
				}
			} while (isOwner->NextRow());
		}
		handler->PSendSysMessage("|cffFFA500You successfully added %s to your phase %u.|r", target->GetName(), phase);
		CreatePhase(target, true, phase);
		return true;
	};

	static bool HandlePhaseKickCommand(ChatHandler * chat, const char * args)
	{
		if (!*args)
			return false;

		Player * player = chat->GetSession()->GetPlayer();

		QueryResult phase = CharacterDatabase.PQuery("SELECT phase FROM phase WHERE guid='%u'", player->GetGUID());
		if (!phase)
			return false;

		if (phase)
		{
			do
			{
				Field * fields = phase->Fetch();
				QueryResult getplayer = CharacterDatabase.PQuery("SELECT get_phase FROM phase WHERE player_name='%s'", args);
				if (!getplayer)
					return false;

				char msg[255];

				Field * mfields = getplayer->Fetch();
				if (mfields[0].GetInt32() == fields[0].GetInt32())
				{
					Player * target = ObjectAccessor::FindPlayerByName(args);
					target->ClearPhases();
					target->ToPlayer()->SendUpdatePhasing();
					snprintf(msg, 255, "|cffFF0000You were kicked from phase %u by %s!|r", fields[0].GetInt32(), player->GetName());
					player->Whisper(msg, LANG_UNIVERSAL, target);
					CharacterDatabase.PExecute("UPDATE phases SET get_phase='0' WHERE (guid='%u')", player->GetGUID());
				}
				else
				{
					snprintf(msg, 255, "|cffFF0000%s is not currently in your phase!|r", args);
					chat->SendSysMessage(msg);
					chat->SetSentErrorMessage(true);
					return false;
				}
			} while (phase->NextRow());
		}
		return true;
	};
};

void AddSC_system_phase()
{
	new phasing_system;
}