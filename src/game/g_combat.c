// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_combat.c

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

#include "g_local.h"

damageRegion_t  g_damageRegions[ PCL_NUM_CLASSES ][ MAX_LOCDAMAGE_REGIONS ];
int             g_numDamageRegions[ PCL_NUM_CLASSES ];

armourRegion_t  g_armourRegions[ UP_NUM_UPGRADES ][ MAX_ARMOUR_REGIONS ];
int             g_numArmourRegions[ UP_NUM_UPGRADES ];

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score ) {
  if ( !ent->client ) {
    return;
  }
  // no scoring during pre-match warmup
  if ( level.warmupTime ) {
    return;
  }
  ent->client->ps.persistant[PERS_SCORE] += score;
  if (g_gametype.integer == GT_TEAM)
    level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
  CalculateRanks();
}


/*
============
AddPoints

Adds points to both the client and his team
============
*/
void AddPoints( gentity_t *ent, int score )
{
  if ( !ent->client ) {
    return;
  }
  // no scoring during pre-match warmup
  if ( level.warmupTime ) {
    return;
  }
  ent->client->ps.persistant[PERS_POINTS] += score;
  ent->client->ps.persistant[PERS_TOTALPOINTS] += score;
  
  /*if (g_gametype.integer == GT_TEAM)
    level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
  CalculateRanks();*/
}


/*
============
CalculatePoints

Calculates the points to given to a client
============
*/
int CalculatePoints( gentity_t *victim, gentity_t *attacker )
{
  int victim_value, attacker_value;

  if( !victim->client || !attacker->client )
    return 0;

  return 1;

}

  
/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self ) {
  gitem_t   *item;
  int     weapon;
  float   angle;
  int     i;
  gentity_t *drop;
  int     ammo, clips, maxclips;

  // drop the weapon if not a gauntlet or machinegun
  weapon = self->s.weapon;

  BG_unpackAmmoArray( weapon, self->client->ps.ammo, self->client->ps.powerups, &ammo, &clips, &maxclips );

  // make a special check to see if they are changing to a new
  // weapon that isn't the mg or gauntlet.  Without this, a client
  // can pick up a weapon, be killed, and not drop the weapon because
  // their weapon change hasn't completed yet and they are still holding the MG.
  if( weapon == WP_MACHINEGUN ) {
    if ( self->client->ps.weaponstate == WEAPON_DROPPING ) {
      weapon = self->client->pers.cmd.weapon;
    }
    if ( !BG_gotWeapon( weapon, self->client->ps.stats ) ) {
      weapon = WP_NONE;
    }
  }
}


