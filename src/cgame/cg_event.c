// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_event.c -- handle entity events at snapshot or playerstate transitions

/*
 *  Portions Copyright (C) 2000-2001 Tim Angus
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the OSML - Open Source Modification License v1.0 as
 *  described in the file COPYING which is distributed with this source
 *  code.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "cg_local.h"

// for the voice chats
#include "../../ui/menudef.h"

//==========================================================================

/*
===================
CG_PlaceString

Also called by scoreboard drawing
===================
*/
const char  *CG_PlaceString( int rank ) {
  static char str[64];
  char  *s, *t;

  if ( rank & RANK_TIED_FLAG ) {
    rank &= ~RANK_TIED_FLAG;
    t = "Tied for ";
  } else {
    t = "";
  }

  if ( rank == 1 ) {
    s = S_COLOR_BLUE "1st" S_COLOR_WHITE;   // draw in blue
  } else if ( rank == 2 ) {
    s = S_COLOR_RED "2nd" S_COLOR_WHITE;    // draw in red
  } else if ( rank == 3 ) {
    s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE;   // draw in yellow
  } else if ( rank == 11 ) {
    s = "11th";
  } else if ( rank == 12 ) {
    s = "12th";
  } else if ( rank == 13 ) {
    s = "13th";
  } else if ( rank % 10 == 1 ) {
    s = va("%ist", rank);
  } else if ( rank % 10 == 2 ) {
    s = va("%ind", rank);
  } else if ( rank % 10 == 3 ) {
    s = va("%ird", rank);
  } else {
    s = va("%ith", rank);
  }

  Com_sprintf( str, sizeof( str ), "%s%s", t, s );
  return str;
}

