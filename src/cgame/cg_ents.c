// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_ents.c -- present snapshot entities, happens every single frame

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
======================
CG_PositionEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
              qhandle_t parentModel, char *tagName ) {
  int       i;
  orientation_t lerped;

  // lerp the tag
  trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
    1.0 - parent->backlerp, tagName );

  // FIXME: allow origin offsets along tag?
  VectorCopy( parent->origin, entity->origin );
  for ( i = 0 ; i < 3 ; i++ ) {
    VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
  }

  // had to cast away the const to avoid compiler problems...
  MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, entity->axis );
  entity->backlerp = parent->backlerp;
}


/*
======================
CG_PositionRotatedEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
              qhandle_t parentModel, char *tagName ) {
  int       i;
  orientation_t lerped;
  vec3_t      tempAxis[3];

//AxisClear( entity->axis );
  // lerp the tag
  trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
    1.0 - parent->backlerp, tagName );

  // FIXME: allow origin offsets along tag?
  VectorCopy( parent->origin, entity->origin );
  for ( i = 0 ; i < 3 ; i++ ) {
    VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
  }

  // had to cast away the const to avoid compiler problems...
  MatrixMultiply( entity->axis, lerped.axis, tempAxis );
  MatrixMultiply( tempAxis, ((refEntity_t *)parent)->axis, entity->axis );
}



/*
==========================================================================

FUNCTIONS CALLED EACH FRAME

==========================================================================
*/

/*
======================
CG_SetEntitySoundPosition

Also called by event processing code
======================
*/
void CG_SetEntitySoundPosition( centity_t *cent ) {
  if ( cent->currentState.solid == SOLID_BMODEL ) {
    vec3_t  origin;
    float *v;

    v = cgs.inlineModelMidpoints[ cent->currentState.modelindex ];
    VectorAdd( cent->lerpOrigin, v, origin );
    trap_S_UpdateEntityPosition( cent->currentState.number, origin );
  } else {
    trap_S_UpdateEntityPosition( cent->currentState.number, cent->lerpOrigin );
  }
}

/*
==================
CG_EntityEffects

Add continuous entity effects, like local entity emission and lighting
==================
*/
static void CG_EntityEffects( centity_t *cent ) {

  // update sound origins
  CG_SetEntitySoundPosition( cent );

  // add loop sound
  if ( cent->currentState.loopSound ) {
    if (cent->currentState.eType != ET_SPEAKER) {
      trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, 
                    cgs.gameSounds[ cent->currentState.loopSound ] );
    } else {
      trap_S_AddRealLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, 
                    cgs.gameSounds[ cent->currentState.loopSound ] );
    }
  }


  // constant light glow
  if ( cent->currentState.constantLight )
  {
    int   cl;
    int   i, r, g, b;

    cl = cent->currentState.constantLight;
    r = cl & 255;
    g = ( cl >> 8 ) & 255;
    b = ( cl >> 16 ) & 255;
    i = ( ( cl >> 24 ) & 255 ) * 4;
    trap_R_AddLightToScene( cent->lerpOrigin, i, r, g, b );
  }

}


/*
==================
CG_General
==================
*/
static void CG_General( centity_t *cent ) {
  refEntity_t     ent;
  entityState_t   *s1;

  s1 = &cent->currentState;

  // if set to invisible, skip
  if (!s1->modelindex) {
    return;
  }

  memset (&ent, 0, sizeof(ent));

  // set frame

  ent.frame = s1->frame;
  ent.oldframe = ent.frame;
  ent.backlerp = 0;

  VectorCopy( cent->lerpOrigin, ent.origin);
  VectorCopy( cent->lerpOrigin, ent.oldorigin);

  ent.hModel = cgs.gameModels[s1->modelindex];

  // player model
  if (s1->number == cg.snap->ps.clientNum) {
    ent.renderfx |= RF_THIRD_PERSON;  // only draw from mirrors
  }

  // convert angles to axis
  AnglesToAxis( cent->lerpAngles, ent.axis );

  // add to refresh list
  trap_R_AddRefEntityToScene (&ent);
}

