/*
 *  Portions Copyright (C) 2000-2001 Tim Angus
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*  To assertain which portions are licensed under the LGPL and which are
 *  licensed by Id Software, Inc. please run a diff between the equivalent
 *  versions of the "Tremulous" modification and the unmodified "Quake3"
 *  game source code.
 */
                  
#include "g_local.h"

#define REFRESH_TIME  2000

/*
================
G_setBuildableAnim

Triggers an animation client side
================
*/
void G_setBuildableAnim( gentity_t *ent, buildableAnimNumber_t anim )
{
  G_AddEvent( ent, EV_BUILD_ANIM, anim );
}

/*
================
G_setIdleBuildableAnim

Set the animation to use whilst no other animations are running
================
*/
void G_setIdleBuildableAnim( gentity_t *ent, buildableAnimNumber_t anim )
{
  ent->s.torsoAnim = anim;
}

/*
================
findPower

attempt to find power for self, return qtrue if successful
================
*/
qboolean findPower( gentity_t *self )
{
  int       i;
  gentity_t *ent;
  gentity_t *closestPower;
  int       distance = 0;
  int       minDistance = 10000;
  vec3_t    temp_v;
  qboolean  foundPower = qfalse;

  //if this already has power then stop now
  if( self->parentNode && self->parentNode->active )
    return qtrue;
  
  //reset parent
  self->parentNode = NULL;
  
  //iterate through entities
  for ( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
  {
    if( !ent->classname )
      continue;

    //if entity is a power item calculate the distance to it
    if( !Q_stricmp( ent->classname, "team_human_reactor" ) ||
        !Q_stricmp( ent->classname, "team_human_repeater" ) )
    {
      VectorSubtract( self->s.origin, ent->s.origin, temp_v );
      distance = VectorLength( temp_v );
      if( distance < minDistance && ( ent->active || !Q_stricmp( self->classname, "team_human_spawn" ) ) )
      {
        closestPower = ent;
        minDistance = distance;
        foundPower = qtrue;
      }
    }
  }

  //if there were no power items nearby give up
  if( !foundPower )
    return qfalse;
  
  //bleh
  if( (
        !Q_stricmp( closestPower->classname, "team_human_reactor" ) &&
        ( minDistance <= REACTOR_BASESIZE )
      ) || 
      (
        !Q_stricmp( closestPower->classname, "team_human_repeater" ) &&
        !Q_stricmp( self->classname, "team_human_spawn" ) &&
        ( minDistance <= REPEATER_BASESIZE ) &&
        closestPower->powered 
      ) ||
      (
        !Q_stricmp( closestPower->classname, "team_human_repeater" ) &&
        ( minDistance <= REPEATER_BASESIZE ) &&
        closestPower->active &&
        closestPower->powered 
      )
    )
  {
    self->parentNode = closestPower;

    return qtrue;
  }
  else
    return qfalse;
}

/*
================
findCreep

attempt to find creep for self, return qtrue if successful
================
*/
qboolean findCreep( gentity_t *self )
{
  int       i;
  gentity_t *ent;
  gentity_t *closestSpawn;
  int       distance = 0;
  int       minDistance = 10000;
  vec3_t    temp_v;
  
  //if self does not have a parentNode or it's parentNode is invalid find a new one
  if( ( self->parentNode == NULL ) || !self->parentNode->inuse )
  {
    for ( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
    {
      if( !Q_stricmp( ent->classname, "team_droid_spawn" ) ||
          !Q_stricmp( ent->classname, "team_droid_hivemind" ) )
      {
        VectorSubtract( self->s.origin, ent->s.origin, temp_v );
        distance = VectorLength( temp_v );
        if( distance < minDistance )
        {
          closestSpawn = ent;
          minDistance = distance;
        }
      }
    }
    
    if( minDistance <= ( CREEP_BASESIZE * 3 ) )
    {
      self->parentNode = closestSpawn;
      return qtrue;
    }
    else
      return qfalse;
  }

  //if we haven't returned by now then we must already have a valid parent
  return qtrue;
}

/*
================
nullDieFunction

hack to prevent compilers complaining about function pointer -> NULL conversion
================
*/
static void nullDieFunction( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
}

/*
================
freeBuildable
================
*/
static void freeBuildable( gentity_t *self )
{
  G_FreeEntity( self );
}


//==================================================================================



/*
================
D_CreepRecede

Called when an droid spawn dies
================
*/
void D_CreepRecede( gentity_t *self )
{
  //if the creep just died begin the recession
  if( !( self->s.eFlags & EF_DEAD ) )
  {
    self->s.eFlags |= EF_DEAD;
    G_AddEvent( self, EV_BUILD_DESTROY, 0 );
  }
  
  //creep is still receeding
  if( ( self->timestamp + 10000 ) > level.time )
  {
    self->nextthink = level.time + 500;
    trap_LinkEntity( self );
  }
  else //creep has died
    G_FreeEntity( self );
}




//==================================================================================




/*
================
DSpawn_Melt

Called when an droid spawn dies
================
*/
void DSpawn_Melt( gentity_t *self )
{
  //FIXME: this line crashes the QVM (but not binary when MOD is set to MOD_[H/D]SPAWN
  G_SelectiveRadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, MOD_SHOTGUN, PTE_DROIDS );

  //start creep recession
  if( !( self->s.eFlags & EF_DEAD ) )
  {
    self->s.eFlags |= EF_DEAD;
    G_AddEvent( self, EV_BUILD_DESTROY, 0 );
  }
  
  //not dead yet
  if( ( self->timestamp + 10000 ) > level.time )
  {
    self->nextthink = level.time + 500;
    trap_LinkEntity( self );
  }
  else //dead now
    G_FreeEntity( self );
}

/*
================
DSpawn_Die

Called when an droid spawn dies
================
*/
void DSpawn_Die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
  vec3_t  dir;

  // we don't have a valid direction, so just point straight up
  dir[0] = dir[1] = 0;
  dir[2] = 1;

  //do a bit of radius damage
  G_SelectiveRadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, self->splashMethodOfDeath, PTE_DROIDS );

  //pretty events and item cleanup
  self->s.modelindex = 0; //don't draw the model once its destroyed
  G_AddEvent( self, EV_GIB_DROID, DirToByte( dir ) );
  self->r.contents = CONTENTS_TRIGGER;
  self->timestamp = level.time;
  self->die = nullDieFunction;
  self->think = DSpawn_Melt;
  self->nextthink = level.time + 500; //wait .5 seconds before damaging others
    
  trap_LinkEntity( self );
}

