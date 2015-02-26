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
		std::string msg = "";
		Player * player = handler->GetSession()->GetPlayer();

		QueryResult getWorldMute = LoginDatabase.PQuery("SELECT guid, worldmute FROM custom WHERE guid='%u'", player->GetSession()->GetAccountId());

		if (getWorldMute)
		{
			Field * fields = getWorldMute->Fetch();
			uint32 isMuted = fields[1].GetUInt32();

			if (isMuted == 1)
			{
				handler->SendSysMessage("You are not allowed to chat!");
				handler->SetSentErrorMessage(true);
				return false;
			}
		}

		std::string nameLink = handler->playerLink(player->GetName());

		switch (player->GetSession()->GetSecurity())
		{
		case SEC_PLAYER:
			if (player->GetTeam() == ALLIANCE)
			{
				msg += "|cff00ff00[A] ";
				msg += nameLink;
				msg += " |cFFFFFFF0 : |cFF66FFFF";
			}
			else
			{
				msg += "|cffff0000[H] ";
				msg += nameLink;
				msg += " |cFFFFFFF0 : |cFF66FFFF";
			}
			break;
		case SEC_MODERATOR:
			msg += "|cffff8a00[Builder] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_GAMEMASTER:
			msg += "|cff00ffff[Dungeon Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_ADMINISTRATOR:
			msg += "|cfffa9900[Architect] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_EVENTMASTER:
			msg += "|cfffa9900[Event Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_LOREMASTER:
			msg += "|cfffa9900[Event Master] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_COMMUNITYREPRESENTATIVE:
			msg += "|cfffa9900[Community Representative] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_STAFFMEMBER:
			msg += "|cfffa9900[Community Manager] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_COUNCILMEMBER:
			msg += "|cfffa9900[Council Member] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_HEADDEVELOPER:
			msg += "|cfffa9900[Head Developer] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;
		case SEC_CONSOLE:
			msg += "|cfffa9900[ROOT] ";
			msg += nameLink;
			msg += " |cFFFFFFF0 : |cFF66FFFF";
			break;

		}

		if (!args)
		{
			handler->PSendSysMessage("|cffFF0000You can not send blank messages!|r");
			handler->SetSentErrorMessage(true);
			return false;
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