/*
==================
CG_Speaker

Speaker entities can automatically play sounds
==================
*/
static void CG_Speaker( centity_t *cent ) {
  if ( ! cent->currentState.clientNum ) { // FIXME: use something other than clientNum...
    return;   // not auto triggering
  }

  if ( cg.time < cent->miscTime ) {
    return;
  }

  trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.gameSounds[cent->currentState.eventParm] );

  //  ent->s.frame = ent->wait * 10;
  //  ent->s.clientNum = ent->random * 10;
  cent->miscTime = cg.time + cent->currentState.frame * 100 + cent->currentState.clientNum * 100 * crandom();
}


//============================================================================

/*
===============
CG_Missile
===============
*/
static void CG_Missile( centity_t *cent )
{
  refEntity_t         ent;
  entityState_t       *s1;
  const weaponInfo_t  *weapon;
  vec3_t              up;
  float               fraction;
  int                 index;

  s1 = &cent->currentState;
  if ( s1->weapon > WP_NUM_WEAPONS ) {
    s1->weapon = 0;
  }
  weapon = &cg_weapons[s1->weapon];

  // calculate the axis
  VectorCopy( s1->angles, cent->lerpAngles);

  // add trails
  if ( weapon->missileTrailFunc )
  {
    weapon->missileTrailFunc( cent, weapon );
  }

  // add dynamic light
  if ( weapon->missileDlight ) {
    trap_R_AddLightToScene(cent->lerpOrigin, weapon->missileDlight,
      weapon->missileDlightColor[0], weapon->missileDlightColor[1], weapon->missileDlightColor[2] );
  }

  // add missile sound
  if ( weapon->missileSound ) {
    vec3_t  velocity;

    BG_EvaluateTrajectoryDelta( &cent->currentState.pos, cg.time, velocity );

    //TA: FIXME: hack until i figure out why trap_S_ALS has a problem with velocity`
    if( s1->weapon == WP_PLASMAGUN )
      VectorClear( velocity );

    trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, velocity, weapon->missileSound );
  }

  // create the render entity
  memset (&ent, 0, sizeof(ent));
  VectorCopy( cent->lerpOrigin, ent.origin);
  VectorCopy( cent->lerpOrigin, ent.oldorigin);

  switch( cent->currentState.weapon )
  {
    case WP_PLASMAGUN:
      ent.reType = RT_SPRITE;
      ent.radius = 16;
      ent.rotation = 0;
      ent.customShader = cgs.media.plasmaBallShader;
      trap_R_AddRefEntityToScene( &ent );
      return;
      break;

    case WP_PULSE_RIFLE:
      ent.reType = RT_SPRITE;
      ent.radius = 4;
      ent.rotation = 0;
      ent.customShader = cgs.media.plasmaBallShader;
      trap_R_AddRefEntityToScene( &ent );
      return;
      break;

    case WP_LUCIFER_CANON:
      ent.skinNum = cg.clientFrame & 1;
      ent.hModel = weapon->missileModel;
      ent.renderfx = weapon->missileRenderfx | RF_NOSHADOW;

      // convert direction of travel into axis
      if ( VectorNormalize2( s1->pos.trDelta, ent.axis[ 0 ] ) == 0 )
        ent.axis[ 0 ][ 2 ] = 1;

      RotateAroundDirection( ent.axis, cg.time / 4 );

      fraction = (float)s1->generic1 / (float)LC_TOTAL_CHARGE;
      VectorScale( ent.axis[ 0 ], fraction, ent.axis[ 0 ] );
      VectorScale( ent.axis[ 1 ], fraction, ent.axis[ 1 ] );
      VectorScale( ent.axis[ 2 ], fraction, ent.axis[ 2 ] );
      ent.nonNormalizedAxes = qtrue;
      
      break;

    case WP_FLAMER:
      //TA: don't actually display the missile (use the particle engine)
      return;
      break;

    default:
      // flicker between two skins
      ent.skinNum = cg.clientFrame & 1;
      ent.hModel = weapon->missileModel;
      ent.renderfx = weapon->missileRenderfx | RF_NOSHADOW;

      // convert direction of travel into axis
      if ( VectorNormalize2( s1->pos.trDelta, ent.axis[ 0 ] ) == 0 )
        ent.axis[ 0 ][ 2 ] = 1;

      // spin as it moves
      if( s1->pos.trType != TR_STATIONARY )
        RotateAroundDirection( ent.axis, cg.time / 4 );
      else
        RotateAroundDirection( ent.axis, s1->time );
  }
  
  // add to refresh list, possibly with quad glow
  CG_AddRefEntityWithPowerups( &ent, s1->powerups, TEAM_FREE );
}

