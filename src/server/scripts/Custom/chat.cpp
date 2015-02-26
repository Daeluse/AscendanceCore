#include "ScriptPCH.h"
#include "Chat.h"
#include "Language.h"

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

		std::string nameLink = handler->playerLink(player->GetName());

		switch (player->GetSession()->GetSecurity())
		{
		case SEC_PLAYER:
			if (player->GetTeam() == ALLIANCE)
			{
				msg += "|cff00ff00[A] ";
				msg += nameLink;
				msg += " |cFFFFFFF0";
			}
			else
			{
				msg += "|cffff0000[H] ";
				msg += nameLink;
				msg += " |cFFFFFFF0";
			}
			break;
		case SEC_MODERATOR:
			msg += "|cffff8a00[Builder] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_GAMEMASTER:
			msg += "|cff00ffff[Dungeon Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_ADMINISTRATOR:
			msg += "|cfffa9900[Architect] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_EVENTMASTER:
			msg += "|cfffa9900[Event Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_LOREMASTER:
			msg += "|cfffa9900[Event Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_COMMUNITYREPRESENTATIVE:
			msg += "|cfffa9900[Community Representative] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_STAFFMEMBER:
			msg += "|cfffa9900[Community Manager] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_COUNCILMEMBER:
			msg += "|cfffa9900[Council Member] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_HEADDEVELOPER:
			msg += "|cfffa9900[Head Developer] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
			break;
		case SEC_CONSOLE:
			msg += "|cfffa9900[ROOT] ";
			msg += nameLink;
			msg += " |cFFFFFFF0";
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