/*
================
DSpawn_Think

think function for Droid Spawn
================
*/
void DSpawn_Think( gentity_t *self )
{
}

/*
================
DSpawn_Pain

pain function for Droid Spawn
================
*/
void DSpawn_Pain( gentity_t *self, gentity_t *attacker, int damage )
{
  G_setBuildableAnim( self, BANIM_PAIN1 );
}



//==================================================================================



/*
================
DBarricade_Pain

pain function for Droid Spawn
================
*/
void DBarricade_Pain( gentity_t *self, gentity_t *attacker, int damage )
{
  if( random() > 0.5f )
    G_setBuildableAnim( self, BANIM_PAIN1 );
  else
    G_setBuildableAnim( self, BANIM_PAIN2 );
}

/*
================
DBarricade_Die

Called when an droid spawn dies
================
*/
void DBarricade_Die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
  vec3_t  dir;
  
  // we don't have a valid direction, so just point straight up
  dir[0] = dir[1] = 0;
  dir[2] = 1;

  //do a bit of radius damage
  G_SelectiveRadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, self->splashMethodOfDeath, PTE_DROIDS );

  //pretty events and item cleanup
  self->s.modelindex = 0; //don't draw the model once its destroyed
  G_AddEvent( self, EV_GIB_DROID, DirToByte( dir ) );
  self->r.contents = CONTENTS_TRIGGER;
  self->timestamp = level.time;
  self->die = nullDieFunction;
  self->think = D_CreepRecede;
  self->nextthink = level.time + 100;

  trap_LinkEntity( self );
}

/*
================
DBarricade_Think

think function for Droid Barricade
================
*/
void DBarricade_Think( gentity_t *self )
{
  //if there is no creep nearby die
  if( !findCreep( self ) )
  {
    G_Damage( self, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
    return;
  }
  
  self->nextthink = level.time + BG_FindNextThinkForBuildable( self->s.clientNum );
}




//==================================================================================




void DAcidTube_Think( gentity_t *self );

/*
================
DAcidTube_Damage

damage function for Droid Acid Tube
================
*/
void DAcidTube_Damage( gentity_t *self )
{
  if( !( self->s.eFlags & EF_FIRING ) )
  {
    self->s.eFlags |= EF_FIRING;
    G_AddEvent( self, EV_GIB_DROID, DirToByte( self->s.origin2 ) );
  }
    
  if( ( self->timestamp + 10000 ) > level.time )
    self->think = DAcidTube_Damage;
  else
  {
    self->think = DAcidTube_Think;
    self->s.eFlags &= ~EF_FIRING;
  }

  //do some damage
  G_SelectiveRadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, self->splashMethodOfDeath, PTE_DROIDS );

  self->nextthink = level.time + BG_FindNextThinkForBuildable( self->s.clientNum );
}