/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
  vec3_t    dir;
  vec3_t    angles;

  if ( attacker && attacker != self )
    VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
  else if ( inflictor && inflictor != self )
    VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
  else
  {
    self->client->ps.generic1 = self->s.angles[YAW];
    return;
  }

  self->client->ps.generic1 = vectoyaw ( dir );

  angles[YAW] = vectoyaw ( dir );
  angles[PITCH] = 0;
  angles[ROLL] = 0;
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer )
{
  if( self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    G_AddEvent( self, EV_GIB_PLAYER, killer );
  else
    G_AddEvent( self, EV_GIB_ALIEN, killer );
    
  self->takedamage = qfalse;
  self->s.eType = ET_INVISIBLE;
  self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
  if ( self->health > GIB_HEALTH ) {
    return;
  }
  if ( !g_blood.integer ) {
    self->health = GIB_HEALTH+1;
    return;
  }

  //TA: no gibbing
  //GibEntity( self, 0 );
}


// these are just for logging, the client prints its own messages
char  *modNames[] = {
  "MOD_UNKNOWN",
  "MOD_SHOTGUN",
  "MOD_GAUNTLET",
  "MOD_MACHINEGUN",
  "MOD_CHAINGUN",
  "MOD_GRENADE",
  "MOD_GRENADE_SPLASH",
  "MOD_ROCKET",
  "MOD_ROCKET_SPLASH",
  "MOD_FLAMER",
  "MOD_FLAMER_SPLASH",
  "MOD_RAILGUN",
  "MOD_LIGHTNING",
  "MOD_BFG",
  "MOD_BFG_SPLASH",
  "MOD_WATER",
  "MOD_SLIME",
  "MOD_LAVA",
  "MOD_CRUSH",
  "MOD_TELEFRAG",
  "MOD_FALLING",
  "MOD_SUICIDE",
  "MOD_TARGET_LASER",
  "MOD_TRIGGER_HURT",
  "MOD_GRAPPLE",
  "MOD_VENOM",
  "MOD_HSPAWN",
  "MOD_DSPAWN"
};


/*
==================
CheckAlmostCapture
==================
*/
void CheckAlmostCapture( gentity_t *self, gentity_t *attacker ) {
  gentity_t *ent;
  vec3_t    dir;
  char    *classname;

  // if this player was carrying a flag
  /*if ( self->client->ps.powerups[PW_REDFLAG] ||
    self->client->ps.powerups[PW_BLUEFLAG] ||
    self->client->ps.powerups[PW_NEUTRALFLAG] ) {
    // get the goal flag this player should have been going for
    if ( g_gametype.integer == GT_CTF ) {
      if ( self->client->sess.sessionTeam == TEAM_ALIENS ) {
        classname = "team_CTF_blueflag";
      }
      else {
        classname = "team_CTF_redflag";
      }
    }
    else {
      if ( self->client->sess.sessionTeam == TEAM_ALIENS ) {
        classname = "team_CTF_redflag";
      }
      else {
        classname = "team_CTF_blueflag";
      }
    }
    ent = NULL;
    do
    {
      ent = G_Find(ent, FOFS(classname), classname);
    } while (ent && (ent->flags & FL_DROPPED_ITEM));
    // if we found the destination flag and it's not picked up
    if (ent && !(ent->r.svFlags & SVF_NOCLIENT) ) {
      // if the player was *very* close
      VectorSubtract( self->client->ps.origin, ent->s.origin, dir );
      if ( VectorLength(dir) < 200 ) {
        self->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
        if ( attacker->client ) {
          attacker->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
        }
      }
    }
  }*/
}

/*
==================
CheckAlmostScored
==================
*/
void CheckAlmostScored( gentity_t *self, gentity_t *attacker ) {
  /*gentity_t *ent;
  vec3_t    dir;
  char    *classname;

  // if the player was carrying cubes
  if ( self->client->ps.generic1 ) {
    if ( self->client->sess.sessionTeam == TEAM_ALIENS ) {
      classname = "team_redobelisk";
    }
    else {
      classname = "team_blueobelisk";
    }
    ent = G_Find(NULL, FOFS(classname), classname);
    // if we found the destination obelisk
    if ( ent ) {
      // if the player was *very* close
      VectorSubtract( self->client->ps.origin, ent->s.origin, dir );
      if ( VectorLength(dir) < 200 ) {
        self->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
        if ( attacker->client ) {
          attacker->client->ps.persistant[PERS_PLAYEREVENTS] ^= PLAYEREVENT_HOLYSHIT;
        }
      }
    }
  }*/
}


/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
  gentity_t *ent;
  int     anim;
  int     contents;
  int     killer;
  int     i;
  char    *killerName, *obit;

  if ( self->client->ps.pm_type == PM_DEAD ) {
    return;
  }

  if ( level.intermissiontime ) {
    return;
  }

  //TA: prolly dont need this
  // check for an almost capture
  //CheckAlmostCapture( self, attacker );
  // check for a player that almost brought in cubes
  //CheckAlmostScored( self, attacker );

  self->client->ps.pm_type = PM_DEAD;

  if ( attacker ) {
    killer = attacker->s.number;
    if ( attacker->client ) {
      killerName = attacker->client->pers.netname;
    } else {
      killerName = "<non-client>";
    }
  } else {
    killer = ENTITYNUM_WORLD;
    killerName = "<world>";
  }

  if ( killer < 0 || killer >= MAX_CLIENTS ) {
    killer = ENTITYNUM_WORLD;
    killerName = "<world>";
  }

  if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
    obit = "<bad obituary>";
  } else {
    obit = modNames[ meansOfDeath ];
  }

  G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n",
    killer, self->s.number, meansOfDeath, killerName,
    self->client->pers.netname, obit );

  // broadcast the death event to everyone
  ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
  ent->s.eventParm = meansOfDeath;
  ent->s.otherEntityNum = self->s.number;
  ent->s.otherEntityNum2 = killer;
  ent->r.svFlags = SVF_BROADCAST; // send to everyone

  self->enemy = attacker;

  self->client->ps.persistant[PERS_KILLED]++;

  if (attacker && attacker->client) {
    attacker->client->lastkilled_client = self->s.number;
    if ( attacker == self || OnSameTeam (self, attacker ) ) {
      AddScore( attacker, -1 );
      AddPoints( attacker, -10 );
    } else {
      AddScore( attacker, 1 );
      AddPoints( attacker, CalculatePoints( self, attacker ) );

      attacker->client->lastKillTime = level.time;
      
      if( attacker->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
        level.alienKills++;
      else if( attacker->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
        level.humanKills++;
    }
  } else {
    AddScore( self, -1 );
    AddPoints( self, -10 );
  }

  // Add team bonuses
  //Team_FragBonuses(self, inflictor, attacker);

  // if client is in a nodrop area, don't drop anything (but return CTF flags!)
  contents = trap_PointContents( self->r.currentOrigin, -1 );
  if ( !( contents & CONTENTS_NODROP ) )
    //TossClientItems( self );

  Cmd_Score_f( self );    // show scores
  // send updated scores to any clients that are following this one,
  // or they would get stale scoreboards
  for ( i = 0 ; i < level.maxclients ; i++ ) {
    gclient_t *client;

    client = &level.clients[i];
    if ( client->pers.connected != CON_CONNECTED ) {
      continue;
    }
    if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
      continue;
    }
    if ( client->sess.spectatorClient == self->s.number ) {
      Cmd_Score_f( g_entities + i );
    }
  }

  self->client->pers.pclass = 0; //TA: reset the classtype

  self->takedamage = qtrue; // can still be gibbed

  self->s.weapon = WP_NONE;
  self->s.powerups = 0;
  self->r.contents = CONTENTS_BODY;
  //self->r.contents = CONTENTS_CORPSE;

  self->s.angles[0] = 0;
  self->s.angles[2] = 0;
  LookAtKiller (self, inflictor, attacker);

  VectorCopy( self->s.angles, self->client->ps.viewangles );

  self->s.loopSound = 0;

  self->r.maxs[2] = -8;

  // don't allow respawn until the death anim is done
  // g_forcerespawn may force spawning at some later time
  self->client->respawnTime = level.time + 1700;

  // remove powerups
  memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

  // never gib in a nodrop
  /*if ( self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) && g_blood.integer ) {
    // gib death
    GibEntity( self, killer );
  } else*/ {
    // normal death
    static int i;

    switch ( i ) {
    case 0:
      anim = BOTH_DEATH1;
      break;
    case 1:
      anim = BOTH_DEATH2;
      break;
    case 2:
    default:
      anim = BOTH_DEATH3;
      break;
    }

    // for the no-blood option, we need to prevent the health
    // from going to gib level
    if ( self->health <= GIB_HEALTH ) {
      self->health = GIB_HEALTH+1;
    }

    self->client->ps.legsAnim =
      ( ( self->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
    self->client->ps.torsoAnim =
      ( ( self->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

    G_AddEvent( self, EV_DEATH1 + i, killer );

    // the body can still be gibbed
    self->die = body_die;

    // globally cycle through the different death animations
    i = ( i + 1 ) % 3;
  }

  trap_LinkEntity (self);
}


/*
================
CheckArmor
================
*/
int CheckArmor( gentity_t *ent, int damage, int dflags )
{
  gclient_t *client;
  int     save;
  int     count;

  if (!damage)
    return 0;

  client = ent->client;

  if (!client)
    return 0;

  if (dflags & DAMAGE_NO_ARMOR)
    return 0;

  // armor
  count = client->ps.stats[STAT_ARMOR];
  save = ceil( damage * ARMOR_PROTECTION );
  if (save >= count)
    save = count;

  if (!save)
    return 0;

  client->ps.stats[STAT_ARMOR] -= save;

  return save;
}


/*
================
RaySphereIntersections
================
*/
int RaySphereIntersections( vec3_t origin, float radius, vec3_t point, vec3_t dir, vec3_t intersections[2] ) {
  float b, c, d, t;

  //  | origin - (point + t * dir) | = radius
  //  a = dir[0]^2 + dir[1]^2 + dir[2]^2;
  //  b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
  //  c = (point[0] - origin[0])^2 + (point[1] - origin[1])^2 + (point[2] - origin[2])^2 - radius^2;

  // normalize dir so a = 1
  VectorNormalize(dir);
  b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
  c = (point[0] - origin[0]) * (point[0] - origin[0]) +
    (point[1] - origin[1]) * (point[1] - origin[1]) +
    (point[2] - origin[2]) * (point[2] - origin[2]) -
    radius * radius;

  d = b * b - 4 * c;
  if (d > 0) {
    t = (- b + sqrt(d)) / 2;
    VectorMA(point, t, dir, intersections[0]);
    t = (- b - sqrt(d)) / 2;
    VectorMA(point, t, dir, intersections[1]);
    return 2;
  }
  else if (d == 0) {
    t = (- b ) / 2;
    VectorMA(point, t, dir, intersections[0]);
    return 1;
  }
  return 0;
}


////////TA: locdamage

/*
===============
G_ParseArmourScript
===============
*/
void G_ParseArmourScript( char *buf, int upgrade )
{
	char	*token;
	int		count;

	count = 0;

	while( 1 )
  {
		token = COM_Parse( &buf );
    
		if( !token[0] )
			break;
      
		if( strcmp( token, "{" ) )
    {
			G_Printf( "Missing { in armour file\n" );
			break;
		}

		if( count == MAX_ARMOUR_REGIONS )
    {
			G_Printf( "Max armour regions exceeded in locdamage file\n" );
			break;
		}

    //default
    g_armourRegions[ upgrade ][ count ].minHeight = 0.0;
    g_armourRegions[ upgrade ][ count ].maxHeight = 1.0;
    g_armourRegions[ upgrade ][ count ].minAngle = 0;
    g_armourRegions[ upgrade ][ count ].maxAngle = 360;
    g_armourRegions[ upgrade ][ count ].modifier = 1.0;
    g_armourRegions[ upgrade ][ count ].crouch = qfalse;
    
    while( 1 )
    {
			token = COM_ParseExt( &buf, qtrue );
      
			if( !token[0] )
      {
				G_Printf( "Unexpected end of armour file\n" );
				break;
			}
      
			if( !Q_stricmp( token, "}" ) )
      {
				break;
      }
      else if( !strcmp( token, "minHeight" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "0" );

        g_armourRegions[ upgrade ][ count ].minHeight = atof( token );
      }
      else if( !strcmp( token, "maxHeight" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "100" );

        g_armourRegions[ upgrade ][ count ].maxHeight = atof( token );
      }
      else if( !strcmp( token, "minAngle" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "0" );

        g_armourRegions[ upgrade ][ count ].minAngle = atoi( token );
      }
      else if( !strcmp( token, "maxAngle" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "360" );

        g_armourRegions[ upgrade ][ count ].maxAngle = atoi( token );
      }
      else if( !strcmp( token, "modifier" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "1.0" );

        g_armourRegions[ upgrade ][ count ].modifier = atof( token );
      }
      else if( !strcmp( token, "crouch" ) )
      {
        g_armourRegions[ upgrade ][ count ].crouch = qtrue;
      }
		}

    g_numArmourRegions[ upgrade ]++;
    count++;
	}
}


/*
===============
G_ParseDmgScript
===============
*/
void G_ParseDmgScript( char *buf, int class )
{
	char	*token;
	int		count;

	count = 0;

	while( 1 )
  {
		token = COM_Parse( &buf );
    
		if( !token[0] )
			break;
      
		if( strcmp( token, "{" ) )
    {
			G_Printf( "Missing { in locdamage file\n" );
			break;
		}

		if( count == MAX_LOCDAMAGE_REGIONS )
    {
			G_Printf( "Max damage regions exceeded in locdamage file\n" );
			break;
		}

    //default
    g_damageRegions[ class ][ count ].minHeight = 0.0;
    g_damageRegions[ class ][ count ].maxHeight = 1.0;
    g_damageRegions[ class ][ count ].minAngle = 0;
    g_damageRegions[ class ][ count ].maxAngle = 360;
    g_damageRegions[ class ][ count ].modifier = 1.0;
    g_damageRegions[ class ][ count ].crouch = qfalse;
    
    while( 1 )
    {
			token = COM_ParseExt( &buf, qtrue );
      
			if( !token[0] )
      {
				G_Printf( "Unexpected end of locdamage file\n" );
				break;
			}
      
			if( !Q_stricmp( token, "}" ) )
      {
				break;
      }
      else if( !strcmp( token, "minHeight" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "0" );

        g_damageRegions[ class ][ count ].minHeight = atof( token );
      }
      else if( !strcmp( token, "maxHeight" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "100" );

        g_damageRegions[ class ][ count ].maxHeight = atof( token );
      }
      else if( !strcmp( token, "minAngle" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "0" );

        g_damageRegions[ class ][ count ].minAngle = atoi( token );
      }
      else if( !strcmp( token, "maxAngle" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "360" );

        g_damageRegions[ class ][ count ].maxAngle = atoi( token );
      }
      else if( !strcmp( token, "modifier" ) )
      {
			  token = COM_ParseExt( &buf, qfalse );

			  if ( !token[0] )
				  strcpy( token, "1.0" );

        g_damageRegions[ class ][ count ].modifier = atof( token );
      }
      else if( !strcmp( token, "crouch" ) )
      {
        g_damageRegions[ class ][ count ].crouch = qtrue;
      }
		}

    g_numDamageRegions[ class ]++;
    count++;
	}
}


/* 
============
G_CalcDamageModifier
============
*/
float G_CalcDamageModifier( vec3_t point, gentity_t *targ, gentity_t *attacker, int class )
{
  vec3_t  bulletPath;
  vec3_t  bulletAngle;
  vec3_t  pMINUSfloor, floor, normal;

  float   clientHeight, hitRelative, hitRatio;
  int     bulletRotation, clientRotation, hitRotation;
  float   modifier = 1.0;
  int     i, j;

  clientHeight = targ->r.maxs[ 2 ] - targ->r.mins[ 2 ];  

  if( targ->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING )
    VectorCopy( targ->client->ps.grapplePoint, normal );
  else
    VectorSet( normal, 0, 0, 1 );

  VectorMA( targ->r.currentOrigin, targ->r.mins[ 2 ], normal, floor );
  VectorSubtract( point, floor, pMINUSfloor );
  
  hitRelative = DotProduct( normal, pMINUSfloor ) / VectorLength( normal );

  if( hitRelative < 0.0 ) hitRelative = 0.0;
  if( hitRelative > clientHeight ) hitRelative = clientHeight;

  hitRatio = hitRelative / clientHeight;
                                
  VectorSubtract( targ->r.currentOrigin, point, bulletPath ); 
  vectoangles( bulletPath, bulletAngle );

  clientRotation = targ->client->ps.viewangles[ YAW ];
  bulletRotation = bulletAngle[ YAW ];

  hitRotation = abs( clientRotation - bulletRotation );
  
  hitRotation = hitRotation % 360; // Keep it in the 0-359 range

  for( i = 0; i < g_numDamageRegions[ class ]; i++ )
  {
    qboolean rotationBound;
    
    if( g_damageRegions[ class ][ i ].minAngle >
        g_damageRegions[ class ][ i ].maxAngle )
    {
      rotationBound = ( hitRotation > g_damageRegions[ class ][ i ].minAngle &&
                        hitRotation < 360 ) || ( hitRotation >= 0 &&
                        hitRotation <= g_damageRegions[ class ][ i ].maxAngle );
    }
    else
    {
      rotationBound = ( hitRotation > g_damageRegions[ class ][ i ].minAngle &&
                        hitRotation <= g_damageRegions[ class ][ i ].maxAngle );
    }
    
    if( rotationBound &&
        hitRatio > g_damageRegions[ class ][ i ].minHeight &&
        hitRatio <= g_damageRegions[ class ][ i ].maxHeight &&
        ( g_damageRegions[ class ][ i ].crouch ==
          ( targ->client->ps.pm_flags & PMF_DUCKED ) ) )
      modifier *= g_damageRegions[ class ][ i ].modifier;
  }

  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_gotItem( i, targ->client->ps.stats ) )
    {
      for( j = 0; j < g_numArmourRegions[ i ]; j++ )
      {
        qboolean rotationBound;
        
        if( g_damageRegions[ class ][ i ].minAngle >
            g_damageRegions[ class ][ i ].maxAngle )
        {
          rotationBound = ( hitRotation > g_damageRegions[ class ][ i ].minAngle &&
                            hitRotation < 360 ) || ( hitRotation >= 0 &&
                            hitRotation <= g_damageRegions[ class ][ i ].maxAngle );
        }
        else
        {
          rotationBound = ( hitRotation > g_damageRegions[ class ][ i ].minAngle &&
                            hitRotation <= g_damageRegions[ class ][ i ].maxAngle );
        }
        
        if( rotationBound &&
            hitRatio > g_armourRegions[ i ][ j ].minHeight &&
            hitRatio <= g_armourRegions[ i ][ j ].maxHeight &&
            ( g_armourRegions[ i ][ j ].crouch ==
              ( targ->client->ps.pm_flags & PMF_DUCKED ) ) )
          modifier *= g_armourRegions[ i ][ j ].modifier;
      }
    }
  }
  
  return modifier;
}


/*
============
G_InitDamageLocations
============
*/
void G_InitDamageLocations( )
{
  char          *modelName;
  char          filename[ MAX_QPATH ];
  int           i;
	int				    len;
	fileHandle_t	fileHandle;
	char			    buffer[ MAX_LOCDAMAGE_TEXT ];

  for( i = PCL_NONE + 1; i < PCL_NUM_CLASSES; i++ )
  {
    modelName = BG_FindModelNameForClass( i );
    Com_sprintf( filename, sizeof( filename ), "models/players/%s/locdamage.cfg", modelName );

    len = trap_FS_FOpenFile( filename, &fileHandle, FS_READ );
    if ( !fileHandle )
    {
      G_Printf( va( S_COLOR_RED "file not found: %s\n", filename ) );
      continue;
    }
    
    if ( len >= MAX_LOCDAMAGE_TEXT )
    {
      G_Printf( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_LOCDAMAGE_TEXT ) );
      trap_FS_FCloseFile( fileHandle );
      continue;
    }

    trap_FS_Read( buffer, len, fileHandle );
    buffer[len] = 0;
    trap_FS_FCloseFile( fileHandle );
      
    G_ParseDmgScript( buffer, i );
  }
  
  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    modelName = BG_FindNameForUpgrade( i );
    Com_sprintf( filename, sizeof( filename ), "armour/%s.armour", modelName );

    len = trap_FS_FOpenFile( filename, &fileHandle, FS_READ );
    
    //no file - no parsage
    if ( !fileHandle )
      continue;
    
    if ( len >= MAX_LOCDAMAGE_TEXT )
    {
      G_Printf( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_LOCDAMAGE_TEXT ) );
      trap_FS_FCloseFile( fileHandle );
      continue;
    }

    trap_FS_Read( buffer, len, fileHandle );
    buffer[len] = 0;
    trap_FS_FCloseFile( fileHandle );
      
    G_ParseArmourScript( buffer, i );
  }
}