/*
===============
CG_Grapple

This is called when the grapple is sitting up against the wall
===============
*/
static void CG_Grapple( centity_t *cent ) {
  refEntity_t     ent;
  entityState_t   *s1;
  const weaponInfo_t    *weapon;

  s1 = &cent->currentState;
  if ( s1->weapon > WP_NUM_WEAPONS ) {
    s1->weapon = 0;
  }
  weapon = &cg_weapons[s1->weapon];

  // calculate the axis
  VectorCopy( s1->angles, cent->lerpAngles);

#if 0 // FIXME add grapple pull sound here..?
  // add missile sound
  if ( weapon->missileSound ) {
    trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->missileSound );
  }
#endif

  // Will draw cable if needed
  CG_GrappleTrail ( cent, weapon );

  // create the render entity
  memset (&ent, 0, sizeof(ent));
  VectorCopy( cent->lerpOrigin, ent.origin);
  VectorCopy( cent->lerpOrigin, ent.oldorigin);

  // flicker between two skins
  ent.skinNum = cg.clientFrame & 1;
  ent.hModel = weapon->missileModel;
  ent.renderfx = weapon->missileRenderfx | RF_NOSHADOW;

  // convert direction of travel into axis
  if ( VectorNormalize2( s1->pos.trDelta, ent.axis[0] ) == 0 ) {
    ent.axis[0][2] = 1;
  }

  trap_R_AddRefEntityToScene( &ent );
}

/*
===============
CG_Mover
===============
*/
static void CG_Mover( centity_t *cent ) {
  refEntity_t     ent;
  entityState_t   *s1;

  s1 = &cent->currentState;

  // create the render entity
  memset (&ent, 0, sizeof(ent));
  VectorCopy( cent->lerpOrigin, ent.origin);
  VectorCopy( cent->lerpOrigin, ent.oldorigin);
  AnglesToAxis( cent->lerpAngles, ent.axis );

  ent.renderfx = RF_NOSHADOW;

  // flicker between two skins (FIXME?)
  ent.skinNum = ( cg.time >> 6 ) & 1;

  // get the model, either as a bmodel or a modelindex
  if ( s1->solid == SOLID_BMODEL ) {
    ent.hModel = cgs.inlineDrawModel[s1->modelindex];
  } else {
    ent.hModel = cgs.gameModels[s1->modelindex];
  }

  // add to refresh list
  trap_R_AddRefEntityToScene(&ent);

  // add the secondary model
  if ( s1->modelindex2 ) {
    ent.skinNum = 0;
    ent.hModel = cgs.gameModels[s1->modelindex2];
    trap_R_AddRefEntityToScene(&ent);
  }

}

/*
===============
CG_Beam

Also called as an event
===============
*/
void CG_Beam( centity_t *cent ) {
  refEntity_t     ent;
  entityState_t   *s1;

  s1 = &cent->currentState;

  // create the render entity
  memset (&ent, 0, sizeof(ent));
  VectorCopy( s1->pos.trBase, ent.origin );
  VectorCopy( s1->origin2, ent.oldorigin );
  AxisClear( ent.axis );
  ent.reType = RT_BEAM;

  ent.renderfx = RF_NOSHADOW;

  // add to refresh list
  trap_R_AddRefEntityToScene(&ent);
}


