// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_playerstate.c -- this file acts on changes in a new playerState_t
// With normal play, this will be done after local prediction, but when
// following another player or playing back a demo, it will be checked
// when the snapshot transitions like all the other entities

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

/*
==============
CG_CheckAmmo

If the ammo has gone low enough to generate the warning, play a sound
==============
*/
void CG_CheckAmmo( void ) {
	int		    i;
	int		    total;
	int		    previous;
  int       ammo, clips, maxclips;

	// see about how many seconds of ammo we have remaining
	total = 0;
	for ( i = WP_MACHINEGUN ; i < WP_NUM_WEAPONS ; i++ ) {
		if ( !BG_gotWeapon( i, cg.snap->ps.stats ) ) {
			continue;
		}

    BG_unpackAmmoArray( i, cg.snap->ps.ammo, cg.snap->ps.powerups, &ammo, &clips, &maxclips );
    
		switch ( i ) {
		/*case WP_ROCKET_LAUNCHER:*/
		case WP_LOCKBLOB_LAUNCHER:
		/*case WP_GRENADE_LAUNCHER:*/
		/*case WP_RAILGUN:*/
		/*case WP_SHOTGUN:*/
			total += ammo * 1000;
			break;
		default:
      if( clips )
        total += 5000;
			total += ammo * 200;
			break;
		}
		if ( total >= 5000 || BG_FindInfinteAmmoForWeapon( i ) )
    {
			cg.lowAmmoWarning = 0;
			return;
		}
	}

	previous = cg.lowAmmoWarning;

	if ( total == 0 ) {
		cg.lowAmmoWarning = 2;
	} else {
		cg.lowAmmoWarning = 1;
	}

	// play a sound on transitions
	if ( cg.lowAmmoWarning != previous ) {
		trap_S_StartLocalSound( cgs.media.noAmmoSound, CHAN_LOCAL_SOUND );
	}
}

/*
==============
CG_DamageFeedback
==============
*/
void CG_DamageFeedback( int yawByte, int pitchByte, int damage ) {
	float		left, front, up;
	float		kick;
	int			health;
	float		scale;
	vec3_t		dir;
	vec3_t		angles;
	float		dist;
	float		yaw, pitch;

	// show the attacking player's head and name in corner
	cg.attackerTime = cg.time;

	// the lower on health you are, the greater the view kick will be
	health = cg.snap->ps.stats[STAT_HEALTH];
	if ( health < 40 ) {
		scale = 1;
	} else {
		scale = 40.0 / health;
	}
	kick = damage * scale;

	if (kick < 5)
		kick = 5;
	if (kick > 10)
		kick = 10;

	// if yaw and pitch are both 255, make the damage always centered (falling, etc)
	if ( yawByte == 255 && pitchByte == 255 ) {
		cg.damageX = 0;
		cg.damageY = 0;
		cg.v_dmg_roll = 0;
		cg.v_dmg_pitch = -kick;
	} else {
		// positional
		pitch = pitchByte / 255.0 * 360;
		yaw = yawByte / 255.0 * 360;

		angles[PITCH] = pitch;
		angles[YAW] = yaw;
		angles[ROLL] = 0;

		AngleVectors( angles, dir, NULL, NULL );
		VectorSubtract( vec3_origin, dir, dir );

		front = DotProduct (dir, cg.refdef.viewaxis[0] );
		left = DotProduct (dir, cg.refdef.viewaxis[1] );
		up = DotProduct (dir, cg.refdef.viewaxis[2] );

		dir[0] = front;
		dir[1] = left;
		dir[2] = 0;
		dist = VectorLength( dir );
		if ( dist < 0.1 ) {
			dist = 0.1f;
		}

		cg.v_dmg_roll = kick * left;
		
		cg.v_dmg_pitch = -kick * front;

		if ( front <= 0.1 ) {
			front = 0.1f;
		}
		cg.damageX = -left / front;
		cg.damageY = up / dist;
	}

	// clamp the position
	if ( cg.damageX > 1.0 ) {
		cg.damageX = 1.0;
	}
	if ( cg.damageX < - 1.0 ) {
		cg.damageX = -1.0;
	}

	if ( cg.damageY > 1.0 ) {
		cg.damageY = 1.0;
	}
	if ( cg.damageY < - 1.0 ) {
		cg.damageY = -1.0;
	}

	// don't let the screen flashes vary as much
	if ( kick > 10 ) {
		kick = 10;
	}
	cg.damageValue = kick;
	cg.v_dmg_time = cg.time + DAMAGE_TIME;
	cg.damageTime = cg.snap->serverTime;
}




/*
================
CG_Respawn

A respawn happened this snapshot
================
*/
void CG_Respawn( void ) {
	// no error decay on player movement
	cg.thisFrameTeleport = qtrue;

	// display weapons available
	cg.weaponSelectTime = cg.time;

	// select the weapon the server says we are using
	cg.weaponSelect = cg.snap->ps.weapon;
}

extern char *eventnames[];

/*
==============
CG_CheckPlayerstateEvents

==============
*/
void CG_CheckPlayerstateEvents( playerState_t *ps, playerState_t *ops ) {
	int			i;
	int			event;
	centity_t	*cent;

	if ( ps->externalEvent && ps->externalEvent != ops->externalEvent ) {
		cent = &cg_entities[ ps->clientNum ];
		cent->currentState.event = ps->externalEvent;
		cent->currentState.eventParm = ps->externalEventParm;
		CG_EntityEvent( cent, cent->lerpOrigin );
	}

	cent = &cg.predictedPlayerEntity; // cg_entities[ ps->clientNum ];

  // go through the predictable events buffer
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
    // if we have a new predictable event
    if ( i >= ops->eventSequence
      // or the server told us to play another event instead of a predicted event we already issued
      // or something the server told us changed our prediction causing a different event
      || (i > ops->eventSequence - MAX_PS_EVENTS && ps->events[i & (MAX_PS_EVENTS-1)] != ops->events[i & (MAX_PS_EVENTS-1)]) ) {

      event = ps->events[ i & (MAX_PS_EVENTS-1) ];

			cent->currentState.event = event;
			cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
			CG_EntityEvent( cent, cent->lerpOrigin );
      cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

      cg.eventSequence++;
		}
	}
}