////////TA: locdamage


/*
============
T_Damage

targ    entity that is being damaged
inflictor entity that is causing the damage
attacker  entity that caused the inflictor to damage targ
  example: targ=monster, inflictor=rocket, attacker=player

dir     direction of the attack for knockback
point   point at which the damage is being inflicted, used for headshots
damage    amount of damage being inflicted
knockback force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags    these flags are used to control how T_Damage works
  DAMAGE_RADIUS     damage was indirect (from a nearby explosion)
  DAMAGE_NO_ARMOR     armor does not protect from this damage
  DAMAGE_NO_KNOCKBACK   do not affect velocity, just view angles
  DAMAGE_NO_PROTECTION  kills godmode, armor, everything
============
*/

//TA: team is the team that is immune to this damage
void G_SelectiveDamage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
         vec3_t dir, vec3_t point, int damage, int dflags, int mod, int team )
{
  if( targ->client && ( team != targ->client->pers.pteam ) )
    G_Damage( targ, inflictor, attacker, dir, point, damage, dflags, mod );
}

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
         vec3_t dir, vec3_t point, int damage, int dflags, int mod )
{
  gclient_t *client;
  int     take;
  int     save;
  int     asave;
  int     knockback;
  int     max;

  if(!targ->takedamage)
    return;

  // the intermission has allready been qualified for, so don't
  // allow any extra scoring
  if ( level.intermissionQueued )
    return;

  if( !inflictor )
    inflictor = &g_entities[ENTITYNUM_WORLD];
    
  if( !attacker )
    attacker = &g_entities[ENTITYNUM_WORLD];

  // shootable doors / buttons don't actually have any health
  if( targ->s.eType == ET_MOVER )
  {
    if( targ->use && targ->moverState == MOVER_POS1 )
      targ->use( targ, inflictor, attacker );
      
    return;
  }

  client = targ->client;

  if( client ) 
  {
    if( client->noclip )
      return;
  }

  if( !dir )
    dflags |= DAMAGE_NO_KNOCKBACK;
  else
    VectorNormalize(dir);

  knockback = damage;
  if( knockback > 200 )
    knockback = 200;
    
  if( targ->flags & FL_NO_KNOCKBACK )
    knockback = 0;

  if( dflags & DAMAGE_NO_KNOCKBACK )
    knockback = 0;

  // figure momentum add, even if the damage won't be taken
  if( knockback && targ->client )
  {
    vec3_t  kvel;
    float mass;

    mass = 200;

    VectorScale( dir, g_knockback.value * (float)knockback / mass, kvel );
    VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );

    // set the timer so that the other client can't cancel
    // out the movement immediately
    if( !targ->client->ps.pm_time )
    {
      int   t;

      t = knockback * 2;
      if( t < 50 )
        t = 50;
        
      if( t > 200 )
        t = 200;
        
      targ->client->ps.pm_time = t;
      targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
    }
  }

  // check for completely getting out of the damage
  if( !(dflags & DAMAGE_NO_PROTECTION) )
  {

    // if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
    // if the attacker was on the same team
    if( targ != attacker && OnSameTeam (targ, attacker)  )
    {
      if( !g_friendlyFire.integer )
        return;
    }

    // check for godmode
    if ( targ->flags & FL_GODMODE )
      return;
  }

  // battlesuit protects from all radius damage (but takes knockback)
  // and protects 50% against all damage
  /*if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
    G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
    if ( ( dflags & DAMAGE_RADIUS ) || ( mod == MOD_FALLING ) ) {
      return;
    }
    damage *= 0.5;
  }*/

  // add to the attacker's hit counter
  if ( attacker->client && targ != attacker && targ->health > 0
      && targ->s.eType != ET_MISSILE
      && targ->s.eType != ET_GENERAL)
  {
    if( OnSameTeam( targ, attacker ) )
      attacker->client->ps.persistant[ PERS_HITS ]--;
    else
      attacker->client->ps.persistant[ PERS_HITS ]++;
  }

  // always give half damage if hurting self
  // calculated after knockback, so rocket jumping works
  if( targ == attacker)
    damage *= 0.5;

  if( damage < 1 )
    damage = 1;
    
  take = damage;
  save = 0;

  // save some from armor
