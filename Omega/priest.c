/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* priest.c */
/* functions for clerics. */

#include "glob.h"

/* prayer occurs at altars, hence name of function */
void l_altar()
{
  int i,deity;
  char response;

  if (Current_Environment == E_COUNTRYSIDE) deity = DRUID;
  else deity = Level->site[Player.x][Player.y].aux;

  switch(deity) {
    default:
      print1(LS(IDS_MSG_22112));
      break;
    case ODIN:
      print1(LS(IDS_MSG_22113));
      break;
    case SET:
      print1(LS(IDS_MSG_22114));
      break;
    case HECATE:
      print1(LS(IDS_MSG_22115));
      break;
    case ATHENA:
      print1(LS(IDS_MSG_22116));
      break;
    case DESTINY:
      print1(LS(IDS_MSG_22117));
      break;
    case DRUID:
      print1(LS(IDS_MSG_22118));
      break;
  }
  print2(LS(IDS_MSG_22119));
  if (ynq2() == 'y') {
    if (Player.rank[PRIESTHOOD] == 0)
      increase_priest_rank(deity);
    else if (! check_sacrilege(deity)) {
      if (Blessing) print1(LS(IDS_MSG_22120));
      print2(LS(IDS_MSG_22121));
      do response = (char) mcigetc();
      while ((response != 'b') && 
	     (response != 's') && 
	     (response != 'p') && 
	     (response != ESCAPE));
      if (response == 'b') {
	print1(LS(IDS_MSG_22122));
	print2(LS(IDS_MSG_22123));
	morewait();
	if (Blessing) {
	  print1(LS(IDS_MSG_22124));
	  print2(LS(IDS_MSG_22125));
	  morewait();
	  gain_experience(Player.rank[PRIESTHOOD]*Player.rank[PRIESTHOOD]*50);
	  cleanse(1);
	  heal(10);
	  bless(1);
	  Blessing = FALSE;
	  increase_priest_rank(deity);
	}
	else {
	  print1(LS(IDS_MSG_22126));
	  print2(LS(IDS_MSG_21629));
	  Player.xp -= (Player.xp/4);
	}
	calc_melee();
      }
      else if (response == 's') {
	print1(LS(IDS_MSG_22127));
	i = getitem(NULL_ITEM);
	if (i==ABORT) i = 0;
	if (Player.possessions[i] == NULL) {
	  print1(LS(IDS_MSG_22128));
	  print2(LS(IDS_MSG_22129));
	  dispel(-1);
	  p_damage(Player.hp-1,UNSTOPPABLE,"a god's pique");
	}
	else if (true_item_value(Player.possessions[i]) >
		 (long) (Player.rank[PRIESTHOOD] *
		  Player.rank[PRIESTHOOD] *
		  Player.rank[PRIESTHOOD] * 50)) {
	  print1(LS(IDS_MSG_22130));
	  dispose_lost_objects(1,Player.possessions[i]);
	  print2(LS(IDS_MSG_22131));
	  morewait();
	  Blessing = TRUE;
	}
	else {
	  print1(LS(IDS_MSG_22132));
	  print2(LS(IDS_MSG_22133));
	  morewait();
	  setgamestatus(SUPPRESS_PRINTING);
	  if (Player.possessions[i]->used) {
	    Player.possessions[i]->used = FALSE;
	    item_use(Player.possessions[i]);
	    Player.possessions[i]->blessing =
	      -1 - abs(Player.possessions[i]->blessing);
	    Player.possessions[i]->used = TRUE;
	    item_use(Player.possessions[i]);
	  }
	  else Player.possessions[i]->blessing =
	    -1 - abs(Player.possessions[i]->blessing);
	  resetgamestatus(SUPPRESS_PRINTING);
	}
      }
      else if (response == 'p') {
	if (deity != Player.patron) 
	  print1(LS(IDS_MSG_21522));
	else if (!increase_priest_rank(deity))
	  answer_prayer();
      }
    }
  }
}




