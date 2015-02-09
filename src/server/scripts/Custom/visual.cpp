#include "ScriptPCH.h"
#include "Config.h"
#include "Language.h"

bool       RequiredToken;
int        SToken;
uint32     SpellToken;
uint32     Visual1;
uint32     Visual2;
uint32     Visual3;
uint32     Visual4;
uint32     Visual5;
uint32     Visual6;
uint32     Visual7;
uint32     Visual8;
uint32     Visual9;
uint32     Visual10;

class VisualNPC : public CreatureScript
{
public:

	VisualNPC() : CreatureScript("Visual_NPC"){}

	bool OnGossipHello(Player * pPlayer, Creature * pCreature)
	{

		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual1", GOSSIP_SENDER_MAIN, 0);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual2", GOSSIP_SENDER_MAIN, 1);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual3", GOSSIP_SENDER_MAIN, 2);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual4", GOSSIP_SENDER_MAIN, 3);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual5", GOSSIP_SENDER_MAIN, 4);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual6", GOSSIP_SENDER_MAIN, 5);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual7", GOSSIP_SENDER_MAIN, 6);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual8", GOSSIP_SENDER_MAIN, 7);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual9", GOSSIP_SENDER_MAIN, 8);
		pPlayer->ADD_GOSSIP_ITEM(4, "|cff00ff00|TInterface\\icons\\INV_Letter_18:30|t|r Visual10", GOSSIP_SENDER_MAIN, 9);
		pPlayer->PlayerTalkClass->SendGossipMenu(9425, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player * Player, Creature * Creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		if (!Player)
			return true;
		if (RequiredToken)
			switch (uiAction)
		{
			case 0:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual1, Player);
					Player->AddSpell(Visual1, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();

				}
				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 1:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual2, Player);
					Player->AddSpell(Visual2, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 2:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual3, Player);
					Player->AddSpell(Visual3, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 3:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual4, Player);
					Player->AddSpell(Visual4, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 4:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual5, Player);
					Player->AddSpell(Visual5, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 5:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual5, Player);
					Player->AddSpell(Visual5, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 6:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual7, Player);
					Player->AddSpell(Visual7, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 7:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual8, Player);
					Player->AddSpell(Visual8, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 8:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual9, Player);
					Player->AddSpell(Visual9, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
			case 9:
				if (Player->HasItemCount(SpellToken, SToken, true))
				{
					Player->DestroyItemCount(SpellToken, SToken, true, false);
					Player->AddAura(Visual10, Player);
					Player->AddSpell(Visual10, false, true, false, false);
					Player->GetSession()->SendNotification("|cffFFFF00You got the visual you wanted.");
					Player->PlayerTalkClass->SendCloseGossip();
				}

				else
				{
					Player->GetSession()->SendNotification("You do not have the required item to continue.");
					Player->PlayerTalkClass->SendCloseGossip();
				}
				break;
		}
		return true;
	}

};

class Script_VisualNPC_WorldScript : public WorldScript
{
public:
	Script_VisualNPC_WorldScript() : WorldScript("Script_VisualNPC_WorldScript") { }

	void OnConfigLoad(bool /*reload*/)
	{
		SpellToken = sConfigMgr->GetIntDefault("Spell.Token", 0);
		RequiredToken = sConfigMgr->GetBoolDefault("Required.Token", true);
		SToken = sConfigMgr->GetIntDefault("SpellCount.Token", 0);
		Visual1 = sConfigMgr->GetIntDefault("Visual.Spell1", 0);
		Visual2 = sConfigMgr->GetIntDefault("Visual.Spell2", 0);
		Visual3 = sConfigMgr->GetIntDefault("Visual.Spell3", 0);
		Visual4 = sConfigMgr->GetIntDefault("Visual.Spell4", 0);
		Visual5 = sConfigMgr->GetIntDefault("Visual.Spell5", 0);
		Visual6 = sConfigMgr->GetIntDefault("Visual.Spell6", 0);
		Visual7 = sConfigMgr->GetIntDefault("Visual.Spell7", 0);
		Visual8 = sConfigMgr->GetIntDefault("Visual.Spell8", 0);
		Visual9 = sConfigMgr->GetIntDefault("Visual.Spell9", 0);
		Visual10 = sConfigMgr->GetIntDefault("Visual.Spell10", 0);

	}
};

void AddSC_VisualNPC()
{
	new VisualNPC();
	new Script_VisualNPC_WorldScript;
}