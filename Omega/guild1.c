/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* guild1.c */
/* L_ functions  */ 

/* These functions implement the various guilds. */
/* They are all l_ functions since they are basically activated*/
/* at some site or other. */

#include "glob.h"


void l_merc_guild()
{
  pob newitem;

  print1(LS(IDS_MSG_21041));
  if (nighttime())
    print2(LS(IDS_MSG_21042)); 
  else {
    print2(LS(IDS_MSG_21043));
    if (Player.rank[LEGION] == COMMANDANT) {
      nprint2(LS(IDS_MSG_21044));
      morewait();
      clearmsg();
    }
    if (Player.rank[LEGION] > 0) {
      nprint2(LS(IDS_MSG_21045));
      morewait();
    }
    switch(Player.rank[LEGION]) {
    case 0:
      nprint2(LS(IDS_MSG_21046));
      morewait();
      print2(LS(IDS_MSG_21047));
      if (ynq2()=='y') {
	clearmsg();
	if (Player.rank[ARENA]>0) {
	  print1(LS(IDS_MSG_21048));
	  print2(LS(IDS_MSG_21049));
	}
	else if (Player.rank[ORDER] > 0) {
	  print1(LS(IDS_MSG_21050));
	  print2(LS(IDS_MSG_21051));
	}
	else if (Player.con < 12) {
	  print1(LS(IDS_MSG_21052));
	  print2(LS(IDS_MSG_21053));
	}
	else if (Player.str < 10) {
	  print1(LS(IDS_MSG_21054));
	  print2(LS(IDS_MSG_21055));
	}
	else {
	  print1(LS(IDS_MSG_21056));
	  morewait();
	  nprint1(LS(IDS_MSG_21057));
	  print2(LS(IDS_MSG_21058));
	  nprint2(Commandant);
	  nprint2(LS(IDS_MSG_21059));
	  morewait();
	  print2(LS(IDS_MSG_21060));
	  morewait();
	  print1(LS(IDS_MSG_21061));
	  print2(LS(IDS_MSG_21062));
	  morewait();
	  clearmsg();
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[WEAPONID+1]; /* shortsword */
	  gain_item(newitem);
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[ARMORID+1]; /* leather */
	  gain_item(newitem);
	  Player.cash += 500;
	  Player.rank[LEGION] = LEGIONAIRE;
	  Player.guildxp[LEGION] = 1;
	  Player.str++; Player.con++;
	  Player.maxstr++; Player.maxcon++;
	}
      }
      break;
    case COMMANDANT:
      print1(LS(IDS_MSG_21063));
      break;
    case COLONEL:
      if ((Player.level > Commandantlevel) &&
	  find_and_remove_item(CORPSEID+0,ML10+4)) {  
	print1(LS(IDS_MSG_21064));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21065));
	print2(LS(IDS_MSG_21066));
	morewait();
	clearmsg(); 
	print1(LS(IDS_MSG_21058));
	nprint1(Commandant);
	nprint1(LS(IDS_MSG_21067));
	print2(LS(IDS_MSG_21068));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21069));
	print2(LS(IDS_MSG_21070));
	strcpy(Commandant,Player.name);
	Commandantlevel = Player.level;
	morewait();
	Commandantbehavior = fixnpc(4);
	save_hiscore_npc(8);
	clearmsg();
	print1(LS(IDS_MSG_21071));
	Spells[S_REGENERATE].known = TRUE;
	Player.rank[LEGION]=COMMANDANT;
	Player.maxstr += 2;
	Player.str += 2;
	Player.maxcon += 2;
	Player.con += 2;
	print2(LS(IDS_MSG_21072));
	Player.cash += 20000;
      }
      else if (Player.level <= Commandantlevel) {
	clearmsg();
	print1(LS(IDS_MSG_21073));
	print2(LS(IDS_MSG_21074));
      }
      else {
	clearmsg();
	print1(LS(IDS_MSG_21075)); 
	print2(LS(IDS_MSG_21076));
      }
      break;
    case FORCE_LEADER:
      clearmsg();
      print1(LS(IDS_MSG_21073));
      if (Player.guildxp[LEGION] < 4000)
	print2(LS(IDS_MSG_21074));
      else  {
	print2(LS(IDS_MSG_21077));
	morewait();
	print1(LS(IDS_MSG_21078));
	print2(LS(IDS_MSG_21079));
	morewait();
	print1(LS(IDS_MSG_21080));
	print2(LS(IDS_MSG_21081));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21082));
	Spells[S_HERO].known = TRUE;
	Player.rank[LEGION]=COLONEL;
	Player.maxstr++;
	Player.str++;
	Player.maxcon++;
	Player.con++;
	print2(LS(IDS_MSG_21083));
	Player.cash += 10000;
      }
      break;
    case CENTURION:
      clearmsg();
      print1(LS(IDS_MSG_21073));
      if (Player.guildxp[LEGION] < 1500)
	print2(LS(IDS_MSG_21074));
      else {
	print2(LS(IDS_MSG_21084));
	Player.rank[LEGION]=FORCE_LEADER;
	Player.maxstr++;
	Player.str++;
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21085));
	Player.cash += 5000;
      }
      break;
      case LEGIONAIRE:
      clearmsg();
      print1(LS(IDS_MSG_21073));
      if (Player.guildxp[LEGION] < 400)
	print2(LS(IDS_MSG_21074));
      else {
	print2(LS(IDS_MSG_21086));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21087));
	Player.rank[LEGION] = CENTURION;
	Player.maxcon++;
	Player.con++;
	Player.cash += 2000;
      }
      break;
    }
  }
}