/*
=============
CG_Obituary
=============
*/
static void CG_Obituary( entityState_t *ent ) {
  int     mod;
  int     target, attacker;
  char    *message;
  char    *message2;
  const char  *targetInfo;
  const char  *attackerInfo;
  char    targetName[32];
  char    attackerName[32];
  gender_t  gender;
  clientInfo_t  *ci;

  target = ent->otherEntityNum;
  attacker = ent->otherEntityNum2;
  mod = ent->eventParm;

  if ( target < 0 || target >= MAX_CLIENTS ) {
    CG_Error( "CG_Obituary: target out of range" );
  }
  ci = &cgs.clientinfo[target];

  if ( attacker < 0 || attacker >= MAX_CLIENTS ) {
    attacker = ENTITYNUM_WORLD;
    attackerInfo = NULL;
  } else {
    attackerInfo = CG_ConfigString( CS_PLAYERS + attacker );
  }

  targetInfo = CG_ConfigString( CS_PLAYERS + target );
  if ( !targetInfo ) {
    return;
  }
  Q_strncpyz( targetName, Info_ValueForKey( targetInfo, "n" ), sizeof(targetName) - 2);
  strcat( targetName, S_COLOR_WHITE );

  message2 = "";

  // check for single client messages

  switch( mod ) {
  case MOD_SUICIDE:
    message = "suicides";
    break;
  case MOD_FALLING:
    message = "cratered";
    break;
  case MOD_CRUSH:
    message = "was squished";
    break;
  case MOD_WATER:
    message = "sank like a rock";
    break;
  case MOD_SLIME:
    message = "melted";
    break;
  case MOD_LAVA:
    message = "does a back flip into the lava";
    break;
  case MOD_TARGET_LASER:
    message = "saw the light";
    break;
  case MOD_TRIGGER_HURT:
    message = "was in the wrong place";
    break;
  case MOD_HSPAWN:
    message = "should have run further";
    break;
  case MOD_ASPAWN:
    message = "was melted by the acid blood";
    break;
  default:
    message = NULL;
    break;
  }

  if (attacker == target) {
    gender = ci->gender;
    switch (mod) {
    case MOD_GRENADE_SPLASH:
      if ( gender == GENDER_FEMALE )
        message = "tripped on her own grenade";
      else if ( gender == GENDER_NEUTER )
        message = "tripped on its own grenade";
      else
        message = "tripped on his own grenade";
      break;
    case MOD_ROCKET_SPLASH:
      if ( gender == GENDER_FEMALE )
        message = "blew herself up";
      else if ( gender == GENDER_NEUTER )
        message = "blew itself up";
      else
        message = "blew himself up";
      break;
    case MOD_FLAMER_SPLASH:
      if ( gender == GENDER_FEMALE )
        message = "toasted herself";
      else if ( gender == GENDER_NEUTER )
        message = "toasted itself";
      else
        message = "toasted himself";
      break;
    case MOD_BFG_SPLASH:
      message = "should have used a smaller gun";
      break;
    default:
      if ( gender == GENDER_FEMALE )
        message = "killed herself";
      else if ( gender == GENDER_NEUTER )
        message = "killed itself";
      else
        message = "killed himself";
      break;
    }
  }

  if (message) {
    CG_Printf( "%s %s.\n", targetName, message);
    return;
  }

  // check for kill messages from the current clientNum
  if ( attacker == cg.snap->ps.clientNum ) {
    char  *s;

    if ( cgs.gametype < GT_TEAM ) {
      s = va("You fragged %s\n%s place with %i", targetName,
        CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),
        cg.snap->ps.persistant[PERS_SCORE] );
    } else {
      s = va("You fragged %s", targetName );
    }
    CG_CenterPrint( s, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );

    // print the text message as well
  }

  // check for double client messages
  if ( !attackerInfo ) {
    attacker = ENTITYNUM_WORLD;
    strcpy( attackerName, "noname" );
  } else {
    Q_strncpyz( attackerName, Info_ValueForKey( attackerInfo, "n" ), sizeof(attackerName) - 2);
    strcat( attackerName, S_COLOR_WHITE );
    // check for kill messages about the current clientNum
    if ( target == cg.snap->ps.clientNum ) {
      Q_strncpyz( cg.killerName, attackerName, sizeof( cg.killerName ) );
    }
  }

  if ( attacker != ENTITYNUM_WORLD ) {
    switch (mod) {
    case MOD_GRAPPLE:
      message = "was caught by";
      break;
    case MOD_GAUNTLET:
      message = "was pummeled by";
      break;
    case MOD_MACHINEGUN:
      message = "was machinegunned by";
      break;
    case MOD_CHAINGUN:
      message = "was chaingunned by";
      break;
    case MOD_SHOTGUN:
      message = "was gunned down by";
      break;
    case MOD_GRENADE:
      message = "ate";
      message2 = "'s grenade";
      break;
    case MOD_GRENADE_SPLASH:
      message = "was shredded by";
      message2 = "'s shrapnel";
      break;
    case MOD_ROCKET:
      message = "ate";
      message2 = "'s rocket";
      break;
    case MOD_ROCKET_SPLASH:
      message = "almost dodged";
      message2 = "'s rocket";
      break;
    case MOD_FLAMER:
      message = "was toasted by";
      message2 = "'s flamer";
      break;
    case MOD_FLAMER_SPLASH:
      message = "was toasted by";
      message2 = "'s flamer";
      break;
    case MOD_RAILGUN:
      message = "was railed by";
      break;
    case MOD_LIGHTNING:
      message = "was electrocuted by";
      break;
    case MOD_VENOM:
          message = "was biten by";
      break;
    case MOD_BFG:
    case MOD_BFG_SPLASH:
      message = "was blasted by";
      message2 = "'s BFG";
      break;
    case MOD_TELEFRAG:
      message = "tried to invade";
      message2 = "'s personal space";
      break;
    default:
      message = "was killed by";
      break;
    }

    if (message) {
      CG_Printf( "%s %s %s%s\n",
        targetName, message, attackerName, message2);
      return;
    }
  }

  // we don't know what it was
  CG_Printf( "%s died.\n", targetName );
}

//==========================================================================

