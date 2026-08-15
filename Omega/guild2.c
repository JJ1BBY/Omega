/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* guild2.c */
/* L_ functions  */ 

/* These functions implement the various guilds. */
/* They are all l_ functions since they are basically activated*/
/* at some site or other. */

#include "glob.h"


void l_thieves_guild()
{
  int fee,count,i,number,done=FALSE,dues=1000;
  char c,action;
  pob lockpick;
  print1(LS(IDS_MSG_21146));
  if (! nighttime()) 
    print2(LS(IDS_MSG_21147));
  else {
    if ((Player.rank[THIEVES]==TMASTER) &&
	(Player.level > Shadowlordlevel) &&
	find_and_remove_item(THINGID+16,-1)) {
      print2(LS(IDS_MSG_21148));
      morewait();
      print1(LS(IDS_MSG_21149));
      print2(LS(IDS_MSG_21150));
      morewait();
      print1(LS(IDS_MSG_21151));
      strcpy(Shadowlord,Player.name);
      Shadowlordlevel = Player.level;
      morewait();
      Shadowlordbehavior = fixnpc(4);
      save_hiscore_npc(7);
      clearmsg();
      print1(LS(IDS_MSG_21152));
      Spells[S_SHADOWFORM].known = TRUE;
      morewait();
      clearmsg();
      Player.rank[THIEVES]=SHADOWLORD;
      Player.maxagi += 2;
      Player.maxdex += 2;
      Player.agi += 2;
      Player.dex += 2;
    }
    while (! done) {
      menuclear();
      if (Player.rank[THIEVES] == 0)
	menuprint("a: Join the Thieves' Guild.\n");
      else
	menuprint("b: Raise your Guild rank.\n");
      menuprint("c: Get an item identified.\n");
      if (Player.rank[THIEVES] > 0)
	menuprint("d: Fence an item.\n");
      menuprint("ESCAPE: Leave this Den of Iniquity.");
      showmenu();
      action = mgetc();
      if (action == ESCAPE) done = TRUE;
      else if (action == 'a') {
	done = TRUE;
	if (Player.rank[THIEVES]> 0)
	  print2(LS(IDS_MSG_21153));
	else if (Player.alignment > 10) 
	  print2(LS(IDS_MSG_21154));
	else {
	  dues += dues*(12-Player.dex)/9;
	  dues += Player.alignment*5;
	  dues = max(100,dues);
	  clearmsg();
	  mprint(LS(IDS_MSG_21155));
	  mnumprint(dues);
	  mprint(LS(IDS_MSG_21156));
	  if (ynq1() =='y') {
	    if (Player.cash < dues) {
	      print1(LS(IDS_MSG_21157));
	      print2(LS(IDS_MSG_21158));
	      Player.cash = 0;
	    }
	    else {
	      print1(LS(IDS_MSG_21159));
	      nprint1(Shadowlord);
	      print2(LS(IDS_MSG_21160)); 
	      morewait();
	      clearmsg();
	      print1(LS(IDS_MSG_21161));
	      print2(LS(IDS_MSG_21162));
	      morewait();
	      Spells[S_OBJ_DET].known = TRUE; 
	      lockpick = ((pob) checkmalloc(sizeof(objtype)));
	      *lockpick = Objects[THINGID+2]; /* lock pick */
	      gain_item(lockpick);
	      Player.cash -= dues;
	      dataprint();
	      Player.guildxp[THIEVES]=1;
	      Player.rank[THIEVES]=TMEMBER;
	      Player.maxdex++;Player.dex++;Player.agi++;Player.maxagi++;
	    }
	  }
	}
      }
      else if (action == 'b') {
	if (Player.rank[THIEVES]==0)
	  print2(LS(IDS_MSG_21163));
	else if (Player.rank[THIEVES]==SHADOWLORD) 
	  print2(LS(IDS_MSG_21164));
	else if (Player.rank[THIEVES]==TMASTER) {
	  if (Player.level <= Shadowlordlevel)
	    print2(LS(IDS_MSG_21165));
	  else print2(LS(IDS_MSG_21166));
	}
	else if (Player.rank[THIEVES]==THIEF) {
	  if (Player.guildxp[THIEVES] < 4000)
	    print2(LS(IDS_MSG_21165));
	  else  {
	    print1(LS(IDS_MSG_21167));
	    print2(LS(IDS_MSG_21168));
	    morewait();
	    print1(LS(IDS_MSG_21169));
	    print2(LS(IDS_MSG_21170));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_21171));
	    Spells[S_APPORT].known = TRUE;
	    Player.rank[THIEVES]=TMASTER;
	    Player.maxagi++;
	    Player.maxdex++;
	    Player.agi++;
	    Player.dex++;
	  }
	}
	else if (Player.rank[THIEVES]==ATHIEF) {	
	  if (Player.guildxp[THIEVES] < 1500)
	    print2(LS(IDS_MSG_21165));
	  else  {
	    print1(LS(IDS_MSG_21172));
	    print2(LS(IDS_MSG_21173));
	    Spells[S_INVISIBLE].known = TRUE;
	    Player.rank[THIEVES]=THIEF;
	    Player.agi++;
	    Player.maxagi++;
	  }
	}
	else if (Player.rank[THIEVES]==TMEMBER) {
	  if (Player.guildxp[THIEVES] < 400)
	    print2(LS(IDS_MSG_21165));
	  else {
	    print1(LS(IDS_MSG_21174));
	    print2(LS(IDS_MSG_21175));
	    Spells[S_LEVITATE].known = TRUE;
	    Player.rank[THIEVES]=ATHIEF;
	    Player.dex++;
	    Player.maxdex++;
	  }
	}
      }
      else if (action == 'c') {
	if (Player.rank[THIEVES]==0) {
	  print1(LS(IDS_MSG_21176));
	  fee = 50;
	}
	else {
	  fee = 5;
	  print1(LS(IDS_MSG_21177));
	}
	print2(LS(IDS_MSG_21178));
	if ((char) mcigetc()=='i') {
	  if (Player.cash < fee)
	    print2(LS(IDS_MSG_21179));
	  else {
	    Player.cash -= fee;
	    dataprint();
	    identify(0);
	  }
	}
	else {
	  count = 0;
	  for(i=1;i<MAXITEMS;i++)
	    if (Player.possessions[i] != NULL)
	      if (Player.possessions[i]->known < 2)
		count++;
	  for(i=0;i<Player.packptr;i++)
	    if (Player.pack[i] != NULL)
	      if (Player.pack[i]->known < 2)
		count++;
	  clearmsg();
	  print1(LS(IDS_MSG_21180));
	  mnumprint(max(count*fee,fee));
	  nprint1(LS(IDS_MSG_21181));
	  if (ynq1()=='y')
	  if (Player.cash < max(count*fee,fee))
	    print2(LS(IDS_MSG_21179));
	  else {
	    Player.cash -= max(count*fee,fee);
	    dataprint();
	    identify(1);
	  }
	}
      }
      else if (action == 'd') {
	if (Player.rank[THIEVES]==0)
	  print2(LS(IDS_MSG_21182));
	else {
	  print1(LS(IDS_MSG_21183));
	  if ((char) mcigetc()=='i') {
	    i = getitem(NULL_ITEM);
	    if ((i==ABORT) || (Player.possessions[i] == NULL))
	      print2(LS(IDS_MSG_21184));
	    else if (Player.possessions[i]->blessing < 0) 
	      print2(LS(IDS_MSG_21185));
	    else {
	      clearmsg();
	      print1(LS(IDS_MSG_21186));
	      mlongprint(2 * item_value(Player.possessions[i]) / 3);
	      nprint1(LS(IDS_MSG_21187));
	      if (ynq1() == 'y') {
		number = getnumber(Player.possessions[i]->number);
		if ((number >= Player.possessions[i]->number) &&
		    Player.possessions[i]->used) {
		  Player.possessions[i]->used = FALSE;
		  item_use(Player.possessions[i]);
		}
		Player.cash += number*2*item_value(Player.possessions[i])/3;
		dispose_lost_objects(number,Player.possessions[i]);
		dataprint();
	      }
	      else print2(LS(IDS_MSG_21188));
	    }
	  }
	  else {
	    for(i=0;i<Player.packptr;i++) {
	      if (Player.pack[i]->blessing > -1) {
		clearmsg();
		print1(LS(IDS_MSG_21189));
		nprint1(itemid(Player.pack[i]));
		nprint1(LS(IDS_MSG_21190));
		mlongprint(2*item_value(Player.pack[i])/3);
		nprint1(LS(IDS_MSG_21191));
		if ((c=ynq1())=='y') {
		  number = getnumber(Player.pack[i]->number);
		  Player.cash += 2*number * item_value(Player.pack[i]) / 3;
		  Player.pack[i]->number -= number;
		  if (Player.pack[i]->number < 1) {
		    free((char *)Player.pack[i]);
		    Player.pack[i] = NULL;
		  }
		  dataprint();
		}
		else if (c=='q') break;
	      }
	    }
	    fixpack();
	  }
	}
      }
    }
  }
  xredraw();
}