int check_sacrilege(deity)
int deity;
{
  int i,sacrilege=FALSE;
  if ((Player.patron != deity) && (Player.patron > 0)) {
    sacrilege=TRUE;
    Player.pow--;
    Player.maxpow--;
    switch(Player.patron) {
    case ODIN:
      print1(LS(IDS_MSG_22134));
      morewait();
      if (deity == ATHENA) {
	print2(LS(IDS_MSG_22135));
	sacrilege = FALSE;
      }
      else {
	print2(LS(IDS_MSG_22136));
	p_damage(Player.level*5,UNSTOPPABLE,"Odin's wrath");
	if (Player.hp > 0) {
	  morewait();
	  print2(LS(IDS_MSG_22137));
	  Player.maxcon = Player.maxcon/2;
	  Player.con = min(Player.con,Player.maxcon);
	  Player.maxstr = Player.maxstr/2;
	  Player.con = min(Player.str,Player.maxstr);
	}
      }
      morewait();
      break;
    case SET:
      print1(LS(IDS_MSG_22138));
      morewait();
      if (deity == HECATE) {
	print1(LS(IDS_MSG_22139));
	print2(LS(IDS_MSG_22140));
	sacrilege = FALSE;
      }
      else {
	print2(LS(IDS_MSG_22141));
	p_damage(Player.level*5,UNSTOPPABLE,"Set's anger");
	if (Player.hp > 0) {
	  morewait();
	  print1(LS(IDS_MSG_22142));
	  for(i=0;i<MAXITEMS;i++) 
	    if ((Player.possessions[i] != NULL) &&
		(Player.possessions[i]->blessing > -1))
	      conform_lost_object(Player.possessions[i]);
	  morewait();
	  print2(LS(IDS_MSG_22143));
	  Player.con = Player.maxcon = Player.maxcon / 4;
	}
      }
      morewait();
      break;
    case HECATE:
      print1(LS(IDS_MSG_22144));
      morewait();
      if (deity == SET) {
	print1(LS(IDS_MSG_22145));
	sacrilege = FALSE;
      }
      else {
	print1(LS(IDS_MSG_22146));
	p_damage(Player.level*5,UNSTOPPABLE,"Hecate's malice");
	if (Player.hp > 0) {
	  print2(LS(IDS_MSG_22147));
	  Player.maxpow = Player.maxpow/5;
	  Player.pow = min(Player.pow,Player.maxpow);
	  for(i=0;i<NUMSPELLS;i++) 
	    Spells[i].known = FALSE;
	}
      }
      morewait();
      break;
    case ATHENA:
      print1(LS(IDS_MSG_22148));
      morewait();
      if (deity == ODIN) {
	print2(LS(IDS_MSG_22149));
	sacrilege = FALSE;
      }
      else {
	print2(LS(IDS_MSG_22150));
	if (Player.hp > 0) {
	  morewait();
	  print1(LS(IDS_MSG_22151));
		Player.level = 0;
	  Player.xp = 0;
	  Player.maxhp = Player.hp = Player.con;
	  print2(LS(IDS_MSG_22152));
	  Player.pow = Player.maxpow = Player.maxpow/3;
	  Player.mana = min(Player.mana,calcmana());
	}
      }
      morewait();
      break;
    case DESTINY:
      print2(LS(IDS_MSG_22153));
      sacrilege = FALSE;
      morewait();
      break;
    case DRUID:
      print2(LS(IDS_MSG_22154));
      if (random_range(2) == 1)
	Player.alignment += 40;
      else Player.alignment -= 40;
      morewait();
      break;
    }
    if (sacrilege) {
      Player.patron = 0;
      Player.rank[PRIESTHOOD] = 0;
    }
  }
  return(sacrilege);
}