/*
===============
CG_Portal
===============
*/
static void CG_Portal( centity_t *cent ) {
  refEntity_t     ent;
  entityState_t   *s1;

  s1 = &cent->currentState;

  // create the render entity
  memset (&ent, 0, sizeof(ent));
  VectorCopy( cent->lerpOrigin, ent.origin );
  VectorCopy( s1->origin2, ent.oldorigin );
  ByteToDir( s1->eventParm, ent.axis[0] );
  PerpendicularVector( ent.axis[1], ent.axis[0] );

  // negating this tends to get the directions like they want
  // we really should have a camera roll value
  VectorSubtract( vec3_origin, ent.axis[1], ent.axis[1] );

  CrossProduct( ent.axis[0], ent.axis[1], ent.axis[2] );
  ent.reType = RT_PORTALSURFACE;
  ent.oldframe = s1->powerups;
  ent.frame = s1->frame;    // rotation speed
  ent.skinNum = s1->clientNum/256.0 * 360;  // roll offset

  // add to refresh list
  trap_R_AddRefEntityToScene(&ent);
}

//============================================================================

/*
=========================
CG_LightFlare
=========================
*/
static void CG_LightFlare( centity_t *cent )
{
  refEntity_t   flare;
  entityState_t *es;
  vec3_t        forward, delta;
  float         len;
  trace_t       tr;
  float         maxAngle;

  es = &cent->currentState;
  
  memset( &flare, 0, sizeof( flare ) );
  
  //bunch of geometry
  AngleVectors( es->angles, forward, NULL, NULL );
  VectorCopy( cent->lerpOrigin, flare.origin );
  VectorSubtract( flare.origin, cg.refdef.vieworg, delta );
  len = VectorLength( delta );
  VectorNormalize( delta );

  flare.reType = RT_SPRITE;
  flare.customShader = cgs.gameShaders[ es->modelindex ];
  flare.shaderRGBA[ 0 ] = 0xFF;
  flare.shaderRGBA[ 1 ] = 0xFF;
  flare.shaderRGBA[ 2 ] = 0xFF;
  flare.shaderRGBA[ 3 ] = 0xFF;

  //can only see the flare when in front of it
  flare.radius = len / es->origin2[ 0 ];
  maxAngle = es->origin2[ 1 ];

  if( maxAngle > 0.0f )
  {
    float radiusMod = 1.0f - ( 180.0f - RAD2DEG( acos( DotProduct( delta, forward ) ) ) ) / maxAngle;
    
    if( es->eFlags & EF_NODRAW )
      flare.radius *= radiusMod;
    else if( radiusMod < 0.0f )
      flare.radius = 0.0f;
  }   

  if( flare.radius < 0.0f )
    flare.radius = 0.0f;
  
  //if can't see the centre do not draw
  CG_Trace( &tr, flare.origin, NULL, NULL, cg.refdef.vieworg, 0, MASK_SHOT );
  if( tr.fraction < 1.0f )
    return;

  trap_R_AddRefEntityToScene( &flare );
}

/*
=========================
CG_AdjustPositionForMover

Also called by client movement prediction code
=========================
*/
void CG_AdjustPositionForMover( const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out ) {
  centity_t *cent;
  vec3_t  oldOrigin, origin, deltaOrigin;
  vec3_t  oldAngles, angles, deltaAngles;

  if ( moverNum <= 0 || moverNum >= ENTITYNUM_MAX_NORMAL ) {
    VectorCopy( in, out );
    return;
  }

  cent = &cg_entities[ moverNum ];
  if ( cent->currentState.eType != ET_MOVER ) {
    VectorCopy( in, out );
    return;
  }

  BG_EvaluateTrajectory( &cent->currentState.pos, fromTime, oldOrigin );
  BG_EvaluateTrajectory( &cent->currentState.apos, fromTime, oldAngles );

  BG_EvaluateTrajectory( &cent->currentState.pos, toTime, origin );
  BG_EvaluateTrajectory( &cent->currentState.apos, toTime, angles );

  VectorSubtract( origin, oldOrigin, deltaOrigin );
  VectorSubtract( angles, oldAngles, deltaAngles );

  VectorAdd( in, deltaOrigin, out );

  // FIXME: origin change when on a rotating object
}