/*
==================
CG_CheckChangedPredictableEvents
==================
*/
void CG_CheckChangedPredictableEvents( playerState_t *ps ) {
  int i;
  int event;
  centity_t *cent;

  cent = &cg.predictedPlayerEntity;
  for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
    //
    if (i >= cg.eventSequence) {
      continue;
    }
    // if this event is not further back in than the maximum predictable events we remember
    if (i > cg.eventSequence - MAX_PREDICTED_EVENTS) {
      // if the new playerstate event is different from a previously predicted one
      if ( ps->events[i & (MAX_PS_EVENTS-1)] != cg.predictableEvents[i & (MAX_PREDICTED_EVENTS-1) ] ) {

        event = ps->events[ i & (MAX_PS_EVENTS-1) ];
        cent->currentState.event = event;
        cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
        CG_EntityEvent( cent, cent->lerpOrigin );

        cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

        if ( cg_showmiss.integer ) {
          CG_Printf("WARNING: changed predicted event\n");
        }
      }
    }
  }
}

/*
==================
pushReward
==================
*/
static void pushReward(sfxHandle_t sfx, qhandle_t shader, int rewardCount) {
  if (cg.rewardStack < (MAX_REWARDSTACK-1)) {
    cg.rewardStack++;
    cg.rewardSound[cg.rewardStack] = sfx;
    cg.rewardShader[cg.rewardStack] = shader;
    cg.rewardCount[cg.rewardStack] = rewardCount;
  }
}


/*
==================
CG_CheckLocalSounds
==================
*/
void CG_CheckLocalSounds( playerState_t *ps, playerState_t *ops ) {
  int     highScore, health, armor, reward;
  sfxHandle_t sfx;

  // don't play the sounds if the player just changed teams
  if ( ps->persistant[PERS_TEAM] != ops->persistant[PERS_TEAM] ) {
    return;
  }

  // hit changes
  if ( ps->persistant[PERS_HITS] > ops->persistant[PERS_HITS] ) {
    /*armor  = ps->persistant[PERS_ATTACKEE_ARMOR] & 0xff;
    health = ps->persistant[PERS_ATTACKEE_ARMOR] >> 8;*/
    trap_S_StartLocalSound( cgs.media.hitSound, CHAN_LOCAL_SOUND );
  } else if ( ps->persistant[PERS_HITS] < ops->persistant[PERS_HITS] ) {
    trap_S_StartLocalSound( cgs.media.hitTeamSound, CHAN_LOCAL_SOUND );
  }

  // health changes of more than -1 should make pain sounds
  if ( ps->stats[STAT_HEALTH] < ops->stats[STAT_HEALTH] - 1 ) {
    if ( ps->stats[STAT_HEALTH] > 0 ) {
      CG_PainEvent( &cg.predictedPlayerEntity, ps->stats[STAT_HEALTH] );
    }
  }


  // if we are going into the intermission, don't start any voices
  if ( cg.intermissionStarted ) {
    return;
  }

  // reward sounds
  reward = qfalse;

  // timelimit warnings
  if ( cgs.timelimit > 0 ) {
    int   msec;

    msec = cg.time - cgs.levelStartTime;
    if ( !( cg.timelimitWarnings & 4 ) && msec > ( cgs.timelimit * 60 + 2 ) * 1000 ) {
      cg.timelimitWarnings |= 1 | 2 | 4;
      trap_S_StartLocalSound( cgs.media.suddenDeathSound, CHAN_ANNOUNCER );
    }
    else if ( !( cg.timelimitWarnings & 2 ) && msec > (cgs.timelimit - 1) * 60 * 1000 ) {
      cg.timelimitWarnings |= 1 | 2;
      trap_S_StartLocalSound( cgs.media.oneMinuteSound, CHAN_ANNOUNCER );
    }
    else if ( cgs.timelimit > 5 && !( cg.timelimitWarnings & 1 ) && msec > (cgs.timelimit - 5) * 60 * 1000 ) {
      cg.timelimitWarnings |= 1;
      trap_S_StartLocalSound( cgs.media.fiveMinuteSound, CHAN_ANNOUNCER );
    }
  }
}


/*
===============
CG_TransitionPlayerState

===============
*/
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops ) {
	// check for changing follow mode
	if ( ps->clientNum != ops->clientNum ) {
		cg.thisFrameTeleport = qtrue;
		// make sure we don't get any unwanted transition effects
		*ops = *ps;
	}

	// damage events (player is getting wounded)
	if ( ps->damageEvent != ops->damageEvent && ps->damageCount ) {
		CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount );
	}

	// respawning
	if ( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT] ) {
		CG_Respawn();
	}

  if ( cg.mapRestart ) {
    CG_Respawn();
    cg.mapRestart = qfalse;
  }

	if ( cg.snap->ps.pm_type != PM_INTERMISSION 
		&& ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		CG_CheckLocalSounds( ps, ops );
	}

	// check for going low on ammo
	//CG_CheckAmmo(); //TA: don't display ammo warnings

	// run events
	CG_CheckPlayerstateEvents( ps, ops );

	// smooth the ducking viewheight change
	if ( ps->viewheight != ops->viewheight ) {
		cg.duckChange = ps->viewheight - ops->viewheight;
		cg.duckTime = cg.time;
	}
}