int increase_priest_rank(deity)
int deity;
{
  if (Player.rank[PRIESTHOOD] == 0) switch(deity) {
  default:
    print2(LS(IDS_MSG_22155));
    Player.hp = Player.maxhp;
    morewait();
    print2(LS(IDS_MSG_22156));
    Level->site[Player.x][Player.y].locchar = FLOOR;
    Level->site[Player.x][Player.y].p_locf = L_NO_OP;
    lset(Player.x, Player.y, CHANGED);
    break;
  case ODIN:
    if (Player.alignment > 0) {
      print1(LS(IDS_MSG_22157));
      print2(Priest[ODIN]);
      nprint2(LS(IDS_MSG_22158));
      nprint2(LS(IDS_MSG_22159));
      Player.patron = ODIN;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      morewait();
      learnclericalspells(ODIN,LAY);
    }
    else print1(LS(IDS_MSG_22160));
    break;
  case SET:
    if (Player.alignment < 0) {
      print1(LS(IDS_MSG_22161));
      print2(Priest[SET]);
      nprint2(LS(IDS_MSG_22162));
      nprint2(LS(IDS_MSG_22163));
      Player.patron = SET;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      morewait();
      learnclericalspells(SET,LAY);
    }
    else print1(LS(IDS_MSG_22164));
    break;
  case ATHENA:
    if (Player.alignment > 0) {
      print1(LS(IDS_MSG_22165));
      print2(Priest[ATHENA]);
      nprint2(LS(IDS_MSG_22158));
      nprint2(LS(IDS_MSG_22166));
      Player.patron = ATHENA;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      morewait();
      learnclericalspells(ATHENA,LAY);
    }
    else print1(LS(IDS_MSG_22167));
    break;
  case HECATE:
    if (Player.alignment < 0) {
      print1(LS(IDS_MSG_22168));
      print2(Priest[HECATE]);
      nprint2(LS(IDS_MSG_22158));
      nprint2(LS(IDS_MSG_22169));
      Player.patron = HECATE;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      morewait();
      learnclericalspells(HECATE,LAY);
    }
    else print1(LS(IDS_MSG_22170));
    break;
  case DRUID:
    if (abs(Player.alignment) < 10) {
      print1(Priest[DRUID]);
      nprint1(LS(IDS_MSG_22158));
      print2(LS(IDS_MSG_22171));
      Player.patron = DRUID;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      morewait();
      learnclericalspells(DRUID,LAY);
    }
    else {
      print1(LS(IDS_MSG_22172));
      morewait();
      print2(LS(IDS_MSG_22173));
    }
    break;
  case DESTINY:
    print1(LS(IDS_MSG_22174));
    print2(LS(IDS_MSG_22175));
    Player.patron = DESTINY;
    Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
    break;
  }
  else if (deity == Player.patron) {
    if ((((deity == ODIN) || (deity == ATHENA)) && 
	 (Player.alignment < 1)) ||
	(((deity == SET) || (deity == HECATE)) && 
	 (Player.alignment > 1)) ||
	((deity == DRUID) && (abs(Player.alignment) > 10))) {
      print1(LS(IDS_MSG_22176));
      print2(LS(IDS_MSG_22177));
      Player.xp -= Player.level*Player.level;
      Player.xp = max(0,Player.xp);
    }
    else if (Player.rank[PRIESTHOOD]== HIGHPRIEST) return 0;
    else if (Player.rank[PRIESTHOOD]== SPRIEST) {
      if (Player.level > Priestlevel[deity])
	hp_req_test();
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==PRIEST) {
      if (Player.guildxp[PRIESTHOOD] >= 4000) {
	print1(LS(IDS_MSG_22178));
	print2(LS(IDS_MSG_22179));
	hp_req_print();
	Player.rank[PRIESTHOOD] = SPRIEST;
      }
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==ACOLYTE) {
      if (Player.guildxp[PRIESTHOOD] >= 1500) {
	print1(LS(IDS_MSG_22180));
	print2(LS(IDS_MSG_22181));
	Player.rank[PRIESTHOOD] = PRIEST;
	morewait();
	learnclericalspells(deity,PRIEST);
      }
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==LAY) {
      if (Player.guildxp[PRIESTHOOD] >= 400) {
	print1(LS(IDS_MSG_22182));
	print2(LS(IDS_MSG_22183));
	Player.rank[PRIESTHOOD] = ACOLYTE;
	morewait();
	learnclericalspells(deity,ACOLYTE);
      }
      else return 0;
    }
  }
  return 1;
}


