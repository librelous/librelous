// Copyright (C) 1999-2000 Id Software, Inc.
//

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

/*
================
G_Bounce

================
*/
static void G_Bounce( gentity_t *ent, trace_t *trace )
{
  vec3_t    velocity;
  float     dot;
  int       hitTime;
  float     minNormal;
  qboolean  invert;

  // reflect the velocity on the trace plane
  hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
  BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
  dot = DotProduct( velocity, trace->plane.normal );
  VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

  if( ent->s.eType == ET_BUILDABLE )
  {
    minNormal = BG_FindMinNormalForBuildable( ent->s.modelindex );
    invert = BG_FindInvertNormalForBuildable( ent->s.modelindex );
  }
  else
    minNormal = 0.707f;

  // cut the velocity to keep from bouncing forever
  if( ( trace->plane.normal[ 2 ] >= minNormal ||
      ( invert && trace->plane.normal[ 2 ] <= -minNormal ) ) &&
      trace->entityNum == ENTITYNUM_WORLD )
    VectorScale( ent->s.pos.trDelta, ent->physicsBounce, ent->s.pos.trDelta );
  else
    VectorScale( ent->s.pos.trDelta, 0.3f, ent->s.pos.trDelta );

  if( VectorLength( ent->s.pos.trDelta ) < 10 )
  {
    VectorMA( trace->endpos, 0.5, trace->plane.normal, trace->endpos ); // make sure it is off ground
    SnapVector( trace->endpos );
    G_SetOrigin( ent, trace->endpos );
    ent->s.groundEntityNum = trace->entityNum;
    VectorCopy( trace->plane.normal, ent->s.origin2 );
    VectorSet( ent->s.pos.trDelta, 0.0f, 0.0f, 0.0f );
    return;
  }

  VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
  VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
  ent->s.pos.trTime = level.time;
}


/*
================
G_Physics

================
*/
void G_Physics( gentity_t *ent, int msec )
{
  vec3_t    origin;
  trace_t   tr;
  int     contents;
  int     mask;
  int     bHealth = BG_FindHealthForBuildable( ent->s.modelindex );
  int     bRegen = BG_FindRegenRateForBuildable( ent->s.modelindex );

  //pack health, power and dcc
  if( ent->s.eType == ET_BUILDABLE )
  {
    ent->s.generic1 = (int)( ( (float)ent->health / (float)bHealth ) * 63.0f );

    if( ent->s.generic1 < 0 )
      ent->s.generic1 = 0;
    
    if( ent->powered )
      ent->s.generic1 |= B_POWERED_TOGGLEBIT;
    
    if( ent->dcced )
      ent->s.generic1 |= B_DCCED_TOGGLEBIT;

    ent->time1000 += msec;

    if( ent->time1000 >= 1000 )
    {
      ent->time1000 -= 1000;
      
      //regenerate health
      if( ent->health < bHealth && bRegen )
      {
        ent->health += bRegen;

        if( ent->health > bHealth )
          ent->health = bHealth;
      }
    }
  }
  
  // if groundentity has been set to -1, it may have been pushed off an edge
  if( ent->s.groundEntityNum == -1 )
  {
    if( ent->s.eType == ET_BUILDABLE )
    {
      if( ent->s.pos.trType != BG_FindTrajectoryForBuildable( ent->s.modelindex ) )
      {
        ent->s.pos.trType = BG_FindTrajectoryForBuildable( ent->s.modelindex );
        ent->s.pos.trTime = level.time;
      }
    }
    else if( ent->s.pos.trType != TR_GRAVITY )
    {
      ent->s.pos.trType = TR_GRAVITY;
      ent->s.pos.trTime = level.time;
    }
  }
  
  // trace a line from the previous position to the current position
  if( ent->clipmask )
    mask = ent->clipmask;
  else
    mask = MASK_PLAYERSOLID & ~CONTENTS_BODY;//MASK_SOLID;

  if( ent->s.pos.trType == TR_STATIONARY )
  {
    // check think function
    G_RunThink( ent );

    VectorCopy( ent->r.currentOrigin, origin );

    VectorMA( origin, -2.0f, ent->s.origin2, origin );

    trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->s.number, mask );
    
    if( tr.fraction == 1.0 )
      ent->s.groundEntityNum = -1;

    return;
  }

  // get current position
  BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

  trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->s.number, mask );

  VectorCopy( tr.endpos, ent->r.currentOrigin );

  if( tr.startsolid )
    tr.fraction = 0;

  trap_LinkEntity( ent ); // FIXME: avoid this for stationary?

  // check think function
  G_RunThink( ent );

  if( tr.fraction == 1 ) 
    return;

  // if it is in a nodrop volume, remove it
  contents = trap_PointContents( ent->r.currentOrigin, -1 );
  if( contents & CONTENTS_NODROP )
  {
    G_FreeEntity( ent );
    return;
  }

  G_Bounce( ent, &tr );
}