/*
===============
CG_UseItem
===============
*/
static void CG_UseItem( centity_t *cent ) {
  clientInfo_t *ci;
  int     itemNum, clientNum;
  gitem_t   *item;
  entityState_t *es;

  es = &cent->currentState;

  itemNum = (es->event & ~EV_EVENT_BITS) - EV_USE_ITEM0;
  if ( itemNum < 0 || itemNum > HI_NUM_HOLDABLE ) {
    itemNum = 0;
  }

  // print a message if the local player
  if ( es->number == cg.snap->ps.clientNum ) {
    if ( !itemNum ) {
      CG_CenterPrint( "No item to use", SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
    } else {
      /*item = BG_FindItemForHoldable( itemNum );*/
      CG_CenterPrint( va("Use %s", item->pickup_name), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
    }
  }

  switch ( itemNum ) {
  default:
  case HI_NONE:
    trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.useNothingSound );
    break;

  case HI_TELEPORTER:
    break;

  case HI_MEDKIT:
    clientNum = cent->currentState.clientNum;
    if ( clientNum >= 0 && clientNum < MAX_CLIENTS ) {
      ci = &cgs.clientinfo[ clientNum ];
      ci->medkitUsageTime = cg.time;
    }
    trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.medkitSound );
    break;
  }

}

/*
================
CG_ItemPickup

A new item was picked up this frame
================
*/
static void CG_ItemPickup( int itemNum ) {
  cg.itemPickup = itemNum;
  cg.itemPickupTime = cg.time;
  cg.itemPickupBlendTime = cg.time;
  // see if it should be the grabbed weapon

}


/*
================
CG_PainEvent

Also called by playerstate transition
================
*/
void CG_PainEvent( centity_t *cent, int health ) {
  char  *snd;

  // don't do more than two pain sounds a second
  if ( cg.time - cent->pe.painTime < 500 ) {
    return;
  }

  if ( health < 25 ) {
    snd = "*pain25_1.wav";
  } else if ( health < 50 ) {
    snd = "*pain50_1.wav";
  } else if ( health < 75 ) {
    snd = "*pain75_1.wav";
  } else {
    snd = "*pain100_1.wav";
  }
  trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE,
    CG_CustomSound( cent->currentState.number, snd ) );

  // save pain time for programitic twitch animation
  cent->pe.painTime = cg.time;
  cent->pe.painDirection ^= 1;
}


/*
==============
CG_Menu
==============
*/
void CG_Menu( int eventParm )
{
  int   i;
  char  carriageCvar[ MAX_TOKEN_CHARS ];

  *carriageCvar = 0;
  
  switch( eventParm )
  {
    case MN_TEAM:       trap_SendConsoleCommand( "menu tremulous_teamselect\n" );               break;
    case MN_D_CLASS:    trap_SendConsoleCommand( "menu tremulous_alienclass\n" );               break;
    case MN_H_SPAWN:    trap_SendConsoleCommand( "menu tremulous_humanitem\n" );                break;
    case MN_D_BUILD:    trap_SendConsoleCommand( "menu tremulous_alienbuild\n" );               break;
    case MN_H_BUILD:    trap_SendConsoleCommand( "menu tremulous_humanbuild\n" );               break;
    case MN_H_MCU:
      for( i = WP_NONE +1; i < WP_NUM_WEAPONS; i++ )
      {
        if( BG_gotWeapon( i, cg.snap->ps.stats ) )
          strcat( carriageCvar, va( "W%d ", i ) );
      }
      for( i = UP_NONE +1; i < UP_NUM_UPGRADES; i++ )
      {
        if( BG_gotItem( i, cg.snap->ps.stats ) )
          strcat( carriageCvar, va( "U%d ", i ) );
      }
      strcat( carriageCvar, "$" );
          
      trap_Cvar_Set( "ui_carriage", carriageCvar );
      trap_SendConsoleCommand( "menu tremulous_humanmcu\n" );
      break;
    case MN_H_BANK:     trap_SendConsoleCommand( "menu tremulous_humanbank\n" );                break;
    case MN_H_NOROOM:   trap_SendConsoleCommand( "menu tremulous_human_no_room\n" );            break;
    case MN_H_NOPOWER:  trap_SendConsoleCommand( "menu tremulous_human_no_power\n" );           break;
    case MN_D_NOROOM:   trap_SendConsoleCommand( "menu tremulous_alien_no_room\n" );            break;
    case MN_D_NOCREEP:  trap_SendConsoleCommand( "menu tremulous_alien_no_creep\n" );           break;
    case MN_D_NOHVMND:  trap_SendConsoleCommand( "menu tremulous_alien_no_hivemind\n" );        break;
    case MN_D_HIVEMIND: trap_SendConsoleCommand( "menu tremulous_alien_one_hivemind\n" );       break;
    case MN_D_NOASSERT: trap_SendConsoleCommand( "menu tremulous_alien_no_assertion\n" );       break;
    case MN_D_SPWNWARN: trap_SendConsoleCommand( "menu tremulous_alien_egg_warning\n" );        break;
    case MN_D_NORMAL:   trap_SendConsoleCommand( "menu tremulous_alien_surface\n" );            break;
    case MN_H_REACTOR:  trap_SendConsoleCommand( "menu tremulous_human_one_reactor\n" );        break;
    case MN_H_REPEATER: trap_SendConsoleCommand( "menu tremulous_human_repeater\n" );           break;
    case MN_H_RPLWARN:  trap_SendConsoleCommand( "menu tremulous_human_replicator_warning\n" ); break;
    case MN_H_RPTWARN:  trap_SendConsoleCommand( "menu tremulous_human_repeater_warning\n" );   break;
    case MN_H_NOSLOTS:  trap_SendConsoleCommand( "menu tremulous_human_no_slots\n" );           break;
    case MN_H_NOFUNDS:  trap_SendConsoleCommand( "menu tremulous_human_no_funds\n" );           break;
    case MN_H_ITEMHELD: trap_SendConsoleCommand( "menu tremulous_human_item_held\n" );          break;
    case MN_D_INFEST:
      trap_Cvar_Set( "ui_currentClass", va( "%d", cg.snap->ps.stats[ STAT_PCLASS ] ) );
      trap_SendConsoleCommand( "menu tremulous_alienupgrade\n" );
      break;

    default:
      Com_Printf( "cgame: debug: no such menu %d\n", eventParm );
  }
}