void answer_prayer()
{
  clearmsg();
  switch(random_range(12)) {
  case 0: print1(LS(IDS_MSG_22184)); break;
  case 1: print1(LS(IDS_MSG_22185)); break;
  case 2: print1(LS(IDS_MSG_22186)); break;
  default: print1(LS(IDS_MSG_22187)); break;
  }
}


void hp_req_test()
{
  pob o;
  switch  (Player.patron) {
  case ODIN:
    if (find_item(&o,ARTIFACTID+15,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case SET:
    if (find_item(&o,ARTIFACTID+14,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case ATHENA:
    if (find_item(&o,ARTIFACTID+17,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case HECATE:
    if (find_item(&o,ARTIFACTID+16,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case DRUID:
    if (find_item(&o,ARTIFACTID+14,-1))
      make_hp(o);
    else if (find_item(&o,ARTIFACTID+15,-1))
      make_hp(o);
    else if (find_item(&o,ARTIFACTID+16,-1))
      make_hp(o);
    else if (find_item(&o,ARTIFACTID+17,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case DESTINY:
    if (find_item(&o,ARTIFACTID+19,-1))
      make_hp(o);
    else hp_req_print();
    break;
  }
}


void hp_req_print()
{
  morewait();
  print1(LS(IDS_MSG_22188));
  switch(Player.patron) {
  case ODIN:
    nprint1(Priest[SET]);
    print2(LS(IDS_MSG_22189));
    break;
  case SET:
    nprint1(Priest[ODIN]);
    print2(LS(IDS_MSG_22190));
    break;
  case ATHENA:
    nprint1(Priest[HECATE]);
    print2(LS(IDS_MSG_22191));
    break;
  case HECATE:
    nprint1(Priest[ATHENA]);
    print2(LS(IDS_MSG_22192));
    break;
  case DRUID:
    print2(LS(IDS_MSG_22193));
    break;
  case DESTINY:
    nprint1(Priest[DESTINY]);
    print2(LS(IDS_MSG_22194));
    break;
  }
}

void make_hp(o)
pob o;
{
  print1(LS(IDS_MSG_22195));
  print2(LS(IDS_MSG_22196));
  switch(Player.patron) {
  case ODIN:
    *o = Objects[ARTIFACTID+14];
    break;
  case SET:
    *o = Objects[ARTIFACTID+15];
    break;
  case ATHENA:
    *o = Objects[ARTIFACTID+16];
    break;
  case HECATE:
    *o = Objects[ARTIFACTID+17];
    break;
  case DRUID:
    *o = Objects[ARTIFACTID+18];
    break;
  case DESTINY:
    *o = Objects[ARTIFACTID+19];
    break;
  }
  o->known = 2;
  o->charge = 17; /* random hack to convey bit that symbol is functional */
  morewait();
  if (Player.patron == DRUID)
    print1(LS(IDS_MSG_22197));
  else print1(LS(IDS_MSG_22198));
  print2(LS(IDS_MSG_22199));
  strcpy(Priest[Player.patron],Player.name);
  Priestlevel[Player.patron] = Player.level;
  Player.rank[PRIESTHOOD] = HIGHPRIEST;
  morewait();
  Priestbehavior[Player.patron] = fixnpc(4);
  save_hiscore_npc(Player.patron);
  learnclericalspells(Player.patron,HIGHPRIEST);
}