/*  asave = CheckArmor (targ, take, dflags);
  take -= asave;*/
  //TA: armour is the chance of deflecting an attack (out of 100)
/*  if( targ->client && targ->client->ps.stats[ STAT_ARMOR ] > 0 )
  {
    //TA: this whole thing is probably a bad idea. Worth a try I guess.
    float chance = (float)targ->client->ps.stats[ STAT_ARMOR ] / 100.0f;

    if( crandom( ) > chance )
      take *= chance;
  }*/

  if( g_debugDamage.integer )
  {
    G_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number,
      targ->health, take, asave );
  }

  // add to the damage inflicted on a player this frame
  // the total will be turned into screen blends and view angle kicks
  // at the end of the frame
  if( client )
  {
    if( attacker )
      client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
    else
      client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
      
    client->damage_armor += asave;
    client->damage_blood += take;
    client->damage_knockback += knockback;
    if( dir )
    {
      VectorCopy ( dir, client->damage_from );
      client->damage_fromWorld = qfalse;
    }
    else
    {
      VectorCopy ( targ->r.currentOrigin, client->damage_from );
      client->damage_fromWorld = qtrue;
    }
  }

  // See if it's the player hurting the emeny flag carrier
  if( g_gametype.integer == GT_CTF)
    Team_CheckHurtCarrier(targ, attacker);

  if(targ->client)
  {
    // set the last client who damaged the target
    targ->client->lasthurt_client = attacker->s.number;
    targ->client->lasthurt_mod = mod;
    take = (int)( (float)take * G_CalcDamageModifier( point, targ, attacker, client->ps.stats[ STAT_PCLASS ] ) );
  }

  // do the damage
  if(take)
  {
    targ->health = targ->health - take;
    if( targ->client )
      targ->client->ps.stats[STAT_HEALTH] = targ->health;

    //TA: add to the attackers "account" on the target
    if( targ->client && attacker->client &&
        targ->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS &&
        attacker->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
      targ->credits[ attacker->client->ps.clientNum ] += take;

    if( targ->health <= 0 )
    {
      if( client )
        targ->flags |= FL_NO_KNOCKBACK;

      if (targ->health < -999)
        targ->health = -999;

      targ->enemy = attacker;
      targ->die (targ, inflictor, attacker, take, mod);
      return;
    }
    else if( targ->pain )
      targ->pain (targ, attacker, take);
  }
}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
  vec3_t  dest;
  trace_t tr;
  vec3_t  midpoint;

  // use the midpoint of the bounds instead of the origin, because
  // bmodels may have their origin is 0,0,0
  VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
  VectorScale (midpoint, 0.5, midpoint);

  VectorCopy (midpoint, dest);
  trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
  if (tr.fraction == 1.0  || tr.entityNum == targ->s.number)
    return qtrue;

  // this should probably check in the plane of projection,
  // rather than in world coordinate, and also include Z
  VectorCopy (midpoint, dest);
  dest[0] += 15.0;
  dest[1] += 15.0;
  trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
  if (tr.fraction == 1.0)
    return qtrue;

  VectorCopy (midpoint, dest);
  dest[0] += 15.0;
  dest[1] -= 15.0;
  trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
  if (tr.fraction == 1.0)
    return qtrue;

  VectorCopy (midpoint, dest);
  dest[0] -= 15.0;
  dest[1] += 15.0;
  trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
  if (tr.fraction == 1.0)
    return qtrue;

  VectorCopy (midpoint, dest);
  dest[0] -= 15.0;
  dest[1] -= 15.0;
  trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
  if (tr.fraction == 1.0)
    return qtrue;


  return qfalse;
}