/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
*/
#define DEBUGNAME(x) if(cg_debugEvents.integer){CG_Printf(x"\n");}
void CG_EntityEvent( centity_t *cent, vec3_t position )
{
  entityState_t *es;
  int           event;
  vec3_t        dir;
  const char    *s;
  int           clientNum;
  clientInfo_t  *ci;
  int           steptime, i;

  steptime = BG_FindSteptimeForClass( cg.predictedPlayerState.stats[ STAT_PCLASS ] );

  es = &cent->currentState;
  event = es->event & ~EV_EVENT_BITS;

  if ( cg_debugEvents.integer ) {
    CG_Printf( "ent:%3i  event:%3i ", es->number, event );
  }

  if ( !event ) {
    DEBUGNAME("ZEROEVENT");
    return;
  }

  clientNum = es->clientNum;
  if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
    clientNum = 0;
  }
  ci = &cgs.clientinfo[ clientNum ];

  switch ( event ) {
  //
  // movement generated events
  //
  case EV_FOOTSTEP:
    DEBUGNAME("EV_FOOTSTEP");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ ci->footsteps ][rand()&3] );
    }
    break;
  case EV_FOOTSTEP_METAL:
    DEBUGNAME("EV_FOOTSTEP_METAL");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ FOOTSTEP_METAL ][rand()&3] );
    }
    break;
  case EV_FOOTSTEP_SQUELCH:
    DEBUGNAME("EV_FOOTSTEP_SQUELCH");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ FOOTSTEP_FLESH ][rand()&3] );
    }
    break;
  case EV_FOOTSPLASH:
    DEBUGNAME("EV_FOOTSPLASH");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
    }
    break;
  case EV_FOOTWADE:
    DEBUGNAME("EV_FOOTWADE");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
    }
    break;
  case EV_SWIM:
    DEBUGNAME("EV_SWIM");
    if (cg_footsteps.integer) {
      trap_S_StartSound (NULL, es->number, CHAN_BODY,
        cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
    }
    break;


  case EV_FALL_SHORT:
    DEBUGNAME("EV_FALL_SHORT");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.landSound );
    if ( clientNum == cg.predictedPlayerState.clientNum ) {
      // smooth landing z changes
      cg.landChange = -8;
      cg.landTime = cg.time;
    }
    break;
  case EV_FALL_MEDIUM:
    DEBUGNAME("EV_FALL_MEDIUM");
    // use normal pain sound
    trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*pain100_1.wav" ) );
    if ( clientNum == cg.predictedPlayerState.clientNum ) {
      // smooth landing z changes
      cg.landChange = -16;
      cg.landTime = cg.time;
    }
    break;
  case EV_FALL_FAR:
    DEBUGNAME("EV_FALL_FAR");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*fall1.wav" ) );
    cent->pe.painTime = cg.time;  // don't play a pain sound right after this
    if ( clientNum == cg.predictedPlayerState.clientNum ) {
      // smooth landing z changes
      cg.landChange = -24;
      cg.landTime = cg.time;
    }
    break;

  case EV_STEP_4:
  case EV_STEP_8:
  case EV_STEP_12:
  case EV_STEP_16:    // smooth out step up transitions
    DEBUGNAME("EV_STEP");
  {
    float oldStep;
    int   delta;
    int   step;

    if ( clientNum != cg.predictedPlayerState.clientNum ) {
      break;
    }
    // if we are interpolating, we don't need to smooth steps
    if ( cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) ||
      cg_nopredict.integer || cg_synchronousClients.integer ) {
      break;
    }
    // check for stepping up before a previous step is completed
    delta = cg.time - cg.stepTime;
    if (delta < steptime) {
      oldStep = cg.stepChange * (steptime - delta) / steptime;
    } else {
      oldStep = 0;
    }

    // add this amount
    step = 4 * (event - EV_STEP_4 + 1 );
    cg.stepChange = oldStep + step;
    if ( cg.stepChange > MAX_STEP_CHANGE ) {
      cg.stepChange = MAX_STEP_CHANGE;
    }
    cg.stepTime = cg.time;
    break;
  }

  case EV_JUMP_PAD:
    DEBUGNAME("EV_JUMP_PAD");
