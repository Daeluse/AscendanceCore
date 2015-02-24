#include "ScriptPCH.h"
#include "Chat.h"
#include <stdarg.h>
#include "GameObject.h"
#include "PoolMgr.h"
#include "ObjectAccessor.h"
#include "Transport.h"
using namespace std;

class refresh : public CommandScript
{
public:
	refresh() : CommandScript("refresh") { }

	ChatCommand * GetCommands() const
	{

		static ChatCommand refreshcommandTable[] =
		{
			{ "refresh", SEC_PLAYER, false, &HandleRefreshCommand, "", NULL },
		};
		return refreshcommandTable;
	}

	static bool HandleRefreshCommand(ChatHandler* handler, const char* /*args*/)
	{
		Unit* target = handler->GetSession()->GetPlayer();

		if (target->GetTypeId() == TYPEID_PLAYER)
			target->ToPlayer()->SendUpdatePhasing();

		return true;
	}
};

void AddSC_refresh()
{
	new refresh;
}