void l_college()
{
  char action;
  int done=FALSE,enrolled = FALSE;
  print1(LS(IDS_MSG_21192));
  if (nighttime())
    print2(LS(IDS_MSG_21193));
  else {
    while (! done) {
      if ((Player.rank[COLLEGE]==MAGE) &&
	  (Player.level > Archmagelevel) &&
	  find_and_remove_item(CORPSEID,ML10+1)) {
	print1(LS(IDS_MSG_21194));
	morewait();
	print1(LS(IDS_MSG_21195));
	print2(LS(IDS_MSG_21196));
	morewait();
	clearmsg();
	strcpy(Archmage,Player.name);
	Archmagelevel = Player.level;
	Player.rank[COLLEGE] = ARCHMAGE;
	Player.maxiq += 5;
	Player.iq += 5;
	Player.maxpow += 5;
	Player.pow += 5;
	morewait();
	Archmagebehavior = fixnpc(4);
	save_hiscore_npc(9);
      }
      menuclear();
      menuprint("May we help you?\n\n");
      menuprint("a: Enroll in the College.\n");
      menuprint("b: Raise your College rank.\n");
      menuprint("c: Do spell research.\n");
      menuprint("ESCAPE: Leave these hallowed halls.\n");
      showmenu();
      action = mgetc();
      if (action == ESCAPE) done = TRUE;
      else if (action == 'a') {
	if (Player.rank[COLLEGE] > 0)
	  print2(LS(IDS_MSG_21197));
	else if (Player.iq < 13) 
	  print2(LS(IDS_MSG_21198));
	else if (Player.rank[CIRCLE] > 0)
	  print2(LS(IDS_MSG_21199));
	else {
	  if (Player.iq > 17) {
	    print2(LS(IDS_MSG_21200));
	    morewait();
	    enrolled=TRUE;
	  }
	  else {
	    print1(LS(IDS_MSG_21201));
	    nprint1(LS(IDS_MSG_21202));
	    if (ynq1() =='y') {
	      if (Player.cash < 1000)
		print2(LS(IDS_MSG_21203));
	      else {
		Player.cash -= 1000;
		enrolled = TRUE;
		dataprint();
	      }		
	    }
	  }
	  if (enrolled) {
	    print1(LS(IDS_MSG_21204));
	    nprint1(Archmage);
	    nprint1(LS(IDS_MSG_21205));
	    print2(LS(IDS_MSG_21206));
	    morewait();
	    print1(LS(IDS_MSG_21207));
	    print2(LS(IDS_MSG_21208));
	    Spellsleft = 1;
	    Player.rank[COLLEGE] = INITIATE;
	    Player.guildxp[COLLEGE] = 1;
	    Player.maxiq += 1;
	    Player.iq += 1;
	    Player.maxpow += 1;
	    Player.pow += 1;
	  }
	}
      }
      else if (action == 'b') {
	if (Player.rank[COLLEGE] == 0)
	  print2(LS(IDS_MSG_21209));
	else if (Player.rank[COLLEGE]==ARCHMAGE) 
	  print2(LS(IDS_MSG_21210));
	else if (Player.rank[COLLEGE]==MAGE) {
	  if (Player.level <= Archmagelevel)
	    print2(LS(IDS_MSG_21165));
	  else
	    print2(LS(IDS_MSG_21211));
	}
	else if (Player.rank[COLLEGE]==PRECEPTOR) {
	  if (Player.guildxp[COLLEGE] < 4000)
	    print2(LS(IDS_MSG_21165));
	  else  {
	    print1(LS(IDS_MSG_21212));
	    print2(LS(IDS_MSG_21213));
	    Spellsleft += 6;
	    morewait();
	    print1(LS(IDS_MSG_21214));
	    print2(LS(IDS_MSG_21215));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_21216));
	    Player.rank[COLLEGE] = MAGE;
	    Player.maxiq += 2;
	    Player.iq += 2;
	    Player.maxpow += 2;
	    Player.pow += 2;
	  }
	}
	else if (Player.rank[COLLEGE]==STUDENT) {
	  if (Player.guildxp[COLLEGE] < 1500)
	    print2(LS(IDS_MSG_21165));
	  else  {
	    print1(LS(IDS_MSG_21217));
	    print2(LS(IDS_MSG_21218));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_21219));
	    Spellsleft +=4;
	    Spells[S_RITUAL].known = TRUE;
	    Player.rank[COLLEGE] = PRECEPTOR;
	    Player.maxiq += 1;
	    Player.iq += 1;
	    Player.maxpow += 1;
	    Player.pow += 1;
	  }
	}
	else if (Player.rank[COLLEGE]==NOVICE) {
	  if (Player.guildxp[COLLEGE] < 400)
	    print2(LS(IDS_MSG_21165));
	  else  {
	    print1(LS(IDS_MSG_21220));
	    print2(LS(IDS_MSG_21221));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_21222));
	    Spellsleft+=2;
	    Spells[S_IDENTIFY].known = TRUE;
	    Player.rank[COLLEGE] = STUDENT;
	    Player.maxiq += 1;
	    Player.iq += 1;
	    Player.maxpow += 1;
	    Player.pow += 1;
	  }
	}
      }
      else if (action == 'c') {
	clearmsg();
	if (Spellsleft > 0) {
	  print1(LS(IDS_MSG_21223));
	  mnumprint(Spellsleft);
	  nprint1(LS(IDS_MSG_21224));
	  morewait();
	}
	if (Spellsleft < 1) {
	  print1(LS(IDS_MSG_21225));
	  nprint1(LS(IDS_MSG_21202));
	  if (ynq1()=='y') {
	    if (Player.cash < 2000) 
	      print1(LS(IDS_MSG_21179));
	    else {
	      Player.cash -= 2000;
	      dataprint();
	      Spellsleft = 1;
	    }
	  }
	}
	if (Spellsleft > 0) {
	  learnspell(0);
	  Spellsleft--;
	}
      }
    }
  }
  xredraw();
}



