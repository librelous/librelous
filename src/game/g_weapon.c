// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_weapon.c
// perform the server side effects of a weapon firing

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

static  vec3_t  forward, right, up;
static  vec3_t  muzzle;

#define NUM_NAILSHOTS 15

/*
================
G_BounceProjectile
================
*/
void G_BounceProjectile( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout )
{
  vec3_t v, newv;
  float dot;

  VectorSubtract( impact, start, v );
  dot = DotProduct( v, dir );
  VectorMA( v, -2 * dot, dir, newv );

  VectorNormalize(newv);
  VectorMA(impact, 8192, newv, endout);
}

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
void SnapVectorTowards( vec3_t v, vec3_t to )
{
  int   i;

  for( i = 0 ; i < 3 ; i++ )
  {
    if( to[ i ] <= v[ i ] )
      v[ i ] = (int)v[ i ];
    else
      v[ i ] = (int)v[ i ] + 1;
  }
}

/*
===============
meleeAttack
===============
*/
void meleeAttack( gentity_t *ent, float range, int damage )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  // set aiming directions
  AngleVectors (ent->client->ps.viewangles, forward, right, up);

  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  VectorMA( muzzle, range, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if ( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // send blood impact
  if ( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
    tent->s.otherEntityNum = traceEnt->s.number;
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
  }

  if ( traceEnt->takedamage )
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, MOD_VENOM );
}

/*
======================================================================

MACHINEGUN

======================================================================
*/

void bulletFire( gentity_t *ent, float spread, int damage, int mod )
{
  trace_t   tr;
  vec3_t    end;
  float   r;
  float   u;
  gentity_t *tent;
  gentity_t *traceEnt;

  r = random( ) * M_PI * 2.0f;
  u = sin( r ) * crandom( ) * spread * 16;
  r = cos( r ) * crandom( ) * spread * 16;
  VectorMA( muzzle, 8192 * 16, forward, end );
  VectorMA( end, r, right, end );
  VectorMA( end, u, up, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send bullet impact
  if( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
    tent->s.eventParm = traceEnt->s.number;
  }
  else
  {
    tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );
    tent->s.eventParm = DirToByte( tr.plane.normal );
  }
  tent->s.otherEntityNum = ent->s.number;

  if( traceEnt->takedamage )
  {
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
      damage, 0, MOD_MACHINEGUN );
  }
}

/*
======================================================================

MASS DRIVER

======================================================================
*/

void massDriverFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  VectorMA( muzzle, 8192 * 16, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send impact
  tent = G_TempEntity( tr.endpos, EV_MASS_DRIVER_HIT );
  tent->s.eventParm = DirToByte( tr.plane.normal );

  if( traceEnt->takedamage )
  {
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
      MDRIVER_DMG, 0, MOD_MACHINEGUN );
  }
}

/*
======================================================================

LOCKBLOB

======================================================================
*/

void lockBlobLauncherFire( gentity_t *ent )
{
  gentity_t *m;

  m = fire_lockblob( ent, muzzle, forward );

//  VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );  // "real" physics
}

/*
======================================================================

PULSE RIFLE

======================================================================
*/

void pulseRifleFire( gentity_t *ent )
{
  gentity_t *m;

  m = fire_pulseRifle( ent, muzzle, forward );

//  VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );  // "real" physics
}

/*
======================================================================

FLAME THROWER

======================================================================
*/

void flamerFire( gentity_t *ent )
{
  gentity_t *m;

  m = fire_flamer( ent, muzzle, forward );
}

/*
======================================================================

LAS GUN

======================================================================
*/

/*
===============
lasGunFire
===============
*/
void lasGunFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  VectorMA( muzzle, 8192 * 16, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // snap the endpos to integers, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send bullet impact
  if( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_LAS_HIT_FLESH );
    tent->s.eventParm = traceEnt->s.number;
  }
  else
  {
    tent = G_TempEntity( tr.endpos, EV_LAS_HIT_WALL );
    tent->s.eventParm = DirToByte( tr.plane.normal );
  }
  tent->s.otherEntityNum = ent->s.number;

  if( traceEnt->takedamage )
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, LASGUN_DAMAGE, 0, MOD_MACHINEGUN );
}