//TA:
/*
============
G_RadiusDamage
============
*/
qboolean G_SelectiveRadiusDamage ( vec3_t origin, gentity_t *attacker, float damage, float radius,
           gentity_t *ignore, int mod, int team ) {
  float   points, dist;
  gentity_t *ent;
  int     entityList[MAX_GENTITIES];
  int     numListedEntities;
  vec3_t    mins, maxs;
  vec3_t    v;
  vec3_t    dir;
  int     i, e;
  qboolean  hitClient = qfalse;

  if ( radius < 1 ) {
    radius = 1;
  }

  for ( i = 0 ; i < 3 ; i++ ) {
    mins[i] = origin[i] - radius;
    maxs[i] = origin[i] + radius;
  }

  numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

  for ( e = 0 ; e < numListedEntities ; e++ ) {
    ent = &g_entities[entityList[ e ]];

    if (ent == ignore)
      continue;
    if (!ent->takedamage)
      continue;

    // find the distance from the edge of the bounding box
    for ( i = 0 ; i < 3 ; i++ ) {
      if ( origin[i] < ent->r.absmin[i] ) {
        v[i] = ent->r.absmin[i] - origin[i];
      } else if ( origin[i] > ent->r.absmax[i] ) {
        v[i] = origin[i] - ent->r.absmax[i];
      } else {
        v[i] = 0;
      }
    }

    dist = VectorLength( v );
    if ( dist >= radius ) {
      continue;
    }

    points = damage * ( 1.0 - dist / radius );

    if( CanDamage (ent, origin) ) {
      VectorSubtract (ent->r.currentOrigin, origin, dir);
      // push the center of mass higher than the origin so players
      // get knocked into the air more
      dir[2] += 24;
      G_SelectiveDamage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod, team );
    }
  }

  return hitClient;
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage ( vec3_t origin, gentity_t *attacker, float damage, float radius,
           gentity_t *ignore, int mod) {
  float   points, dist;
  gentity_t *ent;
  int     entityList[MAX_GENTITIES];
  int     numListedEntities;
  vec3_t    mins, maxs;
  vec3_t    v;
  vec3_t    dir;
  int     i, e;
  qboolean  hitClient = qfalse;

  if ( radius < 1 ) {
    radius = 1;
  }

  for ( i = 0 ; i < 3 ; i++ ) {
    mins[i] = origin[i] - radius;
    maxs[i] = origin[i] + radius;
  }

  numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

  for ( e = 0 ; e < numListedEntities ; e++ ) {
    ent = &g_entities[entityList[ e ]];

    if (ent == ignore)
      continue;
    if (!ent->takedamage)
      continue;

    // find the distance from the edge of the bounding box
    for ( i = 0 ; i < 3 ; i++ ) {
      if ( origin[i] < ent->r.absmin[i] ) {
        v[i] = ent->r.absmin[i] - origin[i];
      } else if ( origin[i] > ent->r.absmax[i] ) {
        v[i] = origin[i] - ent->r.absmax[i];
      } else {
        v[i] = 0;
      }
    }

    dist = VectorLength( v );
    if ( dist >= radius ) {
      continue;
    }

    points = damage * ( 1.0 - dist / radius );

    if( CanDamage (ent, origin) ) {
      VectorSubtract (ent->r.currentOrigin, origin, dir);
      // push the center of mass higher than the origin so players
      // get knocked into the air more
      dir[2] += 24;
      G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod);
    }
  }

  return hitClient;
}