/*
=============================
CG_InterpolateEntityPosition
=============================
*/
static void CG_InterpolateEntityPosition( centity_t *cent ) {
  vec3_t    current, next;
  float   f;

  // it would be an internal error to find an entity that interpolates without
  // a snapshot ahead of the current one
  if ( cg.nextSnap == NULL ) {
    CG_Error( "CG_InterpoateEntityPosition: cg.nextSnap == NULL" );
  }

  f = cg.frameInterpolation;

  // this will linearize a sine or parabolic curve, but it is important
  // to not extrapolate player positions if more recent data is available
  BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, current );
  BG_EvaluateTrajectory( &cent->nextState.pos, cg.nextSnap->serverTime, next );

  cent->lerpOrigin[0] = current[0] + f * ( next[0] - current[0] );
  cent->lerpOrigin[1] = current[1] + f * ( next[1] - current[1] );
  cent->lerpOrigin[2] = current[2] + f * ( next[2] - current[2] );

  BG_EvaluateTrajectory( &cent->currentState.apos, cg.snap->serverTime, current );
  BG_EvaluateTrajectory( &cent->nextState.apos, cg.nextSnap->serverTime, next );

  cent->lerpAngles[0] = LerpAngle( current[0], next[0], f );
  cent->lerpAngles[1] = LerpAngle( current[1], next[1], f );
  cent->lerpAngles[2] = LerpAngle( current[2], next[2], f );

}

/*
===============
CG_CalcEntityLerpPositions

===============
*/
static void CG_CalcEntityLerpPositions( centity_t *cent ) {
  // if this player does not want to see extrapolated players
  if ( !cg_smoothClients.integer ) {
    // make sure the clients use TR_INTERPOLATE
    if ( cent->currentState.number < MAX_CLIENTS ) {
      cent->currentState.pos.trType = TR_INTERPOLATE;
      cent->nextState.pos.trType = TR_INTERPOLATE;
    }
  }

  if ( cent->interpolate && cent->currentState.pos.trType == TR_INTERPOLATE ) {
    CG_InterpolateEntityPosition( cent );
    return;
  }

  // first see if we can interpolate between two snaps for
  // linear extrapolated clients
  if ( cent->interpolate && cent->currentState.pos.trType == TR_LINEAR_STOP &&
                      cent->currentState.number < MAX_CLIENTS) {
    CG_InterpolateEntityPosition( cent );
    return;
  }

  // just use the current frame and evaluate as best we can
  BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
  BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

  // adjust for riding a mover if it wasn't rolled into the predicted
  // player state
  if ( cent != &cg.predictedPlayerEntity ) {
    CG_AdjustPositionForMover( cent->lerpOrigin, cent->currentState.groundEntityNum,
    cg.snap->serverTime, cg.time, cent->lerpOrigin );
  }
}



/*
===============
CG_AddCEntity

===============
*/
static void CG_AddCEntity( centity_t *cent ) {
  // event-only entities will have been dealt with already
  if ( cent->currentState.eType >= ET_EVENTS ) {
    return;
  }
  
  // calculate the current origin
  CG_CalcEntityLerpPositions( cent );

  // add automatic effects
  CG_EntityEffects( cent );

  switch ( cent->currentState.eType ) {
  default:
    CG_Error( "Bad entity type: %i\n", cent->currentState.eType );
    break;
  case ET_INVISIBLE:
  case ET_PUSH_TRIGGER:
  case ET_TELEPORT_TRIGGER:
    break;
  case ET_GENERAL:
    CG_General( cent );
    break;
  case ET_CORPSE:
    CG_Corpse( cent );
    break;
  case ET_PLAYER:
    CG_Player( cent );
    break;
  case ET_BUILDABLE:
    CG_Buildable( cent );
    break;
  case ET_MISSILE:
    CG_Missile( cent );
    break;
  case ET_MOVER:
    CG_Mover( cent );
    break;
  case ET_BEAM:
    CG_Beam( cent );
    break;
  case ET_PORTAL:
    CG_Portal( cent );
    break;
  case ET_SPEAKER:
    CG_Speaker( cent );
    break;
  case ET_GRAPPLE:
    CG_Grapple( cent );
    break;
  case ET_SPRITER:
    CG_Spriter( cent );
    break;
  case ET_ANIMMAPOBJ:
    CG_animMapObj( cent );
    break;
  case ET_LIGHTFLARE:
    CG_LightFlare( cent );
    break;
  }
}