/*
======================================================================

PAIN SAW

======================================================================
*/

void painSawFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;

  // set aiming directions
  AngleVectors( ent->client->ps.viewangles, forward, right, up );

  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  VectorMA( muzzle, PAINSAW_RANGE, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if ( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  // send blood impact
  if ( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
    tent->s.otherEntityNum = traceEnt->s.number;
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
  }

  if ( traceEnt->takedamage )
    G_Damage( traceEnt, ent, ent, forward, tr.endpos, PAINSAW_DAMAGE, DAMAGE_NO_KNOCKBACK, MOD_VENOM );
}

/*
======================================================================

LUCIFER CANON

======================================================================
*/

/*
===============
LCChargeFire
===============
*/
void LCChargeFire( gentity_t *ent, qboolean secondary )
{
  gentity_t *m;

  if( secondary )
    ent->client->ps.stats[ STAT_MISC ] = LCANON_SECONDARY_DAMAGE;

  m = fire_luciferCanon( ent, muzzle, forward, ent->client->ps.stats[ STAT_MISC ] );
  
  ent->client->ps.stats[ STAT_MISC ] = 0;
}

/*
======================================================================

TESLA GENERATOR

======================================================================
*/


void teslaFire( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *traceEnt, *tent;
  int     damage, i, passent;

  damage = 8;

  VectorMA( muzzle, TESLAGEN_RANGE, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

  if( tr.entityNum == ENTITYNUM_NONE )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  if( traceEnt->takedamage)
  {
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
      damage, 0, MOD_LIGHTNING);
  }

  // snap the endpos to integers to save net bandwidth, but nudged towards the line
  SnapVectorTowards( tr.endpos, muzzle );

  // send railgun beam effect
  tent = G_TempEntity( tr.endpos, EV_TESLATRAIL );

  VectorCopy( muzzle, tent->s.origin2 );
  
  tent->s.generic1 = ent->s.number; //src
  tent->s.clientNum = traceEnt->s.number; //dest
  
  // move origin a bit to come closer to the drawn gun muzzle
  VectorMA( tent->s.origin2, 28, up, tent->s.origin2 );

  // no explosion at end if SURF_NOIMPACT, but still make the trail
  if( tr.surfaceFlags & SURF_NOIMPACT )
    tent->s.eventParm = 255;  // don't make the explosion at the end
  else
    tent->s.eventParm = DirToByte( tr.plane.normal );
}


/*
======================================================================

BUILD GUN

======================================================================
*/

void cancelBuildFire( gentity_t *ent )
{
  vec3_t      forward, end;
  trace_t     tr;
  gentity_t   *traceEnt;
  int         bHealth;

  if( ent->client->ps.stats[ STAT_BUILDABLE ] != BA_NONE )
  {
    ent->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    return;
  }
  
  //repair buildable
  if( ent->client->pers.pteam == PTE_HUMANS )
  {
    AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
    VectorMA( ent->client->ps.origin, 100, forward, end );

    trap_Trace( &tr, ent->client->ps.origin, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
    traceEnt = &g_entities[ tr.entityNum ];

    if( tr.fraction < 1.0 &&
        ( traceEnt->s.eType == ET_BUILDABLE ) &&
        ( traceEnt->biteam == ent->client->pers.pteam ) &&
        ( ( ent->client->ps.weapon >= WP_HBUILD2 ) &&
          ( ent->client->ps.weapon <= WP_HBUILD ) ) )
    {
      if( ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_AddPredictableEvent( ent, EV_BUILD_DELAY, 0 );
        return;
      }

      bHealth = BG_FindHealthForBuildable( traceEnt->s.modelindex );

      traceEnt->health += ( bHealth / 10.0f );

      if( traceEnt->health > bHealth )
        traceEnt->health = bHealth;
    }
  }
  else if( ent->client->ps.weapon == WP_ABUILD2 )
    meleeAttack( ent, ABUILDER_CLAW_RANGE, ABUILDER_CLAW_DMG ); //melee attack for alien builder
}

void buildFire( gentity_t *ent, dynMenu_t menu )
{
  if( ( ent->client->ps.stats[ STAT_BUILDABLE ] & ~SB_VALID_TOGGLEBIT ) > BA_NONE )
  {
    if( ent->client->ps.stats[ STAT_MISC ] > 0 )
    {
      G_AddPredictableEvent( ent, EV_BUILD_DELAY, 0 );
      return;
    }
    
    G_ValidateBuild( ent, ent->client->ps.stats[ STAT_BUILDABLE ] & ~SB_VALID_TOGGLEBIT );
    ent->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    ent->client->ps.stats[ STAT_MISC ] += BG_FindBuildDelayForWeapon( ent->s.weapon );
    return;
  }

  G_AddPredictableEvent( ent, EV_MENU, menu );
}


/*
======================================================================

VENOM

======================================================================
*/

/*
===============
CheckVenomAttack
===============
*/
qboolean CheckVenomAttack( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;
  int     damage;

  // set aiming directions
  AngleVectors (ent->client->ps.viewangles, forward, right, up);

  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  VectorMA( muzzle, SOLDIER_BITE_RANGE, forward, end );

  trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);
  if ( tr.surfaceFlags & SURF_NOIMPACT )
    return qfalse;

  traceEnt = &g_entities[ tr.entityNum ];

  if( !traceEnt->takedamage)
    return qfalse;
  if( !traceEnt->client )
    return qfalse;
  if( traceEnt->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
    return qfalse;

  // send blood impact
  if ( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
    tent->s.otherEntityNum = traceEnt->s.number;
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
  }

  G_Damage( traceEnt, ent, ent, forward, tr.endpos, SOLDIER_BITE_DMG, DAMAGE_NO_KNOCKBACK, MOD_VENOM );

  return qtrue;
}

/*
======================================================================

GRAB AND CLAW

======================================================================
*/

/*
===============
CheckGrabAttack
===============
*/
void CheckGrabAttack( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *traceEnt;

  // set aiming directions
  AngleVectors( ent->client->ps.viewangles, forward, right, up );

  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  VectorMA( muzzle, HYDRA_GRAB_RANGE, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
  if( tr.surfaceFlags & SURF_NOIMPACT )
    return;

  traceEnt = &g_entities[ tr.entityNum ];

  if( !traceEnt->takedamage )
    return;
  if( !traceEnt->client )
    return;
  if( traceEnt->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
    return;
    
  if( !( traceEnt->client->ps.stats[ STAT_STATE ] & SS_GRABBED ) )
    VectorCopy( traceEnt->client->ps.viewangles, traceEnt->client->ps.grapplePoint );
  
  traceEnt->client->ps.stats[ STAT_STATE ] |= SS_GRABBED;
  traceEnt->client->lastGrabTime = level.time;

  //FIXME: event for some client side grab effect?
}

/*
===============
poisonCloud
===============
*/
void poisonCloud( gentity_t *ent )
{
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { HYDRA_PCLOUD_RANGE, HYDRA_PCLOUD_RANGE, HYDRA_PCLOUD_RANGE };
  vec3_t    mins, maxs, dir;
  int       i, num;
  gentity_t *humanPlayer;
  
  VectorAdd( ent->client->ps.origin, range, maxs );
  VectorSubtract( ent->client->ps.origin, range, mins );
  
  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
  {
    humanPlayer = &g_entities[ entityList[ i ] ];
    
    if( humanPlayer->client && humanPlayer->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    {
      if( !( humanPlayer->client->ps.stats[ STAT_STATE ] & SS_POISONCLOUDED ) )
      {
        humanPlayer->client->ps.stats[ STAT_STATE ] |= SS_POISONCLOUDED;
        humanPlayer->client->lastPoisonCloudedTime = level.time;
        G_AddPredictableEvent( humanPlayer, EV_POISONCLOUD, 0 );
      }
    }
  }
}

/*
======================================================================

CLAW AND POUNCE

======================================================================
*/

/*
===============
CheckPounceAttack
===============
*/
qboolean CheckPounceAttack( gentity_t *ent )
{
  trace_t   tr;
  vec3_t    end;
  gentity_t *tent;
  gentity_t *traceEnt;
  int     damage;

  if( !ent->client->allowedToPounce )
    return qfalse;

  if( ent->client->ps.groundEntityNum != ENTITYNUM_NONE )
    return qfalse;

  // set aiming directions
  AngleVectors( ent->client->ps.viewangles, forward, right, up );

  CalcMuzzlePoint( ent, forward, right, up, muzzle );

  VectorMA( muzzle, DRAGOON_POUNCE_RANGE, forward, end );

  trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

  //miss
  if( tr.fraction >= 1.0 )
    return qfalse;
    
  if( tr.surfaceFlags & SURF_NOIMPACT )
    return qfalse;

  traceEnt = &g_entities[ tr.entityNum ];

  // send blood impact
  if( traceEnt->takedamage && traceEnt->client )
  {
    tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
    tent->s.otherEntityNum = traceEnt->s.number;
    tent->s.eventParm = DirToByte( tr.plane.normal );
    tent->s.weapon = ent->s.weapon;
  }

  if( !traceEnt->takedamage )
    return qfalse;

  damage = (int)( ( (float)ent->client->pouncePayload / (float)DRAGOON_POUNCE_SPEED ) * DRAGOON_POUNCE_DMG );

  G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK, MOD_VENOM );

  ent->client->allowedToPounce = qfalse;
  
  return qtrue;
}

void slowBlobFire( gentity_t *ent )
{
  gentity_t *m;

  m = fire_slowBlob( ent, muzzle, forward );

//  VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );  // "real" physics
}

/*
======================================================================

ZAP

======================================================================
*/

/*
===============
areaZapFire
===============
*/
void areaZapFire( gentity_t *ent )
{
  int       entityList[ MAX_GENTITIES ];
  int       targetList[ MAX_GENTITIES ];
  vec3_t    range = { CHIMERA_AREAZAP_RANGE, CHIMERA_AREAZAP_RANGE, CHIMERA_AREAZAP_RANGE };
  vec3_t    mins, maxs, dir;
  int       i, num, numTargets = 0;
  gentity_t *enemy;
  gentity_t *tent;
  trace_t   tr;
  int       damage;

  VectorAdd( muzzle, range, maxs );
  VectorSubtract( muzzle, range, mins );
  
  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
  {
    enemy = &g_entities[ entityList[ i ] ];
    
    if( ( enemy->client && enemy->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS ) ||
        ( enemy->s.eType == ET_BUILDABLE && BG_FindTeamForBuildable( enemy->s.modelindex ) == BIT_HUMANS ) )
    {
      trap_Trace( &tr, muzzle, NULL, NULL, enemy->s.origin, ent->s.number, MASK_SHOT );
    
      //can't see target from here
      if( tr.entityNum == ENTITYNUM_WORLD )
        continue;

      targetList[ numTargets++ ] = entityList[ i ];
    }
  }

  damage = (int)( (float)CHIMERA_AREAZAP_DMG / (float)numTargets );
  for( i = 0; i < numTargets; i++ )
  {
    enemy = &g_entities[ targetList[ i ] ];

    VectorSubtract( enemy->s.origin, muzzle, dir );
    VectorNormalize( dir );
    
    //do some damage
    G_Damage( enemy, ent, ent, dir, tr.endpos,
              damage, DAMAGE_NO_KNOCKBACK, MOD_LIGHTNING );
    
    // snap the endpos to integers to save net bandwidth, but nudged towards the line
    SnapVectorTowards( tr.endpos, muzzle );

    // send railgun beam effect
    tent = G_TempEntity( enemy->s.pos.trBase, EV_ALIENZAP );

    VectorCopy( muzzle, tent->s.origin2 );

    tent->s.generic1 = ent->s.number; //src
    tent->s.clientNum = enemy->s.number; //dest
  }
}

/*
===============
directZapFire
===============
*/
void directZapFire( gentity_t *ent )
{
  int       entityList[ MAX_GENTITIES ];
  int       targetList[ MAX_GENTITIES ];
  vec3_t    range = { CHIMERA_DIRECTZAP_RANGE, CHIMERA_DIRECTZAP_RANGE, CHIMERA_DIRECTZAP_RANGE };
  vec3_t    mins, maxs, dir;
  int       i, num, numTargets = 0;
  gentity_t *enemy;
  vec3_t    end;
  gentity_t *target = NULL, *tent;
  float     distance, minDist = 10000.0f;
  trace_t   tr;

  VectorAdd( muzzle, range, maxs );
  VectorSubtract( muzzle, range, mins );
  
  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
  {
    enemy = &g_entities[ entityList[ i ] ];
    
    if( ( enemy->client && enemy->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS ) ||
        ( enemy->s.eType == ET_BUILDABLE && BG_FindTeamForBuildable( enemy->s.modelindex ) == BIT_HUMANS ) )
    {
      trap_Trace( &tr, muzzle, NULL, NULL, enemy->s.origin, ent->s.number, MASK_SHOT );
    
      //can't see target from here
      if( tr.entityNum == ENTITYNUM_WORLD )
        continue;

      targetList[ numTargets++ ] = entityList[ i ];
    }
  }
  
  VectorAdd( muzzle, forward, end );

  for( i = 0; i < numTargets; i++ )
  {
    enemy = &g_entities[ targetList[ i ] ];

    distance = pointToLineDistance( enemy->s.origin, muzzle, end );
    if( distance < minDist )
    {
      target = enemy;
      minDist = distance;
    }
  }
  
  if( target != NULL )
  {
    //do some damage
    G_Damage( target, ent, ent, dir, tr.endpos,
              CHIMERA_DIRECTZAP_DMG, DAMAGE_NO_KNOCKBACK, MOD_LIGHTNING );
    
    // snap the endpos to integers to save net bandwidth, but nudged towards the line
    SnapVectorTowards( tr.endpos, muzzle );

    // send railgun beam effect
    tent = G_TempEntity( target->s.pos.trBase, EV_ALIENZAP );

    VectorCopy( muzzle, tent->s.origin2 );

    tent->s.generic1 = ent->s.number; //src
    tent->s.clientNum = target->s.number; //dest
  }
}


/*
======================================================================

GROUND POUND

======================================================================
*/

/*
===============
groundPound
===============
*/
void groundPound( gentity_t *ent )
{
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { BMOFO_KNOCK_RANGE, BMOFO_KNOCK_RANGE, BMOFO_KNOCK_RANGE };
  vec3_t    mins, maxs, dir;
  int       i, num;
  gentity_t *humanPlayer;
  
  VectorAdd( ent->client->ps.origin, range, maxs );
  VectorSubtract( ent->client->ps.origin, range, mins );
  
  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
  {
    humanPlayer = &g_entities[ entityList[ i ] ];
    
    if( humanPlayer->client && humanPlayer->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    {
      if( !( humanPlayer->client->ps.stats[ STAT_STATE ] & SS_KNOCKEDOVER ) )
      {
        humanPlayer->client->ps.stats[ STAT_STATE ] |= SS_KNOCKEDOVER;
        humanPlayer->client->lastKnockedOverTime = level.time;
        G_AddPredictableEvent( humanPlayer, EV_KNOCKOVER, 0 );
        
        VectorCopy( humanPlayer->client->ps.viewangles, humanPlayer->client->ps.grapplePoint );
        
        //FIXME: fallover anim
        humanPlayer->client->ps.legsAnim =
          ( ( humanPlayer->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_DEATH1;
        humanPlayer->client->ps.torsoAnim =
          ( ( humanPlayer->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_DEATH1;
      }
    }
  }
}

//======================================================================

/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint )
{
  VectorCopy( ent->s.pos.trBase, muzzlePoint );
  muzzlePoint[ 2 ] += ent->client->ps.viewheight;
  VectorMA( muzzlePoint, 1, forward, muzzlePoint );
  VectorMA( muzzlePoint, 1, right, muzzlePoint );
  // snap to integer coordinates for more efficient network bandwidth usage
  SnapVector( muzzlePoint );
}

/*
===============
FireWeapon3
===============
*/
void FireWeapon3( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );
  }
  else
  {
    AngleVectors( ent->s.angles2, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
    case WP_POUNCE_UPG:
      slowBlobFire( ent );
      break;
    case WP_DIRECT_ZAP:
      areaZapFire( ent );
      break;
    case WP_GROUND_POUND:
      slowBlobFire( ent );
      break;
      
    default:
      break;
  }
}

/*
===============
FireWeapon2
===============
*/
void FireWeapon2( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );
  }
  else
  {
    AngleVectors( ent->s.angles2, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
    case WP_GRAB_CLAW_UPG:
      poisonCloud( ent );
      break;
    case WP_AREA_ZAP:
      areaZapFire( ent );
      break;
    case WP_DIRECT_ZAP:
      directZapFire( ent );
      break;
    case WP_GROUND_POUND:
      groundPound( ent );
      break;
      
    case WP_LUCIFER_CANON:
      LCChargeFire( ent, qtrue );
      break;
      
    case WP_ABUILD:
    case WP_ABUILD2:
    case WP_HBUILD:
    case WP_HBUILD2:
      cancelBuildFire( ent );
      break;
    default:
      break;
  }
}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent )
{
  if( ent->client )
  {
    // set aiming directions
    AngleVectors( ent->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );
  }
  else
  {
    AngleVectors( ent->turretAim, forward, right, up );
    VectorCopy( ent->s.pos.trBase, muzzle );
  }

  // fire the specific weapon
  switch( ent->s.weapon )
  {
    case WP_GRAB_CLAW:
    case WP_GRAB_CLAW_UPG:
      meleeAttack( ent, HYDRA_CLAW_RANGE, HYDRA_CLAW_DMG );
      break;
    case WP_POUNCE:
    case WP_POUNCE_UPG:
      meleeAttack( ent, DRAGOON_CLAW_RANGE, DRAGOON_CLAW_DMG );
      break;
    case WP_AREA_ZAP:
      meleeAttack( ent, CHIMERA_CLAW_RANGE, CHIMERA_CLAW_DMG );
      break;
    case WP_DIRECT_ZAP:
      meleeAttack( ent, CHIMERA_CLAW_RANGE, CHIMERA_CLAW_DMG );
      break;
    case WP_GROUND_POUND:
      meleeAttack( ent, BMOFO_CLAW_RANGE, BMOFO_CLAW_DMG );
      break;

    case WP_MACHINEGUN:
      bulletFire( ent, RIFLE_SPREAD, RIFLE_DMG, MOD_MACHINEGUN );
      break;
    case WP_CHAINGUN:
      bulletFire( ent, CHAINGUN_SPREAD, CHAINGUN_DMG, MOD_CHAINGUN );
      break;
    case WP_FLAMER:
      flamerFire( ent );
      break;
    case WP_PULSE_RIFLE:
      pulseRifleFire( ent );
      break;
    case WP_MASS_DRIVER:
      massDriverFire( ent );
      break;
    case WP_LUCIFER_CANON:
      LCChargeFire( ent, qfalse );
      break;
    case WP_LAS_GUN:
      lasGunFire( ent );
      break;
    case WP_PAIN_SAW:
      painSawFire( ent );
      break;
      
    case WP_LOCKBLOB_LAUNCHER:
      lockBlobLauncherFire( ent );
      break;
    case WP_TESLAGEN:
      teslaFire( ent );
      break;
      
    case WP_ABUILD:
      buildFire( ent, MN_A_BUILD );
      break;
    case WP_ABUILD2:
      buildFire( ent, MN_A_BUILD );
      break;
    case WP_HBUILD:
      buildFire( ent, MN_H_BUILD );
      break;
    case WP_HBUILD2:
      buildFire( ent, MN_H_BUILD );
      break;
    default:
      break;
  }
}