/*
================
DAcidTube_Think

think function for Droid Acid Tube
================
*/
void DAcidTube_Think( gentity_t *self )
{
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { 200, 200, 200 };
  vec3_t    mins, maxs;
  int       i, num;
  gentity_t *enemy;

  VectorAdd( self->s.origin, range, maxs );
  VectorSubtract( self->s.origin, range, mins );
  
  //if there is no creep nearby die
  if( !findCreep( self ) )
  {
    G_Damage( self, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
    return;
  }
  
  //do some damage
  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
  {
    enemy = &g_entities[ entityList[ i ] ];
    
    if( enemy->client && enemy->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    {
      self->timestamp = level.time;
      self->think = DAcidTube_Damage;
      self->nextthink = level.time + 100;
      G_setBuildableAnim( self, BANIM_ATTACK1 );
    }
  }

  self->nextthink = level.time + BG_FindNextThinkForBuildable( self->s.clientNum );
}




//==================================================================================



/*
================
ddef_fireonemeny

Used by DDef2_Think to fire at enemy
================
*/
void ddef_fireonenemy( gentity_t *self, int firespeed )
{
  vec3_t  dirToTarget;
 
  VectorSubtract( self->enemy->s.pos.trBase, self->s.pos.trBase, dirToTarget );
  VectorNormalize( dirToTarget );
  vectoangles( dirToTarget, self->s.angles2 );

  //fire at target
  FireWeapon( self );
  G_setBuildableAnim( self, BANIM_ATTACK1 );
  self->count = level.time + firespeed;
}

/*
================
ddef_checktarget

Used by DDef2_Think to check enemies for validity
================
*/
qboolean ddef_checktarget( gentity_t *self, gentity_t *target, int range )
{
  vec3_t    distance;
  trace_t   trace;

  if( !target ) // Do we have a target?
    return qfalse;
  if( !target->inuse ) // Does the target still exist?
    return qfalse;
  if( target == self ) // is the target us?
    return qfalse;
  if( !target->client ) // is the target a bot or player?
    return qfalse;
  if( target->client->ps.stats[ STAT_PTEAM ] == PTE_DROIDS ) // is the target one of us?
    return qfalse;
  if( target->client->sess.sessionTeam == TEAM_SPECTATOR ) // is the target alive?
    return qfalse;
  if( target->health <= 0 ) // is the target still alive?
    return qfalse;

  VectorSubtract( target->r.currentOrigin, self->r.currentOrigin, distance );
  if( VectorLength( distance ) > range ) // is the target within range?
    return qfalse;

  trap_Trace( &trace, self->s.pos.trBase, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
  if ( trace.contents & CONTENTS_SOLID ) // can we see the target?
    return qfalse;

  return qtrue;
}

/*
================
ddef_findenemy

Used by DDef2_Think to locate enemy gentities
================
*/
void ddef_findenemy( gentity_t *ent, int range )
{
  gentity_t *target;

  target = g_entities;

  //iterate through entities
  for (; target < &g_entities[ level.num_entities ]; target++)
  {
    //if target is not valid keep searching
    if( !ddef_checktarget( ent, target, range ) )
      continue;
      
    //we found a target
    ent->enemy = target;
    return;
  }

  //couldn't find a target
  ent->enemy = NULL;
}

/*
================
DDef2_Think

think function for Droid Defense
================
*/
void DDef2_Think( gentity_t *self )
{
  int range =     BG_FindRangeForBuildable( self->s.clientNum );
  int firespeed = BG_FindFireSpeedForBuildable( self->s.clientNum );

  self->nextthink = level.time + BG_FindNextThinkForBuildable( self->s.clientNum );

  //if there is no creep nearby die
  if( !findCreep( self ) )
  {
    G_Damage( self, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
    return;
  }

  //if the current target is not valid find a new one
  if( !ddef_checktarget( self, self->enemy, range ) )
    ddef_findenemy( self, range );

  //if a new target cannot be found don't do anything
  if( !self->enemy )
    return;
  
  //if we are pointing at our target and we can fire shoot it
  if( self->count < level.time )
    ddef_fireonenemy( self, firespeed );
}



//==================================================================================



/*
================
HRpt_Think

Think for human power repeater
================
*/
void HRpt_Think( gentity_t *self )
{
  int       i;
  int       count = 0;
  qboolean  reactor = qfalse;
  gentity_t *ent;
  
  //iterate through entities
  for ( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
  {
    if( !Q_stricmp( ent->classname, "team_human_spawn" ) && ent->parentNode == self )
      count++;
      
    if( !Q_stricmp( ent->classname, "team_human_reactor" ) )
      reactor = qtrue;
  }
  
  //if repeater has children and there is a reactor then this is active
  if( count && reactor )
    self->active = qtrue;
  else
    self->active = qfalse;

  self->powered = reactor;

  self->nextthink = level.time + REFRESH_TIME;
}




//==================================================================================



/*
================
HMCU_Activate

Called when a human activates an MCU
================
*/
void HMCU_Activate( gentity_t *self, gentity_t *other, gentity_t *activator )
{
  //only humans can activate this
  if( activator->client->ps.stats[ STAT_PTEAM ] != PTE_HUMANS ) return;
  
  //if this is powered then call the mcu menu
  if( self->powered )
    G_AddPredictableEvent( activator, EV_MENU, MN_H_MCU );
  else
    G_AddPredictableEvent( activator, EV_MENU, MN_H_MCUPOWER );
}

/*
================
HMCU_Think

Think for mcu
================
*/
void HMCU_Think( gentity_t *self )
{
  //make sure we have power
  self->nextthink = level.time + REFRESH_TIME;
  
  self->powered = findPower( self );
}




//==================================================================================



/*
================
HFM_Touch

Called when a floatmine is triggered
================
*/
void HFM_Touch( gentity_t *self, gentity_t *other, trace_t *trace )
{
  //can't blow up twice
  if( self->health <= 0 )
    return;
    
  //go boom
  G_Damage( self, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
}

/*
================
HFM_Die

Called when a floatmine dies
================
*/
void HFM_Die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
  vec3_t  dir;
  
  // we don't have a valid direction, so just point straight up
  dir[0] = dir[1] = 0;
  dir[2] = -1;

  //do a bit of radius damage
  G_RadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, self->splashMethodOfDeath );

  //pretty events and item cleanup
  self->s.modelindex = 0; //don't draw the model once its destroyed
  G_AddEvent( self, EV_ITEM_EXPLOSION, DirToByte( dir ) );
  self->r.contents = CONTENTS_TRIGGER;
  self->timestamp = level.time;

  self->think = freeBuildable;
  self->die = nullDieFunction;
  self->nextthink = level.time + 100;
  
  trap_LinkEntity( self );
}

/*
================
HFM_Think

Think for floatmine
================
*/
void HFM_Think( gentity_t *self )
{
  //make sure we have power
  self->nextthink = level.time + REFRESH_TIME;
  
  if( !( self->powered = findPower( self ) ) )
    G_Damage( self, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
}




//==================================================================================




//TA: the following defense turret code was written by
// "fuzzysteve"           (fuzzysteve@quakefiles.com) and
// Anthony "inolen" Pesch (www.inolen.com)
//with (heavy) modifications by me of course :)
  
#define HDEF1_ANGULARSPEED      10  //degrees/think ~= 200deg/sec
#define HDEF1_ACCURACYTOLERANCE HDEF1_ANGULARSPEED / 2 //angular difference for turret to fire
#define HDEF1_VERTICALCAP       20 //+/- maximum pitch
#define HDEF1_PROJSPEED         2000.0f //speed of projectile (used in prediction)

/*
================
hdef1_trackenemy

Used by HDef1_Think to track enemy location
================
*/
qboolean hdef1_trackenemy( gentity_t *self )
{
  vec3_t  dirToTarget, angleToTarget, angularDiff;
  float   temp;
  float   distanceToTarget = BG_FindRangeForBuildable( self->s.clientNum );
  float   timeTilImpact;
  vec3_t  halfAcceleration;
  vec3_t  thirdJerk;
  int     i;

  VectorSubtract( self->enemy->s.pos.trBase, self->s.pos.trBase, dirToTarget );

//lead targets
#if 0
  distanceToTarget = VectorLength( dirToTarget );
  timeTilImpact = distanceToTarget / 2000.0f;
  VectorMA( self->enemy->s.pos.trBase, timeTilImpact, self->enemy->s.pos.trDelta, dirToTarget );
  VectorSubtract( dirToTarget, self->s.pos.trBase, dirToTarget );
#endif

//better, but more expensive method
#if 0
  VectorScale( self->enemy->acceleration, 1.0f / 2.0f, halfAcceleration );
  VectorScale( self->enemy->jerk, 1.0f / 3.0f, thirdJerk );

  //O( time ) - worst case O( time ) = 250 iterations
  for( i = 0; ( i * HDEF1_PROJSPEED ) / 1000.0f < distanceToTarget; i++ )
  {
    float time = (float)i / 1000.0f;

    VectorMA( self->enemy->s.pos.trBase, time, self->enemy->s.pos.trDelta, dirToTarget );
    VectorMA( dirToTarget, time * time, halfAcceleration, dirToTarget );
    VectorMA( dirToTarget, time * time * time, thirdJerk, dirToTarget );
    VectorSubtract( dirToTarget, self->s.pos.trBase, dirToTarget );
    distanceToTarget = VectorLength( dirToTarget );

    distanceToTarget -= self->enemy->r.maxs[ 0 ];
  }
#endif
  
  VectorNormalize( dirToTarget );
  
  vectoangles( dirToTarget, angleToTarget );

  angularDiff[ PITCH ] = AngleSubtract( self->s.angles2[ PITCH ], angleToTarget[ PITCH ] );
  angularDiff[ YAW ] = AngleSubtract( self->s.angles2[ YAW ], angleToTarget[ YAW ] );

  //if not pointing at our target then move accordingly
  if( angularDiff[ PITCH ] < -HDEF1_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] += HDEF1_ANGULARSPEED;
  else if( angularDiff[ PITCH ] > HDEF1_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] -= HDEF1_ANGULARSPEED;
  else
    self->s.angles2[ PITCH ] = angleToTarget[ PITCH ];

  //disallow vertical movement past a certain limit
  temp = fabs( self->s.angles2[ PITCH ] );
  if( temp > 180 )
    temp -= 360;
  
  if( temp < -HDEF1_VERTICALCAP )
    self->s.angles2[ PITCH ] = (-360)+HDEF1_VERTICALCAP;
  else if( temp > HDEF1_VERTICALCAP )
    self->s.angles2[ PITCH ] = -HDEF1_VERTICALCAP;
    
  //if not pointing at our target then move accordingly
  if( angularDiff[ YAW ] < -HDEF1_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] += HDEF1_ANGULARSPEED;
  else if( angularDiff[ YAW ] > HDEF1_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] -= HDEF1_ANGULARSPEED;
  else
    self->s.angles2[ YAW ] = angleToTarget[ YAW ];
    
  trap_LinkEntity( self );

  //if pointing at our target return true
  if( abs( angleToTarget[ YAW ] - self->s.angles2[ YAW ] ) <= HDEF1_ACCURACYTOLERANCE &&
      abs( angleToTarget[ PITCH ] - self->s.angles2[ PITCH ] ) <= HDEF1_ACCURACYTOLERANCE )
    return qtrue;
    
  return qfalse;
}

#define HDEF2_ANGULARSPEED      20  //degrees/think ~= 200deg/sec
#define HDEF2_ACCURACYTOLERANCE HDEF2_ANGULARSPEED / 2 //angular difference for turret to fire
#define HDEF2_VERTICALCAP       30  //- maximum pitch

/*
================
hdef2_trackenemy

Used by HDef1_Think to track enemy location
================
*/
qboolean hdef2_trackenemy( gentity_t *self )
{
  vec3_t  dirToTarget, angleToTarget, angularDiff;
  float   temp;

  VectorSubtract( self->enemy->s.pos.trBase, self->s.pos.trBase, dirToTarget );

  VectorNormalize( dirToTarget );
  
  vectoangles( dirToTarget, angleToTarget );

  angularDiff[ PITCH ] = AngleSubtract( self->s.angles2[ PITCH ], angleToTarget[ PITCH ] );
  angularDiff[ YAW ] = AngleSubtract( self->s.angles2[ YAW ], angleToTarget[ YAW ] );

  //if not pointing at our target then move accordingly
  if( angularDiff[ PITCH ] < -HDEF2_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] += HDEF2_ANGULARSPEED;
  else if( angularDiff[ PITCH ] > HDEF2_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] -= HDEF2_ANGULARSPEED;
  else
    self->s.angles2[ PITCH ] = angleToTarget[ PITCH ];

  //disallow vertical movement past a certain limit
  temp = fabs( self->s.angles2[ PITCH ] );
  if( temp > 180 )
    temp -= 360;
  
  if( temp < -HDEF2_VERTICALCAP )
    self->s.angles2[ PITCH ] = (-360)+HDEF2_VERTICALCAP;
    
  //if not pointing at our target then move accordingly
  if( angularDiff[ YAW ] < -HDEF2_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] += HDEF2_ANGULARSPEED;
  else if( angularDiff[ YAW ] > HDEF2_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] -= HDEF2_ANGULARSPEED;
  else
    self->s.angles2[ YAW ] = angleToTarget[ YAW ];
    
  trap_LinkEntity( self );

  //if pointing at our target return true
  if( abs( angleToTarget[ YAW ] - self->s.angles2[ YAW ] ) <= HDEF2_ACCURACYTOLERANCE &&
      abs( angleToTarget[ PITCH ] - self->s.angles2[ PITCH ] ) <= HDEF2_ACCURACYTOLERANCE )
    return qtrue;
    
  return qfalse;
}

#define HDEF3_ANGULARSPEED      2     //degrees/think ~= 200deg/sec
#define HDEF3_ACCURACYTOLERANCE HDEF3_ANGULARSPEED / 2 //angular difference for turret to fire
#define HDEF3_VERTICALCAP       15    //+/- maximum pitch

/*
================
hdef3_trackenemy

Used by HDef1_Think to track enemy location
================
*/
qboolean hdef3_trackenemy( gentity_t *self )
{
  vec3_t  dirToTarget, angleToTarget, angularDiff;
  float   temp;

  VectorSubtract( self->enemy->s.pos.trBase, self->s.pos.trBase, dirToTarget );

  VectorNormalize( dirToTarget );
  
  vectoangles( dirToTarget, angleToTarget );

  angularDiff[ PITCH ] = AngleSubtract( self->s.angles2[ PITCH ], angleToTarget[ PITCH ] );
  angularDiff[ YAW ] = AngleSubtract( self->s.angles2[ YAW ], angleToTarget[ YAW ] );

  //if not pointing at our target then move accordingly
  if( angularDiff[ PITCH ] < -HDEF3_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] += HDEF3_ANGULARSPEED;
  else if( angularDiff[ PITCH ] > HDEF3_ACCURACYTOLERANCE )
    self->s.angles2[ PITCH ] -= HDEF3_ANGULARSPEED;
  else
    self->s.angles2[ PITCH ] = angleToTarget[ PITCH ];

  //disallow vertical movement past a certain limit
  temp = fabs( self->s.angles2[ PITCH ] );
  if( temp > 180 )
    temp -= 360;
  
  if( temp < -HDEF3_VERTICALCAP )
    self->s.angles2[ PITCH ] = (-360)+HDEF3_VERTICALCAP;
  else if( temp > HDEF3_VERTICALCAP )
    self->s.angles2[ PITCH ] = -HDEF3_VERTICALCAP;
    
  //if not pointing at our target then move accordingly
  if( angularDiff[ YAW ] < -HDEF3_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] += HDEF3_ANGULARSPEED;
  else if( angularDiff[ YAW ] > HDEF3_ACCURACYTOLERANCE )
    self->s.angles2[ YAW ] -= HDEF3_ANGULARSPEED;
  else
    self->s.angles2[ YAW ] = angleToTarget[ YAW ];
    
  trap_LinkEntity( self );

  //if pointing at our target return true
  if( abs( angleToTarget[ YAW ] - self->s.angles2[ YAW ] ) <= HDEF3_ACCURACYTOLERANCE &&
      abs( angleToTarget[ PITCH ] - self->s.angles2[ PITCH ] ) <= HDEF3_ACCURACYTOLERANCE )
    return qtrue;
    
  return qfalse;
}

/*
================
hdef_fireonemeny

Used by HDef_Think to fire at enemy
================
*/
void hdef_fireonenemy( gentity_t *self, int firespeed )
{
  //fire at target
  FireWeapon( self );
  G_setBuildableAnim( self, BANIM_ATTACK1 );
  self->count = level.time + firespeed;
}

/*
================
hdef_checktarget

Used by HDef_Think to check enemies for validity
================
*/
qboolean hdef_checktarget( gentity_t *self, gentity_t *target, int range )
{
  vec3_t    distance;
  trace_t   trace;

  if( !target ) // Do we have a target?
    return qfalse;
  if( !target->inuse ) // Does the target still exist?
    return qfalse;
  if( target == self ) // is the target us?
    return qfalse;
  if( !target->client ) // is the target a bot or player?
    return qfalse;
  if( target->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS ) // is the target one of us?
    return qfalse;
  if( target->client->sess.sessionTeam == TEAM_SPECTATOR ) // is the target alive?
    return qfalse;
  if( target->client->ps.stats[ STAT_STATE ] & SS_INFESTING ) // is the target alive?
    return qfalse;
  if( target->health <= 0 ) // is the target still alive?
    return qfalse;

  VectorSubtract( target->r.currentOrigin, self->r.currentOrigin, distance );
  if( VectorLength( distance ) > range ) // is the target within range?
    return qfalse;

  trap_Trace( &trace, self->s.pos.trBase, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
  if ( trace.contents & CONTENTS_SOLID ) // can we see the target?
    return qfalse;

  return qtrue;
}


/*
================
hdef_findenemy

Used by HDef_Think to locate enemy gentities
================
*/
void hdef_findenemy( gentity_t *ent, int range )
{
  gentity_t *target;

  target = g_entities;

  //iterate through entities
  for (; target < &g_entities[ level.num_entities ]; target++)
  {
    //if target is not valid keep searching
    if( !hdef_checktarget( ent, target, range ) )
      continue;
      
    //we found a target
    ent->enemy = target;
    return;
  }

  //couldn't find a target
  ent->enemy = NULL;
}


/*
================
HDef_Think

think function for Human Defense
================
*/
void HDef_Think( gentity_t *self )
{
  int range =     BG_FindRangeForBuildable( self->s.clientNum );
  int firespeed = BG_FindFireSpeedForBuildable( self->s.clientNum );

  self->nextthink = level.time + BG_FindNextThinkForBuildable( self->s.clientNum );

  //find power for self
  self->powered = findPower( self );

  //if not powered don't do anything and check again for power next think
  if( !self->powered )
  {
    self->nextthink = level.time + REFRESH_TIME;
    return;
  }
  
  //if the current target is not valid find a new one
  if( !hdef_checktarget( self, self->enemy, range ) )
    hdef_findenemy( self, range );

  //if a new target cannot be found don't do anything
  if( !self->enemy )
    return;
  
  //if we are pointing at our target and we can fire shoot it
  switch( self->s.clientNum )
  {
    case BA_H_DEF1:
      if( hdef1_trackenemy( self ) && ( self->count < level.time ) )
        hdef_fireonenemy( self, firespeed );
      break;
      
    case BA_H_DEF2:
      if( hdef2_trackenemy( self ) && ( self->count < level.time ) )
        hdef_fireonenemy( self, firespeed );
      break;
      
    case BA_H_DEF3:
      if( hdef3_trackenemy( self ) && ( self->count < level.time ) )
        hdef_fireonenemy( self, firespeed );
      break;

    default:
      Com_Printf( S_COLOR_YELLOW "WARNING: Unknown turret type in think\n" );
      break;
  }
}




//==================================================================================




/*
================
HSpawn_blast

Called when a human spawn explodes
think function
================
*/
void HSpawn_Blast( gentity_t *self )
{
  vec3_t  dir;

  // we don't have a valid direction, so just point straight up
  dir[0] = dir[1] = 0;
  dir[2] = 1;

  self->s.modelindex = 0; //don't draw the model once its destroyed
  G_AddEvent( self, EV_ITEM_EXPLOSION, DirToByte( dir ) );
  self->r.contents = CONTENTS_TRIGGER;
  self->timestamp = level.time;

  //do some radius damage
  G_RadiusDamage( self->s.pos.trBase, self->parent, self->splashDamage,
    self->splashRadius, self, self->splashMethodOfDeath );

  self->think = freeBuildable;
  self->nextthink = level.time + 100;
}


/*
================
HSpawn_die

Called when a human spawn dies
================
*/
void HSpawn_Die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
  //pretty events and cleanup
  G_setBuildableAnim( self, BANIM_DESTROY1 );
  G_setIdleBuildableAnim( self, BANIM_DESTROYED );
  
  self->die = nullDieFunction;
  self->think = HSpawn_Blast;
  self->nextthink = level.time + 1500; //wait 1.5 seconds before damaging others

  trap_LinkEntity( self );
}

/*
================
HSpawn_Think

Think for human spawn
================
*/
void HSpawn_Think( gentity_t *self )
{
  //search for power
  self->nextthink = level.time + REFRESH_TIME;

  self->powered = findPower( self );
}




//==================================================================================




/*
================
G_itemFits

Checks to see if an item fits in a specific area
================
*/
itemBuildError_t G_itemFits( gentity_t *ent, buildable_t buildable, int distance )
{
  vec3_t            forward;
  vec3_t            angles;
  vec3_t            player_origin, entity_origin;
  vec3_t            mins, maxs;
  vec3_t            temp_v;
  trace_t           tr1, tr2;
  int               i;
  itemBuildError_t  reason = IBE_NONE;
  gentity_t         *tempent, *closestPower;
  int               minDistance = 10000;
  int               templength;

  VectorCopy( ent->s.apos.trBase, angles );
  angles[PITCH] = 0;  // always forward

  AngleVectors( angles, forward, NULL, NULL );
  VectorCopy( ent->s.pos.trBase, player_origin );
  VectorMA( player_origin, distance, forward, entity_origin );

  BG_FindBBoxForBuildable( buildable, mins, maxs );
  
  trap_Trace( &tr1, entity_origin, mins, maxs, entity_origin, ent->s.number, MASK_PLAYERSOLID );
  trap_Trace( &tr2, player_origin, NULL, NULL, entity_origin, ent->s.number, MASK_PLAYERSOLID );

  //this item does not fit here
  if( tr1.fraction < 1.0 || tr2.fraction < 1.0 )
    return IBE_NOROOM; //NO other reason is allowed to override this
    
  if( ent->client->ps.stats[ STAT_PTEAM ] == PTE_DROIDS )
  {
    //droid criteria
    //check there is creep near by for building on

    if( BG_FindCreepTestForBuildable( buildable ) )
    {
      for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
      {
        if( !Q_stricmp( tempent->classname, "team_droid_spawn" ) ||
            !Q_stricmp( tempent->classname, "team_droid_hivemind" ) )
        {
          VectorSubtract( entity_origin, tempent->s.origin, temp_v );
          if( VectorLength( temp_v ) <= ( CREEP_BASESIZE * 3 ) )
            break;
        }
      }

      if( i >= level.num_entities )
        reason = IBE_NOCREEP;
    }
    
    //look for a hivemind
    for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
    {
      if( !Q_stricmp( tempent->classname, "team_droid_hivemind" ) )
        break;
    }

    //if none found...
    if( i >= level.num_entities && buildable != BA_D_HIVEMIND )
    {
      if( buildable == BA_D_SPAWN )
        reason = IBE_SPWNWARN;
      else
        reason = IBE_NOHIVEMIND;
    }
    
    //can we only have one of these?
    if( BG_FindUniqueTestForBuildable( buildable ) )
    {
      for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
      {
        if( !Q_stricmp( tempent->classname, "team_droid_hivemind" ) )
        {
          reason = IBE_HIVEMIND;
          break;
        }
      }
    }

    if( level.droidBuildPoints - BG_FindBuildPointsForBuildable( buildable ) < 0 )
      reason = IBE_NOASSERT;
  }
  else if( ent->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
  {
    //human criteria

    closestPower = g_entities + 1; //FIXME
    
    //find the nearest power entity
    for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
    {
      if( !Q_stricmp( tempent->classname, "team_human_reactor" ) ||
          !Q_stricmp( tempent->classname, "team_human_repeater" ) )
      {
        VectorSubtract( entity_origin, tempent->s.origin, temp_v );
        templength = VectorLength( temp_v );
        if( templength < minDistance && ( tempent->active || buildable == BA_H_SPAWN ) )
        {
          closestPower = tempent;
          minDistance = templength;
        }
      }
    }
    
    //if this power entity satisfies expression
    if( !(
           (
             !Q_stricmp( closestPower->classname, "team_human_reactor" ) &&
             minDistance <= REACTOR_BASESIZE
           ) || 
           (
             !Q_stricmp( closestPower->classname, "team_human_repeater" ) &&
             minDistance <= REPEATER_BASESIZE &&
             (
               ( buildable == BA_H_SPAWN && closestPower->powered ) ||
               ( closestPower->powered && closestPower->active )
             )
           )
         )
      )
    {
      //tell player to build a repeater to provide power
      if( buildable != BA_H_REACTOR && buildable != BA_H_REPEATER )
        reason = IBE_REPEATER;

      //warn that the current spawn will not be externally powered
      if( buildable == BA_H_SPAWN )
        reason = IBE_RPLWARN;
    }

    //check that there is a parent reactor when building a repeater
    if( buildable == BA_H_REPEATER )
    {
      for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
      {
        if( !Q_stricmp( tempent->classname, "team_human_reactor" ) )
          break;
      }
      
      if( i >= level.num_entities )
        reason = IBE_RPTWARN;
    }
      
    //can we only build one of these?
    if( BG_FindUniqueTestForBuildable( buildable ) )
    {
      for ( i = 1, tempent = g_entities + i; i < level.num_entities; i++, tempent++ )
      {
        if( !Q_stricmp( tempent->classname, "team_human_reactor" ) )
        {
          reason = IBE_REACTOR;
          break;
        }
      }
    }

    if( level.humanBuildPoints - BG_FindBuildPointsForBuildable( buildable ) < 0 )
      reason = IBE_NOPOWER;
  }

  return reason;
}


/*
================
G_buildItem

Spawns a buildable
================
*/
gentity_t *G_buildItem( gentity_t *ent, buildable_t buildable, int distance, float speed )
{
  vec3_t  forward;
  vec3_t  angles;
  vec3_t  origin;
  gentity_t *built;

  VectorCopy( ent->s.apos.trBase, angles );
  angles[PITCH] = 0;  // always forward

  AngleVectors( angles, forward, NULL, NULL );
  VectorCopy( ent->s.pos.trBase, origin );
  VectorMA( origin, distance, forward, origin );

  //spawn the buildable
  built = G_Spawn();

  built->s.eType = ET_BUILDABLE;

  built->classname = BG_FindEntityNameForBuildable( buildable );
  built->item = BG_FindItemForBuildable( buildable );
  
  built->s.modelindex = built->item - bg_itemlist; // store item number in modelindex
  built->s.clientNum = buildable; //so we can tell what this is on the client side

  BG_FindBBoxForBuildable( buildable, built->r.mins, built->r.maxs );
  built->biteam = built->s.modelindex2 = BG_FindTeamForBuildable( buildable );
  built->health = BG_FindHealthForBuildable( buildable );
  
  built->damage = BG_FindDamageForBuildable( buildable );
  built->splashDamage = BG_FindSplashDamageForBuildable( buildable );
  built->splashRadius = BG_FindSplashRadiusForBuildable( buildable );
  built->splashMethodOfDeath = BG_FindMODForBuildable( buildable );
  
  G_setIdleBuildableAnim( built, BG_FindAnimForBuildable( buildable ) );
    
  built->nextthink = BG_FindNextThinkForBuildable( buildable );

  //things that vary for each buildable that aren't in the dbase
  switch( buildable )
  {
    case BA_D_SPAWN:
      built->die = DSpawn_Die;
      built->think = DSpawn_Think;
      built->pain = DSpawn_Pain;
      break;
      
    case BA_D_BARRICADE:
      built->die = DBarricade_Die;
      built->think = DBarricade_Think;
      built->pain = DBarricade_Pain;
      break;
      
    case BA_D_ACIDTUBE:
      built->die = DBarricade_Die;
      built->think = DAcidTube_Think;
      built->pain = DSpawn_Pain;
      break;
      
/*    case BA_D_DEF2:
      built->die = DDef1_Die;
      built->think = DDef2_Think;
      built->pain = DSpawn_Pain;
      built->enemy = NULL;
      built->s.weapon = BG_FindProjTypeForBuildable( buildable );
      break;*/
      
    case BA_D_HIVEMIND:
      built->die = DSpawn_Die;
      built->pain = DSpawn_Pain;
      break;
      
    case BA_H_SPAWN:
      built->die = HSpawn_Die;
      built->think = HSpawn_Think;
      break;
      
    case BA_H_DEF1:
    case BA_H_DEF2:
    case BA_H_DEF3:
      built->die = HSpawn_Die;
      built->think = HDef_Think;
      built->enemy = NULL;
      built->s.weapon = BG_FindProjTypeForBuildable( buildable );
      break;
      
    case BA_H_MCU:
      built->think = HMCU_Think;
      built->die = HSpawn_Die;
      built->use = HMCU_Activate;
      break;
      
    case BA_H_REACTOR:
      built->die = HSpawn_Die;
      built->powered = built->active = qtrue;
      break;
      
    case BA_H_REPEATER:
      built->think = HRpt_Think;
      built->die = HSpawn_Die;
      break;
      
    case BA_H_FLOATMINE:
      built->think = HFM_Think;
      built->die = HFM_Die;
      built->touch = HFM_Touch;
      break;
      
    default:
      //erk
      break;
  }

  built->takedamage = qtrue;
  built->s.number = built - g_entities;
  built->r.contents = CONTENTS_BODY;
  built->clipmask = MASK_PLAYERSOLID;

  if( ent->client )
    built->builtBy = ent->client->ps.clientNum;
  else
    built->builtBy = -1;

  G_SetOrigin( built, origin );
  VectorCopy( angles, built->s.angles );
  built->s.angles2[ YAW ] = angles[ YAW ];
  VectorCopy( origin, built->s.origin );
  built->s.pos.trType = BG_FindTrajectoryForBuildable( buildable );
  built->physicsBounce = BG_FindBounceForBuildable( buildable );
  built->s.pos.trTime = level.time;
  AngleVectors( ent->s.apos.trBase, built->s.pos.trDelta, NULL, NULL );
  
  VectorScale( built->s.pos.trDelta, speed, built->s.pos.trDelta );
  VectorSet( built->s.origin2, 0.0f, 0.0f, 1.0f );
  
  G_AddEvent( built, EV_BUILD_CONSTRUCT, BANIM_CONSTRUCT1 );

  trap_LinkEntity( built );

  return built;
}
