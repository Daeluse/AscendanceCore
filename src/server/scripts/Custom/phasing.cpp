/*Orginal Code By Nomsoftware
Updated By Amir_Cinderella <Amir.Cinderella@Gmail.com>
*/
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
			{ "join", rbac::RBAC_PERM_COMMAND_PHASE_Join, false, &HandlePhaseJoinCommand, "", NULL },
			{ "join", rbac::RBAC_PERM_COMMAND_PHASE_Leave, false, &HandlePhaseLeaveCommand, "", NULL },
			{ NULL, 0, false, NULL, "", NULL }
		};

		static ChatCommand phasecommandTable[] =
		{
			{ "phase", rbac::RBAC_PERM_COMMAND_PHASE, false, NULL, "", phaseCmdTable },
			{ NULL, 0, false, NULL, "", NULL }
		};
		return phasecommandTable;
	}

	static bool HandlePhaseJoinCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		uint32 phase = (uint32)atoi((char*)args);

		Unit* target = handler->getSelectedUnit();
		if (!target)
			target = handler->GetSession()->GetPlayer();

		target->SetInPhase(phase, true, !target->IsInPhase(phase));

		if (target->GetTypeId() == TYPEID_PLAYER)
			target->ToPlayer()->SendUpdatePhasing();

		return true;
	}

	static bool HandlePhaseLeaveCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		uint32 phase = (uint32)atoi((char*)args);

		Unit* target = handler->getSelectedUnit();
		if (!target)
			target = handler->GetSession()->GetPlayer();

		target->ClearPhases();

		if (target->GetTypeId() == TYPEID_PLAYER)
			target->ToPlayer()->SendUpdatePhasing();

		return true;
	}
};

void AddSC_system_phase()
{
	new phasing_system;
}