/*
===============
CG_AddPacketEntities

===============
*/
void CG_AddPacketEntities( void ) {
  int         num;
  centity_t     *cent;
  playerState_t   *ps;

  // set cg.frameInterpolation
  if ( cg.nextSnap ) {
    int   delta;

    delta = (cg.nextSnap->serverTime - cg.snap->serverTime);
    if ( delta == 0 ) {
      cg.frameInterpolation = 0;
    } else {
      cg.frameInterpolation = (float)( cg.time - cg.snap->serverTime ) / delta;
    }
  } else {
    cg.frameInterpolation = 0;  // actually, it should never be used, because
                  // no entities should be marked as interpolating
  }

  // the auto-rotating items will all have the same axis
  cg.autoAngles[0] = 0;
  cg.autoAngles[1] = ( cg.time & 2047 ) * 360 / 2048.0;
  cg.autoAngles[2] = 0;

  cg.autoAnglesFast[0] = 0;
  cg.autoAnglesFast[1] = ( cg.time & 1023 ) * 360 / 1024.0f;
  cg.autoAnglesFast[2] = 0;

  AnglesToAxis( cg.autoAngles, cg.autoAxis );
  AnglesToAxis( cg.autoAnglesFast, cg.autoAxisFast );

  // generate and add the entity from the playerstate
  ps = &cg.predictedPlayerState;
  BG_PlayerStateToEntityState( ps, &cg.predictedPlayerEntity.currentState, qfalse );
  CG_AddCEntity( &cg.predictedPlayerEntity );

  // lerp the non-predicted value for lightning gun origins
  CG_CalcEntityLerpPositions( &cg_entities[ cg.snap->ps.clientNum ] );

  //TA: "empty" item position arrays
  cg.ep.numAlienBuildables = 0;
  cg.ep.numHumanBuildables = 0;
  cg.ep.numAlienClients = 0;
  cg.ep.numHumanClients = 0;

  for( num = 0 ; num < cg.snap->numEntities ; num++ )
  {
    cent = &cg_entities[ cg.snap->entities[ num ].number ];
    
    if( cent->currentState.eType == ET_BUILDABLE )
    {
      //TA: add to list of item positions (for creep)
      if( cent->currentState.modelindex2 == BIT_ALIENS )
      {
        VectorCopy( cent->lerpOrigin, cg.ep.alienBuildablePos[ cg.ep.numAlienBuildables ] );
        cg.ep.alienBuildableTimes[ cg.ep.numAlienBuildables ] = cent->miscTime;
        cg.ep.numAlienBuildables++;
      }
      else if( cent->currentState.modelindex2 == BIT_HUMANS )
      {
        VectorCopy( cent->lerpOrigin, cg.ep.humanBuildablePos[ cg.ep.numHumanBuildables ] );
        cg.ep.numHumanBuildables++;
      }
    }
    else if( cent->currentState.eType == ET_PLAYER )
    {
      int team = cent->currentState.powerups & 0x00FF;

      if( team == PTE_ALIENS )
      {
        VectorCopy( cent->lerpOrigin, cg.ep.alienClientPos[ cg.ep.numAlienClients ] );
        cg.ep.numAlienClients++;
      }
      else if( team == PTE_HUMANS )
      {
        VectorCopy( cent->lerpOrigin, cg.ep.humanClientPos[ cg.ep.numHumanClients ] );
        cg.ep.numHumanClients++;
      }
    }
  }

  //Com_Printf( "%d %d\n", cgIP.numAlienClients, cgIP.numHumanClients );

  // add each entity sent over by the server
  for( num = 0; num < cg.snap->numEntities; num++ )
  {
    cent = &cg_entities[ cg.snap->entities[ num ].number ];
    CG_AddCEntity( cent );
  }
}