//    CG_Printf( "EV_JUMP_PAD w/effect #%i\n", es->eventParm );
    {
      localEntity_t *smoke;
      vec3_t      up = {0, 0, 1};


      smoke = CG_SmokePuff( cent->lerpOrigin, up, 
              32, 
              1, 1, 1, 0.33f,
              1000, 
              cg.time, 0,
              LEF_PUFF_DONT_SCALE, 
              cgs.media.smokePuffShader );
    }
    // boing sound at origin, jump sound on player
    trap_S_StartSound ( cent->lerpOrigin, -1, CHAN_VOICE, cgs.media.jumpPadSound );
    trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );
    break;

  case EV_JUMP:
    DEBUGNAME("EV_JUMP");
    trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );
    break;
  case EV_TAUNT:
    DEBUGNAME("EV_TAUNT");
    trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*taunt.wav" ) );
    break;
  case EV_WATER_TOUCH:
    DEBUGNAME("EV_WATER_TOUCH");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrInSound );
    break;
  case EV_WATER_LEAVE:
    DEBUGNAME("EV_WATER_LEAVE");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound );
    break;
  case EV_WATER_UNDER:
    DEBUGNAME("EV_WATER_UNDER");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound );
    break;
  case EV_WATER_CLEAR:
    DEBUGNAME("EV_WATER_CLEAR");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*gasp.wav" ) );
    break;

  case EV_ITEM_PICKUP:
    DEBUGNAME("EV_ITEM_PICKUP");
    {
    }
    break;

  case EV_GLOBAL_ITEM_PICKUP:
    DEBUGNAME("EV_GLOBAL_ITEM_PICKUP");
    {
    }
    break;

  //
  // weapon events
  //
  case EV_NOAMMO:
    DEBUGNAME("EV_NOAMMO");