void l_sorcerors()
{
  char action;
  int done=FALSE,fee=3000;
  long total;
  print1(LS(IDS_MSG_21226));
  if (Player.rank[CIRCLE] == -1) {
    print2(LS(IDS_MSG_21227));
    Player.mana = 0;
    dataprint();
  }
  else  while (! done) {
    if ((Player.rank[CIRCLE]==HIGHSORCEROR) &&
	(Player.level > Primelevel) &&
	find_and_remove_item(CORPSEID,ML10+2)) {
      print2(LS(IDS_MSG_21228));
      morewait();
      print1(LS(IDS_MSG_21229));
      print2(LS(IDS_MSG_21230));
      strcpy(Prime,Player.name);
      Primelevel = Player.level;
      morewait();
      Primebehavior = fixnpc(4);
      save_hiscore_npc(10);
      clearmsg();
      print1(LS(IDS_MSG_21231));
      morewait();
      clearmsg();
      Spells[S_DISINTEGRATE].known = TRUE;
      Player.rank[CIRCLE] = PRIME;
      Player.maxpow += 10;
      Player.pow += 10;
    }
    menuclear();
    menuprint("May we help you?\n\n");
    menuprint("a: Become an Initiate of the Circle.\n");
    menuprint("b: Raise your rank in the Circle.\n");
    menuprint("c: Restore mana points\n");
    menuprint("ESCAPE: Leave these Chambers of Power.\n");
    showmenu();
    action = mgetc();
    if (action == ESCAPE) done = TRUE;
    else if (action == 'a') {
      if (Player.rank[CIRCLE] > 0)
	  print2(LS(IDS_MSG_21232));
      else if (Player.alignment > 0)
	print2(LS(IDS_MSG_21233));
      else if (Player.rank[COLLEGE] != 0)
	print2(LS(IDS_MSG_21234));
      else {
	fee += Player.alignment*100;
	fee += fee*(12 - Player.pow)/9;
	fee = max(100,fee);
	clearmsg();
	mprint(LS(IDS_MSG_21235));
	mnumprint(fee);
	mprint(LS(IDS_MSG_21236));
	print2(LS(IDS_MSG_21202));
	if (ynq2() =='y') {
	  if (Player.cash < fee) 
	    print3(LS(IDS_MSG_21237));
	  else {
	    print1(LS(IDS_MSG_21238));
	    nprint1(Prime);
	    print2(LS(IDS_MSG_21239));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_21240));
	    Spells[S_MISSILE].known = TRUE;
	    Player.cash -= fee;
	    dataprint();
	    Player.rank[CIRCLE] = INITIATE;
	    Player.guildxp[CIRCLE] = 1;
	    Player.maxpow++;
	    Player.pow++;
	  }
	}
      }
    }
    else if (action == 'b') {
      if (Player.rank[CIRCLE] == 0)
	print2(LS(IDS_MSG_21209));
      else if (Player.alignment > -1) {
	print1(LS(IDS_MSG_21241));
	print2(LS(IDS_MSG_21242));
	Player.rank[CIRCLE] = -1;
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21243));
	if (! Player.immunity[INFECTION])
	  Player.status[DISEASED]+=100;
	print2(LS(IDS_MSG_21244));
	morewait();
	clearmsg();
	acquire(-1);
	clearmsg();
	enchant(-1);
	bless(-1);
	print3(LS(IDS_MSG_21245));
	p_damage(25,UNSTOPPABLE,"a sorceror's curse");
      }
      else if (Player.rank[CIRCLE]==PRIME) 
	print2(LS(IDS_MSG_21246));
      else if (Player.rank[CIRCLE]==HIGHSORCEROR) {
	if (Player.level <= Primelevel)
	  print2(LS(IDS_MSG_21165));
	else 
	  print2(LS(IDS_MSG_21247));
      }
      else if (Player.rank[CIRCLE]==SORCEROR) {
	if (Player.guildxp[CIRCLE] < 4000)
	  print2(LS(IDS_MSG_21165));
	else  {
	  print1(LS(IDS_MSG_21248));
	  print2(LS(IDS_MSG_21249));
	  morewait();
	  clearmsg();
	  print1(LS(IDS_MSG_21250));
	  print2(LS(IDS_MSG_21251));
	  Spells[S_DISRUPT].known = TRUE;
	  Player.rank[CIRCLE] = HIGHSORCEROR;
	  Player.maxpow += 5;
	  Player.pow += 5;
	}
      }
      else if (Player.rank[CIRCLE]==ENCHANTER) {
	if (Player.guildxp[CIRCLE] < 1500)
	  print2(LS(IDS_MSG_21165));
	else  {
	  print1(LS(IDS_MSG_21252));
	  print2(LS(IDS_MSG_21253));
	  Spells[S_LBALL].known = TRUE;
	  Player.rank[CIRCLE] = SORCEROR;
	  Player.maxpow += 2; 
	  Player.pow+=2;
	}
      }
      else if (Player.rank[CIRCLE]==INITIATE) {
	if (Player.guildxp[CIRCLE] < 400)
	  print2(LS(IDS_MSG_21165));
	else  {
	  print1(LS(IDS_MSG_21254));
	  print2(LS(IDS_MSG_21255));
	  Spells[S_FIREBOLT].known = TRUE;
	  Player.rank[CIRCLE] = ENCHANTER;
	  Player.maxpow+=2;
	  Player.pow+=2;
	}
      }
    }
    else if (action == 'c') {
      done = TRUE;
      fee = Player.level*100;
      if (Player.rank[CIRCLE]) fee = fee / 2;
      clearmsg();
      print1(LS(IDS_MSG_21256));
      mnumprint(fee);
      nprint1(LS(IDS_MSG_21181));
      if (ynq1()=='y') {
	if (Player.cash < fee) 
	  print2(LS(IDS_MSG_21257));
	else {
	  Player.cash -= fee;
	  total = calcmana();
	  while (Player.mana < total) {
	    Player.mana++;
	    dataprint();
	  }
	  print2(LS(IDS_MSG_21258));
	}
      }
      else print2(LS(IDS_MSG_21259));
    }
  }
  xredraw();
}