void l_castle()
{
  pob o;
  int x, y;

  if (Player.level < 3) {
    print1(LS(IDS_MSG_21088));
    print2(LS(IDS_MSG_21089));
  }
  else {
    print1(LS(IDS_MSG_21090));
    if (Player.rank[NOBILITY]<DUKE) {
      print2(LS(IDS_MSG_21091));
      nprint2(Duke);
      nprint2(LS(IDS_MSG_21092));
      morewait();
      clearmsg();
    }
    if (Player.rank[NOBILITY]==0) {
      print1(LS(IDS_MSG_21093));
      if (ynq1() == 'y') {
	print2(LS(IDS_MSG_21094));
	Player.rank[NOBILITY]=COMMONER;
      }
      else {
	print1(LS(IDS_MSG_21095));
	morewait();
	p_damage(25,UNSTOPPABLE,"castle guards for lese majeste");
	send_to_jail();
      }
    }
    else if (Player.rank[NOBILITY]==COMMONER) {
      if (find_and_remove_item(CORPSEID,ML3+5)) {
	print1(LS(IDS_MSG_21096));
	Player.rank[NOBILITY]=ESQUIRE;
	gain_experience(100);
	print2(LS(IDS_MSG_21097));
	morewait();
	print1(LS(IDS_MSG_21098));
	print2(LS(IDS_MSG_21099));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21100));
      }
      else print2(LS(IDS_MSG_21101));
    }
    else if (Player.rank[NOBILITY]==ESQUIRE) {
      if (find_and_remove_item(WEAPONID+34,-1)) {
	print1(LS(IDS_MSG_21102));
	Player.rank[NOBILITY]=KNIGHT;
	gain_experience(1000);
	print2(LS(IDS_MSG_21103));
	morewait();
	print1(LS(IDS_MSG_21104));
	print2(LS(IDS_MSG_21105));
      }
      else print2(LS(IDS_MSG_21106));
    }
    else if (Player.rank[NOBILITY]==KNIGHT) {
      if (find_and_remove_item(ARMORID+12,-1)) {
	print1(LS(IDS_MSG_21107));
	print2(LS(IDS_MSG_21108));
	Player.rank[NOBILITY]=LORD;
	gain_experience(10000);
	morewait();
	print1(LS(IDS_MSG_21109));
	print2(LS(IDS_MSG_21110));
	morewait();
	print1(LS(IDS_MSG_21111));
	print2(LS(IDS_MSG_21112));
      }
      else print2(LS(IDS_MSG_21113));
    }
    else if (Player.rank[NOBILITY]==LORD) {
      if (find_item(&o,ARTIFACTID+0,-1)) {
	print1(LS(IDS_MSG_21114));
	print2(LS(IDS_MSG_21115));
	morewait();
	print1(LS(IDS_MSG_21116));
	print2(LS(IDS_MSG_21117));
	Player.rank[NOBILITY]=DUKE;
	gain_experience(10000);
	strcpy(Duke,Player.name);
	morewait();
	Dukebehavior = fixnpc(4);
	save_hiscore_npc(12);
	for (y = 52; y < 63; y++)
	    for (x = 2; x < 52; x++) {
		if (Level->site[x][y].p_locf == L_TRAP_SIREN) {
		    Level->site[x][y].p_locf = L_NO_OP;
		    lset(x, y, CHANGED);
		}
		if (x >= 12 && loc_statusp(x, y, SECRET)) {
		    lreset(x, y, SECRET);
		    lset(x, y, CHANGED);
		}
		if (x >= 20 && x <= 23 && y == 56) {
		    Level->site[x][y].locchar = FLOOR;
		    lset(x, y, CHANGED);
		}
	    }

      }
      else print2(LS(IDS_MSG_21118));
    }
  }
}