//    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound );
    if ( es->number == cg.snap->ps.clientNum ) {
      CG_OutOfAmmoChange();
    }
    break;
  case EV_CHANGE_WEAPON:
    DEBUGNAME("EV_CHANGE_WEAPON");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.selectSound );
    break;
  case EV_FIRE_WEAPON:
    DEBUGNAME("EV_FIRE_WEAPON");
    CG_FireWeapon( cent );
    break;
  case EV_FIRE_WEAPON2:
    DEBUGNAME("EV_FIRE_WEAPON2");
    CG_FireWeapon( cent ); //FIXME:??
    break;
  case EV_FIRE_WEAPONBOTH:
    DEBUGNAME("EV_FIRE_WEAPONBOTH");
    CG_FireWeapon( cent ); //FIXME:??
    break;
  case EV_USE_ITEM0:
    DEBUGNAME("EV_USE_ITEM0");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM1:
    DEBUGNAME("EV_USE_ITEM1");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM2:
    DEBUGNAME("EV_USE_ITEM2");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM3:
    DEBUGNAME("EV_USE_ITEM3");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM4:
    DEBUGNAME("EV_USE_ITEM4");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM5:
    DEBUGNAME("EV_USE_ITEM5");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM6:
    DEBUGNAME("EV_USE_ITEM6");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM7:
    DEBUGNAME("EV_USE_ITEM7");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM8:
    DEBUGNAME("EV_USE_ITEM8");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM9:
    DEBUGNAME("EV_USE_ITEM9");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM10:
    DEBUGNAME("EV_USE_ITEM10");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM11:
    DEBUGNAME("EV_USE_ITEM11");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM12:
    DEBUGNAME("EV_USE_ITEM12");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM13:
    DEBUGNAME("EV_USE_ITEM13");
    CG_UseItem( cent );
    break;
  case EV_USE_ITEM14:
    DEBUGNAME("EV_USE_ITEM14");
    CG_UseItem( cent );
    break;

  //=================================================================

  //
  // other events
  //
  case EV_PLAYER_TELEPORT_IN:
    DEBUGNAME("EV_PLAYER_TELEPORT_IN");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleInSound );
    CG_SpawnEffect( position);
    break;

  case EV_PLAYER_TELEPORT_OUT:
    DEBUGNAME("EV_PLAYER_TELEPORT_OUT");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound );
    CG_SpawnEffect(  position);
    break;

  case EV_ITEM_POP:
    DEBUGNAME("EV_ITEM_POP");
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
    break;
  case EV_ITEM_RESPAWN:
    DEBUGNAME("EV_ITEM_RESPAWN");
    cent->miscTime = cg.time; // scale up from this
    trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
    break;

  //TA: make alien items "grow"
  case EV_BUILD_CONSTRUCT:
    DEBUGNAME("EV_BUILD_CONSTRUCT");
    cent->miscTime = cg.time; // scale up from this
    //probably a better place for this, but for the time being it lives here
    memset( &cent->lerpFrame, 0, sizeof( lerpFrame_t ) );
    cent->buildableAnim = es->eventParm;
    break;
    
  //TA: make alien creep "recede"
  case EV_BUILD_DESTROY:
    DEBUGNAME("EV_BUILD_DESTROY");
    cent->miscTime = -cg.time; // scale down from this
    break;

  //TA: trigger an anim on a buildable item
  case EV_BUILD_ANIM:
    DEBUGNAME("EV_BUILD_ANIM");
    if( cent->buildableAnim == es->torsoAnim || es->eventParm & ANIM_TOGGLEBIT )
      cent->buildableAnim = es->eventParm;
    break;
    
  case EV_GRENADE_BOUNCE:
    DEBUGNAME("EV_GRENADE_BOUNCE");
    if ( rand() & 1 ) {
      trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.hgrenb1aSound );
    } else {
      trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.hgrenb2aSound );
    }
    break;

  //
  // missile impacts
  //
  case EV_MISSILE_HIT:
    DEBUGNAME("EV_MISSILE_HIT");
    ByteToDir( es->eventParm, dir );
    CG_MissileHitPlayer( es->weapon, position, dir, es->otherEntityNum );
    break;

  case EV_MISSILE_MISS:
    DEBUGNAME("EV_MISSILE_MISS");
    ByteToDir( es->eventParm, dir );
    CG_MissileHitWall( es->weapon, 0, position, dir, IMPACTSOUND_DEFAULT );
    break;

  case EV_MISSILE_MISS_METAL:
    DEBUGNAME("EV_MISSILE_MISS_METAL");
    ByteToDir( es->eventParm, dir );
    CG_MissileHitWall( es->weapon, 0, position, dir, IMPACTSOUND_METAL );
    break;

  case EV_ITEM_EXPLOSION:
    DEBUGNAME("EV_ITEM_EXPLOSION");
    ByteToDir( es->eventParm, dir );
    CG_Explosion( 0, position, dir );
    break;

  case EV_RAILTRAIL:
/*    DEBUGNAME("EV_RAILTRAIL");
    cent->currentState.weapon = WP_RAILGUN;
    // if the end was on a nomark surface, don't make an explosion
    CG_RailTrail( es->origin2, es->pos.trBase );
    if ( es->eventParm != 255 ) {
      ByteToDir( es->eventParm, dir );
      CG_MissileHitWall( es->weapon, es->clientNum, position, dir, IMPACTSOUND_DEFAULT );
    }*/
    break;

  case EV_TESLATRAIL:
    DEBUGNAME("EV_TESLATRAIL");
    cent->currentState.weapon = WP_TESLAGEN;
    // if the end was on a nomark surface, don't make an explosion
    CG_TeslaTrail( es->origin2, es->pos.trBase );
    if ( es->eventParm != 255 ) {
      ByteToDir( es->eventParm, dir );
      CG_MissileHitWall( es->weapon, es->clientNum, position, dir, IMPACTSOUND_DEFAULT );
    }
    break;

  case EV_BULLET_HIT_WALL:
    DEBUGNAME("EV_BULLET_HIT_WALL");
    ByteToDir( es->eventParm, dir );
    CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qfalse, ENTITYNUM_WORLD );
    break;

  case EV_BULLET_HIT_FLESH:
    DEBUGNAME("EV_BULLET_HIT_FLESH");
    CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qtrue, es->eventParm );
    break;