void l_order()
{
  pob newitem;
  print1(LS(IDS_MSG_21260));
  morewait();
  if ((Player.rank[ORDER]==PALADIN) &&
      (Player.level > Justiciarlevel) &&
      gamestatusp(GAVE_STARGEM) &&
      Player.alignment > 300) {
    print1(LS(IDS_MSG_21261));
    morewait();
    print1(LS(IDS_MSG_21262));
    print2(LS(IDS_MSG_21263));
    strcpy(Justiciar,Player.name);
    Justiciarlevel = Player.level;
    morewait();
    Justiciarbehavior = fixnpc(4);
    save_hiscore_npc(15);
    clearmsg();
    print1(LS(IDS_MSG_21264));
    morewait();
    newitem = ((pob) checkmalloc(sizeof(objtype)));
    *newitem = Objects[SHIELDID+7]; /* shield of deflection */
    newitem->blessing = 9;
    gain_item(newitem);
    morewait();
    Player.rank[ORDER] = JUSTICIAR;
    Player.maxstr += 5;
    Player.str += 5;
    Player.maxpow += 5;
    Player.pow += 5;
  }
  if (Player.alignment < 1) {
    if (Player.rank[ORDER] > 0) {
      print1(LS(IDS_MSG_21265));
      print2(LS(IDS_MSG_21266));
      morewait();
      Player.rank[ORDER]= -1;
      send_to_jail();
    }
    else
      print1(LS(IDS_MSG_21267));
  }
  else if (Player.rank[ORDER] == -1)
    print1(LS(IDS_MSG_21268));
  else if (Player.rank[ORDER] == 0) {
    if (Player.rank[ARENA] != 0) 
      print1(LS(IDS_MSG_21269));
    else if (Player.rank[LEGION] != 0) 
      print1(LS(IDS_MSG_21270));
    else {
      print1(LS(IDS_MSG_21271));
      if (ynq1()=='y') {
	print1(LS(IDS_MSG_21272));
	nprint1(Justiciar);
	nprint1(LS(IDS_MSG_21273));
	print2(LS(IDS_MSG_21274));
	morewait();
	print1(LS(IDS_MSG_21275));
	print2(LS(IDS_MSG_21276));
	morewait();
	Player.rank[ORDER] = GALLANT;
	Player.guildxp[ORDER] = 1;
	setgamestatus(MOUNTED);
	newitem = ((pob) checkmalloc(sizeof(objtype)));
	*newitem = Objects[WEAPONID+19]; /* spear */
	newitem->blessing = 9;
	newitem->plus = 1;
	newitem->known = 2;
	gain_item(newitem);
      }
    }
  } else {
      print1(LS(IDS_MSG_21277));
      if (!gamestatusp(MOUNTED)) {
	print2(LS(IDS_MSG_21278));
	setgamestatus(MOUNTED);
      }
      morewait();
      clearmsg();
      if ((Player.hp < Player.maxhp) || (Player.status[DISEASED]) ||
	(Player.status[POISONED]))
	print1(LS(IDS_MSG_21279));
      cleanse(0);
      Player.hp = Player.maxhp;
      Player.food = 40;
      print2(LS(IDS_MSG_21280));
      morewait();
      clearmsg();
      if (Player.rank[ORDER]==PALADIN) {
	if (Player.level <= Justiciarlevel)
	  print2(LS(IDS_MSG_21165));
	else if (Player.alignment < 300)
	  print2(LS(IDS_MSG_21281));
	else print2(LS(IDS_MSG_21282));
      }
      else if (Player.rank[ORDER]==CHEVALIER) {
	if (Player.guildxp[ORDER] < 4000)
	  print2(LS(IDS_MSG_21165));
	else if (Player.alignment < 200)
	  print2(LS(IDS_MSG_21281));
	else {
	  print1(LS(IDS_MSG_21283));
	  print2(LS(IDS_MSG_21284));
	  morewait();
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[ARMORID+11]; /* mithril plate armor */
	  newitem->blessing = 9;
	  newitem->known = 2;
	  gain_item(newitem);
	  morewait();
	  clearmsg();
	  print1(LS(IDS_MSG_21285));
	  print2(LS(IDS_MSG_21286));
	  morewait();
	  print1(LS(IDS_MSG_21287));
	  print2(LS(IDS_MSG_21288));
	  morewait();
	  print1(LS(IDS_MSG_21289));
	  print2(LS(IDS_MSG_21290));
	  morewait();
	  Spells[S_HERO].known = TRUE;
	  Player.rank[ORDER] = PALADIN;
	}
      }
      else if (Player.rank[ORDER]==GUARDIAN) {
	if (Player.guildxp[ORDER] < 1500)
	  print2(LS(IDS_MSG_21165));
	else if (Player.alignment < 125)
	  print2(LS(IDS_MSG_21291));
	else {
	  Player.rank[ORDER] = CHEVALIER;
	  print1(LS(IDS_MSG_21292));
	  print2(LS(IDS_MSG_21293));
	  morewait();
	  clearmsg();
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[WEAPONID+25]; /* mace of disruption */
	  newitem->known = 2;
	  gain_item(newitem);
	}
      }
      else if (Player.rank[ORDER]==GALLANT) {
	if (Player.guildxp[ORDER] < 400)
	  print2(LS(IDS_MSG_21165));
	else if (Player.alignment < 50)
	  print2(LS(IDS_MSG_21294));
	else  {
	  print1(LS(IDS_MSG_21295));
	  print2(LS(IDS_MSG_21296));
	  morewait();
	  print1(LS(IDS_MSG_21297));
	  print2(LS(IDS_MSG_21298));
	  morewait();
	  clearmsg();
	  Player.rank[ORDER] = GUARDIAN;
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[ARTIFACTID+7]; /* holy hand grenade. */
	  newitem->known = 2;
	  gain_item(newitem);
	}
      }
  }
}
