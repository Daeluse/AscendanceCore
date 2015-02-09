#include "ScriptPCH.h"
#include "Config.h"
#include "Language.h"

bool       DefaultToken;
int        RToken;
int        FToken;
int        CToken;
int        ReToken;
uint32     IDItem;

class npc_changer : public CreatureScript
{
public:

	npc_changer() : CreatureScript("npc_changer"){}

	bool OnGossipHello(Player * pPlayer, Creature * pCreature)
	{


		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\Ability_Rogue_Disguise:30|t|r Change My Race ", GOSSIP_SENDER_MAIN, 0);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\Achievement_BG_winWSG:30|t|r Change My Faction", GOSSIP_SENDER_MAIN, 1);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Misc_GroupLooking:30|t|r Customize My Character", GOSSIP_SENDER_MAIN, 2);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Change My Name", GOSSIP_SENDER_MAIN, 3);
		pPlayer->PlayerTalkClass->SendGossipMenu(9425, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player * Player, Creature * Creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		if (!Player)
			return true;
		if (DefaultToken)
			switch (uiAction)
		{
			case 0:
				if (Player->HasItemCount(IDItem, RToken, true))
				{
					Player->DestroyItemCount(IDItem, RToken, true, false);
					Player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
					Player->GetSession()->SendNotification("|cffFFFF00You Need to Relog , To Change Your Race!");
					Player->PlayerTalkClass->SendCloseGossip();

				}
				else
				{
					Player->GetSession()->SendNotification("You don't Have Enough item for Change Race!");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 1:
				if (Player->HasItemCount(IDItem, FToken, true))
				{
					Player->DestroyItemCount(IDItem, FToken, true, false);
					Player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
					Player->GetSession()->SendNotification("|cffFFFF00You Need To Relog , To change Tour Faction!");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				else
				{
					Player->GetSession()->SendNotification("You don't Have Enough item for Change Faction!");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 2:
				if (Player->HasItemCount(IDItem, CToken, true))
				{
					Player->DestroyItemCount(IDItem, CToken, true, false);
					Player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
					Player->GetSession()->SendNotification("|cffFFFF00You Need To Relog , To Change Your Customize!");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You don't Have Enough item for Change Customize!");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 3:
				if (Player->HasItemCount(IDItem, ReToken, true))
				{
					Player->DestroyItemCount(IDItem, ReToken, true, false);
					Player->SetAtLoginFlag(AT_LOGIN_RENAME);
					Player->GetSession()->SendNotification("|cffFFFF00You Need To Relog , To Change Your Name!");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You don't Have Enough item for Change Name!");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
		}
		return true;
	}

};

class Script_npc_changer_WorldScript : public WorldScript
{
public:
	Script_npc_changer_WorldScript() : WorldScript("Script_npc_changer_WorldScript") { }

	void OnConfigLoad(bool /*reload*/)
	{
		DefaultToken = sConfigMgr->GetBoolDefault("Default.Token", true);
		RToken = sConfigMgr->GetIntDefault("Race.Token", 0);
		FToken = sConfigMgr->GetIntDefault("Faction.Token", 0);
		CToken = sConfigMgr->GetIntDefault("Customize.Token", 0);
		ReToken = sConfigMgr->GetIntDefault("Rename.Token", 0);
		IDItem = sConfigMgr->GetIntDefault("IDItem", 0);

	}
};

void AddSC_npc_changer()
{
	new npc_changer();
	new Script_npc_changer_WorldScript;

}