#define MASS_EJECTION_VEL 300
  case EV_MASS_DRIVER_HIT:
    DEBUGNAME("EV_MASS_DRIVER_HIT");
    for( i = 0; i <= 10; i++ )
    {
      qhandle_t spark;
      vec3_t    velocity;
      vec3_t    accel = { 0.0f, 0.0f, -DEFAULT_GRAVITY };
      vec3_t    origin, normal;

      ByteToDir( es->eventParm, normal );

      VectorMA( es->pos.trBase, 10.0f, normal, origin );

      if( crandom( ) > 0.5f )
        spark = cgs.media.gibSpark1;
      else
        spark = cgs.media.gibSpark2;

      velocity[ 0 ] = crandom( ) * MASS_EJECTION_VEL;
      velocity[ 1 ] = crandom( ) * MASS_EJECTION_VEL;
      velocity[ 2 ] = MASS_EJECTION_VEL + crandom( ) * MASS_EJECTION_VEL;
      
      CG_LaunchSprite( origin, velocity, accel, 0.6, 4.0f, 2.0f, 255, 0, rand( ) % 360,
                       cg.time, 5000 + ( crandom( ) * 3000 ),
                       spark, qfalse, qfalse );
    }

    break;

  case EV_SHOTGUN:
    DEBUGNAME("EV_SHOTGUN");
    CG_ShotgunFire( es );
    break;

  case EV_GENERAL_SOUND:
    DEBUGNAME("EV_GENERAL_SOUND");
    if ( cgs.gameSounds[ es->eventParm ] ) {
      trap_S_StartSound (NULL, es->number, CHAN_VOICE, cgs.gameSounds[ es->eventParm ] );
    } else {
      s = CG_ConfigString( CS_SOUNDS + es->eventParm );
      trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, s ) );
    }
    break;

  case EV_GLOBAL_SOUND: // play from the player's head so it never diminishes
    DEBUGNAME("EV_GLOBAL_SOUND");
    if ( cgs.gameSounds[ es->eventParm ] ) {
      trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.gameSounds[ es->eventParm ] );
    } else {
      s = CG_ConfigString( CS_SOUNDS + es->eventParm );
      trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, CG_CustomSound( es->number, s ) );
    }
    break;

  case EV_PAIN:
    // local player sounds are triggered in CG_CheckLocalSounds,
    // so ignore events on the player
    DEBUGNAME("EV_PAIN");
    if ( cent->currentState.number != cg.snap->ps.clientNum ) {
      CG_PainEvent( cent, es->eventParm );
    }
    break;

  case EV_DEATH1:
  case EV_DEATH2:
  case EV_DEATH3:
    DEBUGNAME("EV_DEATHx");
    trap_S_StartSound( NULL, es->number, CHAN_VOICE,
        CG_CustomSound( es->number, va("*death%i.wav", event - EV_DEATH1 + 1) ) );
    break;


  case EV_OBITUARY:
    DEBUGNAME("EV_OBITUARY");
    CG_Obituary( es );
    break;

  //
  // powerup events
  //
  case EV_POWERUP_QUAD:
    DEBUGNAME("EV_POWERUP_QUAD");
    if ( es->number == cg.snap->ps.clientNum ) {
      cg.powerupActive = PW_QUAD;
      cg.powerupTime = cg.time;
    }
    trap_S_StartSound (NULL, es->number, CHAN_ITEM, cgs.media.quadSound );
    break;
  case EV_POWERUP_BATTLESUIT:
    DEBUGNAME("EV_POWERUP_BATTLESUIT");
    if ( es->number == cg.snap->ps.clientNum ) {
      cg.powerupActive = PW_BATTLESUIT;
      cg.powerupTime = cg.time;
    }
    trap_S_StartSound (NULL, es->number, CHAN_ITEM, cgs.media.protectSound );
    break;
  case EV_POWERUP_REGEN:
    DEBUGNAME("EV_POWERUP_REGEN");
    if ( es->number == cg.snap->ps.clientNum ) {
      cg.powerupActive = PW_REGEN;
      cg.powerupTime = cg.time;
    }
    trap_S_StartSound (NULL, es->number, CHAN_ITEM, cgs.media.regenSound );
    break;

  case EV_GIB_PLAYER:
    DEBUGNAME("EV_GIB_PLAYER");
    trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.gibSound );
    CG_GibPlayer( cent->lerpOrigin );
    break;

  case EV_GIB_ALIEN:
    DEBUGNAME("EV_GIB_ALIEN");
    trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.gibSound );
    CG_GenericGib( cent->lerpOrigin );
    break;

  case EV_STOPLOOPINGSOUND:
    DEBUGNAME("EV_STOPLOOPINGSOUND");
    trap_S_StopLoopingSound( es->number );
    es->loopSound = 0;
    break;
                  
  case EV_DEBUG_LINE:
    DEBUGNAME("EV_DEBUG_LINE");
    CG_Beam( cent );
    break;

  case EV_MENU:
    DEBUGNAME("EV_MENU");
    if( es->number == cg.clientNum )
      CG_Menu( es->eventParm );
    break;

  case EV_PLAYER_RESPAWN:
    DEBUGNAME("EV_PLAYER_RESPAWN");
    if( es->number == cg.clientNum )
      cg.spawnTime = cg.time;
    break;

  default:
    DEBUGNAME("UNKNOWN");
    CG_Error( "Unknown event: %i", event );
    break;
  }

}


/*
==============
CG_CheckEvents

==============
*/
void CG_CheckEvents( centity_t *cent ) {
  // check for event-only entities
  if ( cent->currentState.eType > ET_EVENTS ) {
    if ( cent->previousEvent ) {
      return; // already fired
    }
    cent->previousEvent = 1;

    cent->currentState.event = cent->currentState.eType - ET_EVENTS;
  } else {
    // check for events riding with another entity
    if ( cent->currentState.event == cent->previousEvent ) {
      return;
    }
    cent->previousEvent = cent->currentState.event;
    if ( ( cent->currentState.event & ~EV_EVENT_BITS ) == 0 ) {
      return;
    }
  }

  // calculate the position at exactly the frame time
  BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin );
  CG_SetEntitySoundPosition( cent );

  CG_EntityEvent( cent, cent->lerpOrigin );
}