void l_arena()
{
  char response;
  pob newitem;
  int i,prize,monsterlevel;
  char *name, *corpse, *melee = NULL;

  print1(LS(IDS_MSG_21119));
  if (Player.rank[ARENA] == 0) {
    print2(LS(IDS_MSG_21120));
    do response = (char) mcigetc();
    while ((response != 'e') && (response != 'r') && (response != ESCAPE));
  }
  else {
    print2(LS(IDS_MSG_21121));
    response = ynq2();
    if (response == 'y') response = 'e';
    else response = ESCAPE;
  }
  if (response == 'r') {
    if (Player.rank[ARENA]>0)
      print2(LS(IDS_MSG_21122));
    else if (Player.rank[ORDER]>0)
      print2(LS(IDS_MSG_21123));
    else if (Player.rank[LEGION]>0)
      print2(LS(IDS_MSG_21124));
    else if (Player.str < 13)
      print2(LS(IDS_MSG_21125));
    else if (Player.agi < 12)
      print2(LS(IDS_MSG_21126));
    else {
      print1(LS(IDS_MSG_21127));
      print2(LS(IDS_MSG_21128));
      morewait();
      clearmsg();
      newitem = ((pob) checkmalloc(sizeof(objtype)));
      *newitem = Objects[WEAPONID+17]; /* club */
      gain_item(newitem);
      newitem = ((pob) checkmalloc(sizeof(objtype)));
      *newitem = Objects[SHIELDID+2]; /* shield */
      gain_item(newitem);
      Player.rank[ARENA] = TRAINEE;
      Arena_Opponent = 3;
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_21129));
      Gymcredit+=5000;
    }
  }
  else if (response == 'e') {
    print1(LS(IDS_MSG_21130));
    morewait();
    Arena_Monster = ((pmt) checkmalloc(sizeof(montype)));
    Arena_Victory = FALSE;
    switch(Arena_Opponent) {
    case 0: 
      *Arena_Monster = Monsters[ML1+9];
      break;
    case 1:
      *Arena_Monster = Monsters[ML0+0];
      break;
    case 2:
      *Arena_Monster = Monsters[ML1+21];
      break;
    case 3:
      *Arena_Monster = Monsters[ML1+6];
      break;
    case 4:
      *Arena_Monster = Monsters[ML1+0];
      break;
    case 5:
      *Arena_Monster = Monsters[ML2+4];
      break;      
    case 6:
      *Arena_Monster = Monsters[ML2+0];
      break;
    case 7:
      *Arena_Monster = Monsters[ML3+0];
      break;
    case 8:
      *Arena_Monster = Monsters[ML2+11];
      break;
    case 9:
      *Arena_Monster = Monsters[ML4+0];
      break;
    case 10:
      *Arena_Monster = Monsters[ML5+5];
      break;
    case 11:
      *Arena_Monster = Monsters[ML6+4];
      break;
    case 12:
      *Arena_Monster = Monsters[ML6+5];
      break;
    case 13: 
      *Arena_Monster = Monsters[ML8+2];
      break;
    case 14:
      *Arena_Monster = Monsters[ML8+4];
      break;
    case 15:
      *Arena_Monster = Monsters[ML9+0];
      break;
    default:
      if ((Player.rank[ARENA] < 5) && (Player.rank[ARENA] > 0)) {
	strcpy(Str1,Champion);
	strcat(Str1,", the arena champion");
	*Arena_Monster = Monsters[ML0+8];
	name = Arena_Monster->monstring = salloc(Str1);
	strcpy(Str2,"The corpse of ");
	strcat(Str2,Str1);
	corpse = Arena_Monster->corpsestr = salloc(Str2);
	Arena_Monster->level = 20;
	Arena_Monster->hp = Championlevel*Championlevel*5;
	Arena_Monster->hit = Championlevel*4;
	Arena_Monster->ac = Championlevel*3;
	Arena_Monster->dmg = 100+Championlevel*2;
	Arena_Monster->xpv = Championlevel*Championlevel*5;
	Arena_Monster->speed = 3;
	melee = Arena_Monster->meleestr = (char *) checkmalloc(30*sizeof(char));
	strcpy(Arena_Monster->meleestr,"");
	for(i=0;i<Championlevel/5;i++)
	  strcat(Arena_Monster->meleestr,"L?R?");
	m_status_set(Arena_Monster, MOBILE);
	m_status_set(Arena_Monster, HOSTILE);
      }
      else {
	do 
	  *Arena_Monster = Monsters[random_range(ML9-ML0)+ML0];
	while ((Arena_Monster->uniqueness != COMMON) || 
	       (Arena_Monster->dmg == 0));
      }
      break;
    }
    monsterlevel = Arena_Monster->level;
    if (Arena_Monster->level != 20) {
      strcpy(Str1,nameprint());
      strcat(Str1," the ");
      strcat(Str1,Arena_Monster->monstring);
      name = Arena_Monster->monstring = salloc(Str1);
      strcpy(Str2,"The corpse of ");
      strcat(Str2,Str1);
      corpse = Arena_Monster->corpsestr = salloc(Str2);
    }
    Arena_Monster->uniqueness = UNIQUE_MADE;
    print1(LS(IDS_MSG_21131));
    print2(Arena_Monster->monstring);
    Arena_Monster->attacked = TRUE;
    m_status_set(Arena_Monster,HOSTILE);
    morewait();
    clearmsg();
    change_environment(E_ARENA);
    print1(LS(IDS_MSG_21132));

    time_clock(TRUE);
    while (Current_Environment == E_ARENA) 
      time_clock(FALSE);

    free(name);		/* hey - why waste space? */
    free(corpse);
    if (melee)
      free(melee);
    if (! Arena_Victory) {
      print1(LS(IDS_MSG_21133));
      if (Player.rank[ARENA] > 0) {
	print2(LS(IDS_MSG_21134));
	morewait();
	clearmsg();
	if (Gymcredit > 0) print1(LS(IDS_MSG_21135));
	Gymcredit = 0;
	Player.rank[ARENA] = -1;
      }
    }      
    else {
      Arena_Opponent++;
      if (monsterlevel == 20) {
	print1(LS(IDS_MSG_21136));
	if (Player.rank[ARENA]) {
	  print2(LS(IDS_MSG_21137));
	  Championlevel = Player.level;
	  strcpy(Champion,Player.name);
	  Player.rank[ARENA] = 5;
	  morewait();
	  Championbehavior = fixnpc(4);
	  save_hiscore_npc(11);
	  print1(LS(IDS_MSG_21138));
	  morewait();
	  newitem = ((pob) checkmalloc(sizeof(objtype)));
	  *newitem = Objects[WEAPONID+35];
	  gain_item(newitem);
	  
	}
	else {
	  print1(LS(IDS_MSG_21139));
	  nprint1(LS(IDS_MSG_21140));
	  morewait();
	}
      }
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_21141));
      nprint1(LS(IDS_MSG_21142));
      prize = max(25,monsterlevel * 50);
      if (Player.rank[ARENA] > 0) prize *= 2;
      mnumprint(prize);
      nprint1(LS(IDS_MSG_21143));
      Player.cash+=prize;
      if ((Player.rank[ARENA]<4) &&
	  (Arena_Opponent>5) &&
	  (Arena_Opponent % 3 == 0)) {
	if (Player.rank[ARENA]>0) {
	  Player.rank[ARENA]++;
	  morewait();
	  print1(LS(IDS_MSG_21144));
	  print2(LS(IDS_MSG_21145));
	  Gymcredit+=Arena_Opponent*1000;
	}
      }
    }
    xredraw();
  }
  else clearmsg();
}
