#include "ScriptPCH.h"
#include "Chat.h"

class chat : public CommandScript
{
public:
	chat() : CommandScript("chat"){}

	ChatCommand * GetCommands() const
	{
		static ChatCommand ChatCommandTable[] =
		{
			{ "chat", SEC_PLAYER, true, &HandleChatCommand, "", NULL },
			{ NULL, 0, false, NULL, "", NULL }
		};

		return ChatCommandTable;
	}

	static bool HandleChatCommand(ChatHandler * handler, const char * args)
	{
		if (!args)
			return false;

		std::string msg = "";
		Player * player = handler->GetSession()->GetPlayer();


		switch (player->GetSession()->GetSecurity())
		{
		case SEC_PLAYER:
			if (player->GetTeam() == ALLIANCE)
			{
				msg += "|cff00ff00[A] |cFF00FFFF[";
				msg += player->GetName();
				msg += "] |cFFFFFFF0";
			}
			else
			{
				msg += "|cffff0000[H] |cFF00FFFF[";
				msg += player->GetName();
				msg += "] |cFFFFFFF0";
			}
			break;
		case SEC_MODERATOR:
			msg += "|cffff8a00[Builder] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_GAMEMASTER:
			msg += "|cff00ffff[Dungeon Master] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_ADMINISTRATOR:
			msg += "|cfffa9900[Architect] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_EVENTMASTER:
			msg += "|cfffa9900[Event Master] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_LOREMASTER:
			msg += "|cfffa9900[Event Master] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_COMMUNITYREPRESENTATIVE:
			msg += "|cfffa9900[Community Representative] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_STAFFMEMBER:
			msg += "|cfffa9900[Community Manager] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_COUNCILMEMBER:
			msg += "|cfffa9900[Council Member] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_HEADDEVELOPER:
			msg += "|cfffa9900[Head Developer] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;
		case SEC_CONSOLE:
			msg += "|cfffa9900[ROOT] |cFF00FFFF[";
			msg += player->GetName();
			msg += "] |cFFFFFFF0";
			break;

		}

		msg += args;
		sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str(), 0);

		return true;
	}
};

void AddSC_chat()
{
	new chat();
}

