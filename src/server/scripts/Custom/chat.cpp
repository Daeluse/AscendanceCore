#include "ScriptPCH.h"
#include "Chat.h"
#include "Language.h"
#include "Config.h"

std::string PLAYER_H_PREFIX;
std::string PLAYER_H_SUFFIX;
std::string PLAYER_A_PREFIX;
std::string PLAYER_A_SUFFIX;
std::string BUILDER_PREFIX;
std::string BUILDER_SUFFIX;
std::string ARCHITECT_PREFIX;
std::string ARCHITECT_SUFFIX;
std::string DUNGEONMASTER_PREFIX;
std::string DUNGEONMASTER_SUFFIX;
std::string EVENTMASTER_PREFIX;
std::string EVENTMASTER_SUFFIX;
std::string LOREMASTER_PREFIX;
std::string LOREMASTER_SUFFIX;
std::string COMMUNITYREPRESENTATIVE_PREFIX;
std::string COMMUNITYREPRESENTATIVE_SUFFIX;
std::string COMMUNITYMANAGER_PREFIX;
std::string COMMUNITYMANAGER_SUFFIX;
std::string COUNCILMEMBER_PREFIX;
std::string COUNCILMEMBER_SUFFIX;
std::string HEADDEVELOPER_PREFIX;
std::string HEADDEVELOPER_SUFFIX;

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

		std::string nameLink = handler->playerLink(player->GetName());

		switch (player->GetSession()->GetSecurity())
		{
		case SEC_PLAYER:
			if (player->GetTeam() == ALLIANCE)
			{
				msg += PLAYER_H_PREFIX;
				msg += nameLink;
				msg += PLAYER_H_SUFFIX;
			}
			else
			{
				msg += PLAYER_A_PREFIX;
				msg += nameLink;
				msg += PLAYER_A_SUFFIX;
			}
			break;
		case SEC_MODERATOR:
			msg += BUILDER_PREFIX;
			msg += nameLink;
			msg += BUILDER_SUFFIX;
			break;
		case SEC_GAMEMASTER:
			msg += DUNGEONMASTER_PREFIX;
			msg += nameLink;
			msg += DUNGEONMASTER_SUFFIX;
			break;
		case SEC_ADMINISTRATOR:
			msg += ARCHITECT_PREFIX;
			msg += nameLink;
			msg += ARCHITECT_SUFFIX;
			break;
		case SEC_EVENTMASTER:
			msg += EVENTMASTER_PREFIX;
			msg += nameLink;
			msg += EVENTMASTER_SUFFIX;
			break;
		case SEC_LOREMASTER:
			msg += LOREMASTER_PREFIX;
			msg += nameLink;
			msg += LOREMASTER_SUFFIX;
			break;
		case SEC_COMMUNITYREPRESENTATIVE:
			msg += COMMUNITYREPRESENTATIVE_PREFIX;
			msg += nameLink;
			msg += COMMUNITYREPRESENTATIVE_SUFFIX;
			break;
		case SEC_STAFFMEMBER:
			msg += COMMUNITYMANAGER_PREFIX;
			msg += nameLink;
			msg += COMMUNITYMANAGER_SUFFIX;
			break;
		case SEC_COUNCILMEMBER:
			msg += COUNCILMEMBER_PREFIX;
			msg += nameLink;
			msg += COUNCILMEMBER_SUFFIX;
			break;
		case SEC_HEADDEVELOPER:
			msg += HEADDEVELOPER_PREFIX;
			msg += nameLink;
			msg += HEADDEVELOPER_SUFFIX;
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

class Script_chat_WorldScript : public WorldScript
{
public:
	Script_chat_WorldScript() : WorldScript("Script_chat_WorldScript") { }

	void OnConfigLoad(bool /*reload*/)
	{
		PLAYER_H_PREFIX = sConfigMgr->GetStringDefault("PLAYER_H_PREFIX", 0);
		PLAYER_H_SUFFIX = sConfigMgr->GetStringDefault("PLAYER_H_SUFFIX", 0);

		PLAYER_A_PREFIX = sConfigMgr->GetStringDefault("PLAYER_A_PREFIX", 0);
		PLAYER_A_SUFFIX = sConfigMgr->GetStringDefault("PLAYER_A_SUFFIX", 0);

		BUILDER_PREFIX = sConfigMgr->GetStringDefault("BUILDER_PREFIX", 0);
		BUILDER_SUFFIX = sConfigMgr->GetStringDefault("BUILDER_SUFFIX", 0);

		DUNGEONMASTER_PREFIX = sConfigMgr->GetStringDefault("DUNGEONMASTER_PREFIX", 0);
		DUNGEONMASTER_SUFFIX = sConfigMgr->GetStringDefault("DUNGEONMASTER_SUFFIX", 0);

		ARCHITECT_PREFIX = sConfigMgr->GetStringDefault("ARCHITECT_PREFIX", 0);
		ARCHITECT_SUFFIX = sConfigMgr->GetStringDefault("ARCHITECT_SUFFIX", 0);

		EVENTMASTER_PREFIX = sConfigMgr->GetStringDefault("EVENTMASTER_PREFIX", 0);
		EVENTMASTER_SUFFIX = sConfigMgr->GetStringDefault("EVENTMASTER_SUFFIX", 0);

		LOREMASTER_PREFIX = sConfigMgr->GetStringDefault("LOREMASTER_PREFIX", 0);
		LOREMASTER_SUFFIX = sConfigMgr->GetStringDefault("LOREMASTER_SUFFIX", 0);

		COMMUNITYREPRESENTATIVE_PREFIX = sConfigMgr->GetStringDefault("COMMUNITYREPRESENTATIVE_SUFFIX", 0);
		COMMUNITYREPRESENTATIVE_SUFFIX = sConfigMgr->GetStringDefault("COMMUNITYREPRESENTATIVE_SUFFIX", 0);

		COMMUNITYMANAGER_PREFIX = sConfigMgr->GetStringDefault("COMMUNITYMANAGER_PREFIX", 0);
		COMMUNITYMANAGER_SUFFIX = sConfigMgr->GetStringDefault("COMMUNITYMANAGER_SUFFIX", 0);

		COUNCILMEMBER_PREFIX = sConfigMgr->GetStringDefault("COUNCILMEMBER_PREFIX", 0);
		COUNCILMEMBER_SUFFIX = sConfigMgr->GetStringDefault("COUNCILMEMBER_SUFFIX", 0);

		HEADDEVELOPER_PREFIX = sConfigMgr->GetStringDefault("HEADDEVELOPER_PREFIX", 0);
		HEADDEVELOPER_SUFFIX = sConfigMgr->GetStringDefault("HEADDEVELOPER_SUFFIX", 0);

	}
};

void AddSC_chat()
{
	new chat();
}

