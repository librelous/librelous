// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_weapons.c -- events and effects dealing with weapons

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
==========================
CG_MachineGunEjectBrass
==========================
*/
static void CG_MachineGunEjectBrass( centity_t *cent ) {
  localEntity_t *le;
  refEntity_t   *re;
  vec3_t      velocity, xvelocity;
  vec3_t      offset, xoffset;
  float     waterScale = 1.0f;
  vec3_t      v[3];

  if ( cg_brassTime.integer <= 0 ) {
    return;
  }

  le = CG_AllocLocalEntity();
  re = &le->refEntity;

  velocity[0] = 0;
  velocity[1] = -50 + 40 * crandom();
  velocity[2] = 100 + 50 * crandom();

  le->leType = LE_FRAGMENT;
  le->startTime = cg.time;
  le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();

  le->pos.trType = TR_GRAVITY;
  le->pos.trTime = cg.time - (rand()&15);

  AnglesToAxis( cent->lerpAngles, v );

  offset[0] = 8;
  offset[1] = -4;
  offset[2] = 24;

  xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
  xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
  xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
  VectorAdd( cent->lerpOrigin, xoffset, re->origin );

  VectorCopy( re->origin, le->pos.trBase );

  if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
    waterScale = 0.10f;
  }

  xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
  xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
  xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
  VectorScale( xvelocity, waterScale, le->pos.trDelta );

  AxisCopy( axisDefault, re->axis );
  re->hModel = cgs.media.machinegunBrassModel;

  le->bounceFactor = 0.4 * waterScale;

  le->angles.trType = TR_LINEAR;
  le->angles.trTime = cg.time;
  le->angles.trBase[0] = rand()&31;
  le->angles.trBase[1] = rand()&31;
  le->angles.trBase[2] = rand()&31;
  le->angles.trDelta[0] = 2;
  le->angles.trDelta[1] = 1;
  le->angles.trDelta[2] = 0;

  le->leFlags = LEF_TUMBLE;
  le->leBounceSoundType = LEBS_BRASS;
  le->leMarkType = LEMT_NONE;
}

/*
==========================
CG_ShotgunEjectBrass
==========================
*/
static void CG_ShotgunEjectBrass( centity_t *cent ) {
  localEntity_t *le;
  refEntity_t   *re;
  vec3_t      velocity, xvelocity;
  vec3_t      offset, xoffset;
  vec3_t      v[3];
  int       i;

  if ( cg_brassTime.integer <= 0 ) {
    return;
  }

  for ( i = 0; i < 2; i++ ) {
    float waterScale = 1.0f;

    le = CG_AllocLocalEntity();
    re = &le->refEntity;

    velocity[0] = 60 + 60 * crandom();
    if ( i == 0 ) {
      velocity[1] = 40 + 10 * crandom();
    } else {
      velocity[1] = -40 + 10 * crandom();
    }
    velocity[2] = 100 + 50 * crandom();

    le->leType = LE_FRAGMENT;
    le->startTime = cg.time;
    le->endTime = le->startTime + cg_brassTime.integer*3 + cg_brassTime.integer * random();

    le->pos.trType = TR_GRAVITY;
    le->pos.trTime = cg.time;

    AnglesToAxis( cent->lerpAngles, v );

    offset[0] = 8;
    offset[1] = 0;
    offset[2] = 24;

    xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
    xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
    xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
    VectorAdd( cent->lerpOrigin, xoffset, re->origin );
    VectorCopy( re->origin, le->pos.trBase );
    if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
      waterScale = 0.10f;
    }

    xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
    xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
    xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
    VectorScale( xvelocity, waterScale, le->pos.trDelta );

    AxisCopy( axisDefault, re->axis );
    re->hModel = cgs.media.shotgunBrassModel;
    le->bounceFactor = 0.3f;

    le->angles.trType = TR_LINEAR;
    le->angles.trTime = cg.time;
    le->angles.trBase[0] = rand()&31;
    le->angles.trBase[1] = rand()&31;
    le->angles.trBase[2] = rand()&31;
    le->angles.trDelta[0] = 1;
    le->angles.trDelta[1] = 0.5;
    le->angles.trDelta[2] = 0;

    le->leFlags = LEF_TUMBLE;
    le->leBounceSoundType = LEBS_BRASS;
    le->leMarkType = LEMT_NONE;
  }
}


/*
==========================
CG_RailTrail
==========================
*/
void CG_RailTrail( vec3_t start, vec3_t end )
{
	vec3_t axis[36], move, move2, next_move, vec, temp;
	float  len;
	int    i, j, skip;
 
  localEntity_t *le;
  refEntity_t   *re;

#define RADIUS   4
#define ROTATION 1
#define SPACING  5
 
	start[2] -= 4;
	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);
	PerpendicularVector(temp, vec);
	for (i = 0 ; i < 36; i++) {
		RotatePointAroundVector(axis[i], vec, temp, i * 10);//banshee 2.4 was 10
	}
 
  le = CG_AllocLocalEntity();
  re = &le->refEntity;

  le->leType = LE_FADE_RGB;
  le->startTime = cg.time;
  le->endTime = cg.time + cg_railTrailTime.value;
  le->lifeRate = 1.0 / ( le->endTime - le->startTime );

  re->shaderTime = cg.time / 1000.0f;
  re->reType = RT_RAIL_CORE;
  re->customShader = cgs.media.railCoreShader;

  VectorCopy( start, re->origin );
  VectorCopy( end, re->oldorigin );

	re->shaderRGBA[0] = 255;
  re->shaderRGBA[1] = 255;
  re->shaderRGBA[2] = 255;
  re->shaderRGBA[3] = 255;

	le->color[0] = 0.75f;
	le->color[1] = 0.75f;
	le->color[2] = 0.75f;
  le->color[3] = 1.0f;

  AxisClear( re->axis );

	VectorMA(move, 20, vec, move);
	VectorCopy(move, next_move);
	VectorScale (vec, SPACING, vec);

	if (cg_oldRail.integer != 0) {
		// nudge down a bit so it isn't exactly in center
		re->origin[2] -= 8;
		re->oldorigin[2] -= 8;
		return;
	}
	skip = -1;
 
	j = 18;
    for (i = 0; i < len; i += SPACING) {
		if (i != skip) {
			skip = i + SPACING;
			le = CG_AllocLocalEntity();
            re = &le->refEntity;
            le->leFlags = LEF_PUFF_DONT_SCALE;
			le->leType = LE_MOVE_SCALE_FADE;
            le->startTime = cg.time;
            le->endTime = cg.time + (i>>1) + 600;
            le->lifeRate = 1.0 / (le->endTime - le->startTime);

            re->shaderTime = cg.time / 1000.0f;
            re->reType = RT_SPRITE;
            re->radius = 1.1f;
			re->customShader = cgs.media.railRingsShader;

            re->shaderRGBA[0] = 255;
            re->shaderRGBA[1] = 255;
            re->shaderRGBA[2] = 255;
            re->shaderRGBA[3] = 255;

            le->color[0] = 0.75f;
            le->color[1] = 0.75f;
            le->color[2] = 0.75f;
            le->color[3] = 1.0f;

            le->pos.trType = TR_LINEAR;
            le->pos.trTime = cg.time;

			VectorCopy( move, move2);
            VectorMA(move2, RADIUS , axis[j], move2);
            VectorCopy(move2, le->pos.trBase);

            le->pos.trDelta[0] = axis[j][0]*6;
            le->pos.trDelta[1] = axis[j][1]*6;
            le->pos.trDelta[2] = axis[j][2]*6;
		}

        VectorAdd (move, vec, move);

        j = j + ROTATION < 36 ? j + ROTATION : (j + ROTATION) % 36;
	}
}

/*
==========================
CG_TeslaTrail
==========================
*/
void CG_TeslaTrail( vec3_t start, vec3_t end )
{
  localEntity_t *le;
  refEntity_t   *re;

  start[ 2 ] += 12; //nudge up a bit so the bolt comes from the sphere

  //add a bunch of bolt segments
  le = CG_AllocLocalEntity();
  re = &le->refEntity;

  le->leType = LE_FADE_RGB;
  le->startTime = cg.time;
  le->endTime = cg.time + cg_teslaTrailTime.value;
  le->lifeRate = 1.0 / ( le->endTime - le->startTime );

  re->shaderTime = cg.time / 1000.0f;
  re->reType = RT_RAIL_CORE;
  re->customShader = cgs.media.lightningShader;

  VectorCopy( start, re->origin );
  VectorCopy( end, re->oldorigin );

  re->shaderRGBA[0] = 255;
  re->shaderRGBA[1] = 255;
  re->shaderRGBA[2] = 255;
  re->shaderRGBA[3] = 255;

  le->color[0] = 1.0f;
  le->color[1] = 1.0f;
  le->color[2] = 1.0f;
  le->color[3] = 1.0f;

  AxisClear( re->axis );
}

/*
==========================
CG_RocketTrail
==========================
*/
static void CG_RocketTrail( centity_t *ent, const weaponInfo_t *wi ) {
  int   step;
  vec3_t  origin, lastPos;
  int   t;
  int   startTime, contents;
  int   lastContents;
  entityState_t *es;
  vec3_t  up;
  localEntity_t *smoke;

  if ( cg_noProjectileTrail.integer ) {
    return;
  }
  
  up[0] = 0;
  up[1] = 0;
  up[2] = 0;

  step = 50;

  es = &ent->currentState;
  startTime = ent->trailTime;
  t = step * ( (startTime + step) / step );

  BG_EvaluateTrajectory( &es->pos, cg.time, origin );
  contents = CG_PointContents( origin, -1 );

  // if object (e.g. grenade) is stationary, don't toss up smoke
  if ( es->pos.trType == TR_STATIONARY ) {
    ent->trailTime = cg.time;
    return;
  }

  BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
  lastContents = CG_PointContents( lastPos, -1 );

  ent->trailTime = cg.time;

  if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
    if ( contents & lastContents & CONTENTS_WATER ) {
      CG_BubbleTrail( lastPos, origin, 8 );
    }
    return;
  }

  for ( ; t <= ent->trailTime ; t += step ) {
    BG_EvaluateTrajectory( &es->pos, t, lastPos );

    smoke = CG_SmokePuff( lastPos, up,
            wi->trailRadius,
            1, 1, 1, 0.33f,
            wi->wiTrailTime,
            t,
            0,
            0,
            cgs.media.smokePuffShader );
    // use the optimized local entity add
    smoke->leType = LE_SCALE_FADE;
  }

}

/*
==========================
CG_PlasmaTrail
==========================
*/
static void CG_PlasmaTrail( centity_t *cent, const weaponInfo_t *wi ) {
	localEntity_t	*le;
	refEntity_t		*re;
	entityState_t	*es;
	vec3_t			velocity, xvelocity, origin;
	vec3_t			offset, xoffset;
	vec3_t			v[3];
	int				t, startTime, step;

	float	waterScale = 1.0f;

	if ( cg_noProjectileTrail.integer || cg_oldPlasma.integer ) {
		return;
	}

	step = 50;

	es = &cent->currentState;
	startTime = cent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 60 - 120 * crandom();
	velocity[1] = 40 - 80 * crandom();
	velocity[2] = 100 - 200 * crandom();

	le->leType = LE_MOVE_SCALE_FADE;
	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;

	le->startTime = cg.time;
	le->endTime = le->startTime + 600;

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time;

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 2;
	offset[1] = 2;
	offset[2] = 2;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];

	VectorAdd( origin, xoffset, re->origin );
	VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_SPRITE;
    re->radius = 0.25f;
	re->customShader = cgs.media.railRingsShader;
	le->bounceFactor = 0.3f;

    re->shaderRGBA[0] = wi->flashDlightColor[0] * 63;
    re->shaderRGBA[1] = wi->flashDlightColor[1] * 63;
    re->shaderRGBA[2] = wi->flashDlightColor[2] * 63;
    re->shaderRGBA[3] = 63;

    le->color[0] = wi->flashDlightColor[0] * 0.2;
    le->color[1] = wi->flashDlightColor[1] * 0.2;
    le->color[2] = wi->flashDlightColor[2] * 0.2;
    le->color[3] = 0.25f;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 1;
	le->angles.trDelta[1] = 0.5;
	le->angles.trDelta[2] = 0;

}

/*
==========================
CG_GrappleTrail
==========================
*/
void CG_GrappleTrail( centity_t *ent, const weaponInfo_t *wi ) {
  vec3_t  origin;
  entityState_t *es;
  vec3_t      forward, up;
  refEntity_t   beam;

  es = &ent->currentState;

  BG_EvaluateTrajectory( &es->pos, cg.time, origin );
  ent->trailTime = cg.time;

  memset( &beam, 0, sizeof( beam ) );
  //FIXME adjust for muzzle position
  VectorCopy ( cg_entities[ ent->currentState.otherEntityNum ].lerpOrigin, beam.origin );
  beam.origin[2] += 26;
  AngleVectors( cg_entities[ ent->currentState.otherEntityNum ].lerpAngles, forward, NULL, up );
  VectorMA( beam.origin, -6, up, beam.origin );
  VectorCopy( origin, beam.oldorigin );

  if (Distance( beam.origin, beam.oldorigin ) < 64 )
    return; // Don't draw if close

  beam.reType = RT_LIGHTNING;
  beam.customShader = cgs.media.lightningShader;

  AxisClear( beam.axis );
  beam.shaderRGBA[0] = 0xff;
  beam.shaderRGBA[1] = 0xff;
  beam.shaderRGBA[2] = 0xff;
  beam.shaderRGBA[3] = 0xff;
  trap_R_AddRefEntityToScene( &beam );
}

/*
==========================
CG_GrenadeTrail
==========================
*/
static void CG_GrenadeTrail( centity_t *ent, const weaponInfo_t *wi ) {
  CG_RocketTrail( ent, wi );
}

/*
=================
CG_RegisterUpgrade

The server says this item is used on this level
=================
*/
void CG_RegisterUpgrade( int upgradeNum )
{
  upgradeInfo_t   *upgradeInfo;
  char            path[MAX_QPATH];
  int             i;
  char            *icon;

  upgradeInfo = &cg_upgrades[ upgradeNum ];

  if( upgradeNum == 0 )
    return;

  if( upgradeInfo->registered )
    return;

  memset( upgradeInfo, 0, sizeof( *upgradeInfo ) );
  upgradeInfo->registered = qtrue;

  if( !BG_FindNameForUpgrade( upgradeNum ) )
    CG_Error( "Couldn't find upgrade %i", upgradeNum );
  
  upgradeInfo->humanName = BG_FindHumanNameForUpgrade( upgradeNum );
  
  if( icon = BG_FindIconForUpgrade( upgradeNum ) )
    upgradeInfo->upgradeIcon = trap_R_RegisterShader( icon );
}

/*
===============
CG_InitUpgrades

Precaches upgrades
===============
*/
void CG_InitUpgrades( )
{
  int   i;

  memset( cg_upgrades, 0, sizeof( cg_upgrades ) );

  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    CG_RegisterUpgrade( i );
}

/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/
void CG_RegisterWeapon( int weaponNum )
{
  weaponInfo_t  *weaponInfo;
  char          path[MAX_QPATH];
  vec3_t        mins, maxs;
  int           i;
  char          *icon, *model;

  weaponInfo = &cg_weapons[ weaponNum ];

  if( weaponNum == 0 )
    return;

  if( weaponInfo->registered )
    return;

  memset( weaponInfo, 0, sizeof( *weaponInfo ) );
  weaponInfo->registered = qtrue;

  if( !BG_FindNameForWeapon( weaponNum ) )
    CG_Error( "Couldn't find weapon %i", weaponNum );
  
  weaponInfo->humanName = BG_FindHumanNameForWeapon( weaponNum );

  // load cmodel before model so filecache works
  if( model = BG_FindModelsForWeapon( weaponNum, 0 ) )
    weaponInfo->weaponModel = trap_R_RegisterModel( model );

  // calc midpoint for rotation
  trap_R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
  for( i = 0 ; i < 3 ; i++ )
    weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );

  if( icon = BG_FindIconForWeapon( weaponNum ) )
  {
    weaponInfo->weaponIcon = trap_R_RegisterShader( icon );
    weaponInfo->ammoIcon = trap_R_RegisterShader( icon );
  }
  
  strcpy( path, model );
  COM_StripExtension( path, path );
  strcat( path, "_flash.md3" );
  weaponInfo->flashModel = trap_R_RegisterModel( path );

  strcpy( path, model );
  COM_StripExtension( path, path );
  strcat( path, "_barrel.md3" );
  weaponInfo->barrelModel = trap_R_RegisterModel( path );

  strcpy( path, model );
  COM_StripExtension( path, path );
  strcat( path, "_hand.md3" );
  weaponInfo->handsModel = trap_R_RegisterModel( path );

  if ( !weaponInfo->handsModel ) {
    weaponInfo->handsModel = trap_R_RegisterModel( "models/weapons2/shotgun/shotgun_hand.md3" );
  }

  weaponInfo->loopFireSound = qfalse;

  switch( weaponNum )
  {
    case WP_TESLAGEN:
    case WP_AREA_ZAP:
    case WP_DIRECT_ZAP:
      MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
      weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/melee/fsthum.wav", qfalse );
      weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/lightning/lg_hum.wav", qfalse );

      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
      cgs.media.lightningShader = trap_R_RegisterShader( "models/ammo/tesla/tesla_bolt");
      cgs.media.lightningExplosionModel = trap_R_RegisterModel( "models/weaphits/crackle.md3" );
      cgs.media.sfx_lghit = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
      break;

    case WP_MACHINEGUN:
      MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf1b.wav", qfalse );
      weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf2b.wav", qfalse );
      weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
      weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
      weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
      cgs.media.bulletExplosionShader = trap_R_RegisterShader( "bulletExplosion" );
      break;

    case WP_MASS_DRIVER:
      MAKERGB( weaponInfo->flashDlightColor, 0, 0, 1 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf1b.wav", qfalse );
      weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf2b.wav", qfalse );
      weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
      weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
      break;

    case WP_CHAINGUN:
      MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf1b.wav", qfalse );
      weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf2b.wav", qfalse );
      weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
      weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
      weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
      cgs.media.bulletExplosionShader = trap_R_RegisterShader( "bulletExplosion" );
      break;
      
    case WP_LOCKBLOB_LAUNCHER:
      weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/trapper/trapper.md3" );
  /*    weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
      weaponInfo->missileTrailFunc = CG_RocketTrail;
      weaponInfo->missileDlight = 200;
      weaponInfo->wiTrailTime = 2000;
      weaponInfo->trailRadius = 64;
      MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
      MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );*/
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
      /*cgs.media.rocketExplosionShader = trap_R_RegisterShader( "rocketExplosion" );*/
      break;

    case WP_FLAMER:
      weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/flamer/fireloop.wav", qfalse );
      MAKERGB( weaponInfo->flashDlightColor, 0.25, 0.1, 0 );
      //weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/railgun/railgf1a.wav", qfalse );
      /*cgs.media.flameExplShader = trap_R_RegisterShader( "rocketExplosion" );*/
      break;
      
    case WP_PLASMAGUN:
      weaponInfo->missileTrailFunc = CG_PlasmaTrail;
      weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/plasma/lasfly.wav", qfalse );
      MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
      cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "plasmaExplosion" );
      cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
      break;

    case WP_PULSE_RIFLE:
      weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/plasma/lasfly.wav", qfalse );
      MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
      cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "plasmaExplosion" );
      cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
      break;

    case WP_LUCIFER_CANON:
      weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
      MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
      cgs.media.bfgExplosionShader = trap_R_RegisterShader( "bfgExplosion" );
      weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/bfg.md3" );
      weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
      break;

    case WP_VENOM:
      MAKERGB( weaponInfo->flashDlightColor, 0, 0, 0 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav", qfalse );
      break;

    case WP_GRAB_CLAW:
      MAKERGB( weaponInfo->flashDlightColor, 0, 0, 0 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav", qfalse );
      break;

    case WP_POUNCE:
      MAKERGB( weaponInfo->flashDlightColor, 0, 0, 0 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav", qfalse );
      break;

    case WP_ABUILD:
    case WP_ABUILD2:
    case WP_HBUILD:
    case WP_HBUILD2:
      //nowt
      break;

    default:
      MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
      weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
      break;
  }
}

/*
===============
CG_InitWeapons

Precaches weapons
===============
*/
void CG_InitWeapons( )
{
  int   i;

  memset( cg_weapons, 0, sizeof( cg_weapons ) );

  for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    CG_RegisterWeapon( i );
}


/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

/*
=================
CG_MapTorsoToWeaponFrame

=================
*/
static int CG_MapTorsoToWeaponFrame( clientInfo_t *ci, int frame ) {

  // change weapon
  if ( frame >= ci->animations[TORSO_DROP].firstFrame
    && frame < ci->animations[TORSO_DROP].firstFrame + 9 ) {
    return frame - ci->animations[TORSO_DROP].firstFrame + 6;
  }

  // stand attack
  if ( frame >= ci->animations[TORSO_ATTACK].firstFrame
    && frame < ci->animations[TORSO_ATTACK].firstFrame + 6 ) {
    return 1 + frame - ci->animations[TORSO_ATTACK].firstFrame;
  }

  // stand attack 2
  if ( frame >= ci->animations[TORSO_ATTACK2].firstFrame
    && frame < ci->animations[TORSO_ATTACK2].firstFrame + 6 ) {
    return 1 + frame - ci->animations[TORSO_ATTACK].firstFrame;
  }

  return 0;
}


/*
==============
CG_CalculateWeaponPosition
==============
*/
static void CG_CalculateWeaponPosition( vec3_t origin, vec3_t angles ) {
  float scale;
  int   delta;
  float fracsin;
  float bob;

  VectorCopy( cg.refdef.vieworg, origin );
  VectorCopy( cg.refdefViewAngles, angles );

  // on odd legs, invert some angles
  if ( cg.bobcycle & 1 ) {
    scale = -cg.xyspeed;
  } else {
    scale = cg.xyspeed;
  }

  // gun angles from bobbing
  //TA: bob amount is class dependant
  bob = BG_FindBobForClass( cg.predictedPlayerState.stats[ STAT_PCLASS ] );
  if( bob != 0 )
  {
    angles[ROLL] += scale * cg.bobfracsin * 0.005;
    angles[YAW] += scale * cg.bobfracsin * 0.01;
    angles[PITCH] += cg.xyspeed * cg.bobfracsin * 0.005;
  }

  // drop the weapon when landing
  if( !BG_ClassHasAbility( cg.predictedPlayerState.stats[ STAT_PCLASS ], SCA_NOWEAPONDRIFT ) )
  {
    delta = cg.time - cg.landTime;
    if ( delta < LAND_DEFLECT_TIME )
    {
      origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
    }
    else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME )
    {
      origin[2] += cg.landChange*0.25 *
        (LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
    }

  //TA: huh? why drop the weapon when stair climbing... just as well this isn't actually used :)
#if 0
  // drop the weapon when stair climbing
  delta = cg.time - cg.stepTime;
  if ( delta < STEP_TIME/2 ) {
    origin[2] -= cg.stepChange*0.25 * delta / (STEP_TIME/2);
  } else if ( delta < STEP_TIME ) {
    origin[2] -= cg.stepChange*0.25 * (STEP_TIME - delta) / (STEP_TIME/2);
  }
#endif

  // idle drift
    scale = cg.xyspeed + 40;
    fracsin = sin( cg.time * 0.001 );
    angles[ROLL] += scale * fracsin * 0.01;
    angles[YAW] += scale * fracsin * 0.01;
    angles[PITCH] += scale * fracsin * 0.01;
  }
}


/*
===============
CG_LightningBolt

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
The cent should be the non-predicted cent if it is from the player,
so the endpoint will reflect the simulated strike (lagging the predicted
angle)
===============
*/
static void CG_LightningBolt( centity_t *cent, vec3_t origin ) {
  trace_t   trace;
  refEntity_t   beam;
  vec3_t      forward;
  vec3_t      muzzlePoint, endPoint;

  if ( cent->currentState.weapon != WP_TESLAGEN ) {
    return;
  }

  memset( &beam, 0, sizeof( beam ) );

	// CPMA  "true" lightning
	if ((cent->currentState.number == cg.predictedPlayerState.clientNum) && (cg_trueLightning.value != 0)) {
		vec3_t angle;
		int i;

		for (i = 0; i < 3; i++) {
			float a = cent->lerpAngles[i] - cg.refdefViewAngles[i];
			if (a > 180) {
				a -= 360;
			}
			if (a < -180) {
				a += 360;
			}

			angle[i] = cg.refdefViewAngles[i] + a * (1.0 - cg_trueLightning.value);
			if (angle[i] < 0) {
				angle[i] += 360;
			}
			if (angle[i] > 360) {
				angle[i] -= 360;
			}
		}

		AngleVectors(angle, forward, NULL, NULL );
		VectorCopy(cent->lerpOrigin, muzzlePoint );
//		VectorCopy(cg.refdef.vieworg, muzzlePoint );
	} else {
		// !CPMA
		AngleVectors( cent->lerpAngles, forward, NULL, NULL );
		VectorCopy(cent->lerpOrigin, muzzlePoint );
	}

  // FIXME: crouch
  muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

  VectorMA( muzzlePoint, 14, forward, muzzlePoint );

  // project forward by the lightning range
  VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

  // see if it hit a wall
  CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint,
    cent->currentState.number, MASK_SHOT );

  // this is the endpoint
  VectorCopy( trace.endpos, beam.oldorigin );

  // use the provided origin, even though it may be slightly
  // different than the muzzle origin
  VectorCopy( origin, beam.origin );

  beam.reType = RT_LIGHTNING;
  beam.customShader = cgs.media.lightningShader;
  trap_R_AddRefEntityToScene( &beam );

  // add the impact flare if it hit something
  if ( trace.fraction < 1.0 ) {
    vec3_t  angles;
    vec3_t  dir;

    VectorSubtract( beam.oldorigin, beam.origin, dir );
    VectorNormalize( dir );

    memset( &beam, 0, sizeof( beam ) );
    beam.hModel = cgs.media.lightningExplosionModel;

    VectorMA( trace.endpos, -16, dir, beam.origin );

    // make a random orientation
    angles[0] = rand() % 360;
    angles[1] = rand() % 360;
    angles[2] = rand() % 360;
    AnglesToAxis( angles, beam.axis );
    trap_R_AddRefEntityToScene( &beam );
  }
}


/*
===============
CG_FlameTrail
===============
*/
static void CG_FlameTrail( centity_t *cent, vec3_t origin )
{
  vec3_t  forward, right, up;
  vec3_t  muzzlePoint;
  vec3_t  velocity;
  vec3_t  pVelocity;

  if( cent->currentState.weapon != WP_FLAMER )
    return;

  //not time for the next ball yet
  if( cg.time < cent->flamerTime )
    return;
  
  if( cent->currentState.clientNum == cg.predictedPlayerState.clientNum && !cg.renderingThirdPerson )
  {
    AngleVectors( cg.refdefViewAngles, forward, right, up );
    VectorCopy( cg.refdef.vieworg, muzzlePoint );
    VectorScale( cg.predictedPlayerState.velocity, FIREBALL_LAG, pVelocity );
  }
  else
  {
    AngleVectors( cent->lerpAngles, forward, right, up );
    VectorCopy( cent->lerpOrigin, muzzlePoint );

    //FIXME: this is gonna look weird when crouching
    muzzlePoint[ 2 ] += DEFAULT_VIEWHEIGHT;
    VectorScale( cent->currentState.pos.trDelta, FIREBALL_LAG, pVelocity );
  }
  
  VectorMA( pVelocity, FIREBALL_SPEED, forward, velocity );

  //FIXME: tweak these numbers when (if?) the flamer model is done
  VectorMA( muzzlePoint, 24.0f, forward, muzzlePoint );
  VectorMA( muzzlePoint, 6.0f, right, muzzlePoint );
  VectorMA( muzzlePoint, -6.0f, up, muzzlePoint );
  
  CG_LaunchSprite( muzzlePoint, velocity, vec3_origin,
                   0.1f, 4.0f, 40.0f, 255.0f, 255.0f,
                   rand( ) % 360, cg.time, FIREBALL_LIFETIME,
                   cgs.media.flameShader[ 0 ], qfalse, qfalse );

  //set next ball time
  cent->flamerTime = cg.time + FIREBALL_GAP;
}


/*
===============
CG_SpawnRailTrail

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
===============
*/
static void CG_SpawnRailTrail( centity_t *cent, vec3_t origin )
{
/*  if ( cent->currentState.weapon != WP_RAILGUN )
    return;

  if ( !cent->pe.railgunFlash )
    return;
 
  cent->pe.railgunFlash = qtrue;
  CG_RailTrail( origin, cent->pe.railgunImpact );*/
}


/*
======================
CG_MachinegunSpinAngle
======================
*/
#define   SPIN_SPEED  0.9
#define   COAST_TIME  1000
static float  CG_MachinegunSpinAngle( centity_t *cent ) {
  int   delta;
  float angle;
  float speed;

  delta = cg.time - cent->pe.barrelTime;
  if ( cent->pe.barrelSpinning ) {
    angle = cent->pe.barrelAngle + delta * SPIN_SPEED;
  } else {
    if ( delta > COAST_TIME ) {
      delta = COAST_TIME;
    }

    speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
    angle = cent->pe.barrelAngle + delta * speed;
  }

  if ( cent->pe.barrelSpinning == !(cent->currentState.eFlags & EF_FIRING) ) {
    cent->pe.barrelTime = cg.time;
    cent->pe.barrelAngle = AngleMod( angle );
    cent->pe.barrelSpinning = !!(cent->currentState.eFlags & EF_FIRING);
  }

  return angle;
}


/*
========================
CG_AddWeaponWithPowerups
========================
*/
static void CG_AddWeaponWithPowerups( refEntity_t *gun, int powerups ) {
  // add powerup effects
  /*if ( powerups & ( 1 << PW_INVIS ) ) {
    gun->customShader = cgs.media.invisShader;
    trap_R_AddRefEntityToScene( gun );
  } else*/ {
    trap_R_AddRefEntityToScene( gun );

    /*if ( powerups & ( 1 << PW_BATTLESUIT ) ) {
      gun->customShader = cgs.media.battleWeaponShader;
      trap_R_AddRefEntityToScene( gun );
    }
    if ( powerups & ( 1 << PW_QUAD ) ) {
      gun->customShader = cgs.media.quadWeaponShader;
      trap_R_AddRefEntityToScene( gun );
    }*/
  }
}


/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent ) {
  refEntity_t gun;
  refEntity_t barrel;
  refEntity_t flash;
  vec3_t    angles;
  weapon_t  weaponNum;
  weaponInfo_t  *weapon;
  centity_t *nonPredictedCent;

  weaponNum = cent->currentState.weapon;

  CG_RegisterWeapon( weaponNum );
  weapon = &cg_weapons[weaponNum];

  // add the weapon
  memset( &gun, 0, sizeof( gun ) );
  VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
  gun.shadowPlane = parent->shadowPlane;
  gun.renderfx = parent->renderfx;

  // set custom shading for railgun refire rate
  if ( ps )
  {
/*    if ( cg.predictedPlayerState.weapon == WP_RAILGUN
      && cg.predictedPlayerState.weaponstate == WEAPON_FIRING )
    {
      float f;

      f = (float)cg.predictedPlayerState.weaponTime / 1500;
      gun.shaderRGBA[1] = 0;
      gun.shaderRGBA[0] =
      gun.shaderRGBA[2] = 255 * ( 1.0 - f );
    }
    else*/
    {
      gun.shaderRGBA[0] = 255;
      gun.shaderRGBA[1] = 255;
      gun.shaderRGBA[2] = 255;
      gun.shaderRGBA[3] = 255;
    }

    //set weapon[1/2]Time when respective buttons change state
    if( cg.weapon1Firing != ( cg.predictedPlayerState.eFlags & EF_FIRING ) )
    {
      cg.weapon1Time = cg.time;
      cg.weapon1Firing = ( cg.predictedPlayerState.eFlags & EF_FIRING );
    }
    
    if( cg.weapon2Firing != ( cg.predictedPlayerState.eFlags & EF_FIRING2 ) )
    {
      cg.weapon2Time = cg.time;
      cg.weapon2Firing = ( cg.predictedPlayerState.eFlags & EF_FIRING2 );
    }
  }

  gun.hModel = weapon->weaponModel;
  if (!gun.hModel) {
    return;
  }

  if ( !ps ) {
    // add weapon ready sound
    cent->pe.lightningFiring = qfalse;
    if ( ( cent->currentState.eFlags & EF_FIRING ) && weapon->firingSound ) {
      // lightning gun and guantlet make a different sound when fire is held down
      trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->firingSound );
      cent->pe.lightningFiring = qtrue;
    } else if ( weapon->readySound ) {
      trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->readySound );
    }
  }

  CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_weapon");

  CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups );

  // add the spinning barrel
  if( weapon->barrelModel ) {
    memset( &barrel, 0, sizeof( barrel ) );
    VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
    barrel.shadowPlane = parent->shadowPlane;
    barrel.renderfx = parent->renderfx;

    barrel.hModel = weapon->barrelModel;
    angles[YAW] = 0;
    angles[PITCH] = 0;
    angles[ROLL] = CG_MachinegunSpinAngle( cent );
    AnglesToAxis( angles, barrel.axis );

    CG_PositionRotatedEntityOnTag( &barrel, &gun, weapon->weaponModel, "tag_barrel" );

    CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups );
  }

  // make sure we aren't looking at cg.predictedPlayerEntity for LG
  nonPredictedCent = &cg_entities[cent->currentState.clientNum];

  // if the index of the nonPredictedCent is not the same as the clientNum
  // then this is a fake player (like on teh single player podiums), so
  // go ahead and use the cent
  if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum ) {
    nonPredictedCent = cent;
  }

  // add the flash
  if ( ( weaponNum == WP_TESLAGEN || weaponNum == WP_FLAMER ) &&
       ( nonPredictedCent->currentState.eFlags & EF_FIRING ) )
  {
    // continuous flash
  } else {
    // impulse flash
    if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME && !cent->pe.railgunFlash ) {
      return;
    }
  }

  memset( &flash, 0, sizeof( flash ) );
  VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
  flash.shadowPlane = parent->shadowPlane;
  flash.renderfx = parent->renderfx;

  flash.hModel = weapon->flashModel;
  if (!flash.hModel) {
    return;
  }
  angles[YAW] = 0;
  angles[PITCH] = 0;
  angles[ROLL] = crandom() * 10;
  AnglesToAxis( angles, flash.axis );

  // colorize the railgun blast
/*  if ( weaponNum == WP_RAILGUN ) {
    clientInfo_t  *ci;

    ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    flash.shaderRGBA[0] = 255 * ci->color1[0];
    flash.shaderRGBA[1] = 255 * ci->color1[1];
    flash.shaderRGBA[2] = 255 * ci->color1[2];
  }*/

  CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_flash");
  trap_R_AddRefEntityToScene( &flash );

  if ( ps || cg.renderingThirdPerson ||
    cent->currentState.number != cg.predictedPlayerState.clientNum ) {
    // add lightning bolt
    CG_LightningBolt( nonPredictedCent, flash.origin );
    
    CG_FlameTrail( nonPredictedCent, flash.origin );

    // add rail trail
    CG_SpawnRailTrail( cent, flash.origin );

    // make a dlight for the flash
    if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] ) {
      trap_R_AddLightToScene( flash.origin, 300 + (rand()&31), weapon->flashDlightColor[0],
        weapon->flashDlightColor[1], weapon->flashDlightColor[2] );
    }
  }
}

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void CG_AddViewWeapon( playerState_t *ps ) {
  refEntity_t hand;
  centity_t *cent;
  clientInfo_t  *ci;
  float   fovOffset;
  vec3_t    angles;
  weaponInfo_t  *weapon;

  if( ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) ||
      ( ps->stats[ STAT_STATE ] & SS_INFESTING ) ||
      ( ps->stats[ STAT_STATE ] & SS_HOVELING ) )
    return;

  //TA: no weapon carried - can't draw it
  if( ps->weapon == WP_NONE )
    return;
      
  if ( ps->pm_type == PM_INTERMISSION ) {
    return;
  }

  //TA: draw a prospective buildable infront of the player
  if( ( ps->stats[ STAT_BUILDABLE ] & ~SB_VALID_TOGGLEBIT ) > BA_NONE )
    CG_GhostBuildable( ps->stats[ STAT_BUILDABLE ] & ~SB_VALID_TOGGLEBIT );

  // no gun if in third person view
  if ( cg.renderingThirdPerson ) {
    return;
  }

  // allow the gun to be completely removed
  if ( !cg_drawGun.integer ) {
    vec3_t    origin;

    if ( cg.predictedPlayerState.eFlags & EF_FIRING ) {
      // special hack for lightning gun...
      // TA: and flamer
      VectorCopy( cg.refdef.vieworg, origin );
      VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
      CG_LightningBolt( &cg_entities[ps->clientNum], origin );
      CG_FlameTrail( &cg_entities[ps->clientNum], origin );
    }
    return;
  }

  // don't draw if testing a gun model
  if ( cg.testGun ) {
    return;
  }

  // drop gun lower at higher fov
  //if ( cg_fov.integer > 90 ) {
  //TA: the client side variable isn't used ( shouldn't iD have done this anyway? )
  if( cg.refdef.fov_y > 90 )
    fovOffset = -0.4 * ( cg.refdef.fov_y - 90 );
  else
    fovOffset = 0;

  cent = &cg.predictedPlayerEntity; // &cg_entities[cg.snap->ps.clientNum];
  CG_RegisterWeapon( ps->weapon );
  weapon = &cg_weapons[ ps->weapon ];

  memset (&hand, 0, sizeof(hand));

  // set up gun position
  CG_CalculateWeaponPosition( hand.origin, angles );

  VectorMA( hand.origin, cg_gun_x.value, cg.refdef.viewaxis[0], hand.origin );
  VectorMA( hand.origin, cg_gun_y.value, cg.refdef.viewaxis[1], hand.origin );
  VectorMA( hand.origin, (cg_gun_z.value+fovOffset), cg.refdef.viewaxis[2], hand.origin );

  if( ps->weapon == WP_LUCIFER_CANON && ps->stats[ STAT_MISC ] > 0 )
  {
    float fraction = (float)ps->stats[ STAT_MISC ] / (float)LC_TOTAL_CHARGE;

    VectorMA( hand.origin, random( ) * fraction, cg.refdef.viewaxis[0], hand.origin );
    VectorMA( hand.origin, random( ) * fraction, cg.refdef.viewaxis[1], hand.origin );
  }
  
  AnglesToAxis( angles, hand.axis );

  // map torso animations to weapon animations
  if ( cg_gun_frame.integer ) {
    // development tool
    hand.frame = hand.oldframe = cg_gun_frame.integer;
    hand.backlerp = 0;
  } else {
    // get clientinfo for animation map
    ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    hand.frame = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.frame );
    hand.oldframe = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.oldFrame );
    hand.backlerp = cent->pe.torso.backlerp;
  }

  hand.hModel = weapon->handsModel;
  hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT;

  // add everything onto the hand
  CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity );
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/

qboolean haveWeapon( int *weapons, int num, int weapon )
{
  int i;

  for( i = 0; i < num; i++ )
  {
    if( weapons[ i ] == weapon )
      return qtrue;
  }

  return qfalse;
}

qboolean haveUpgrade( int *upgrades, int num, int upgrade )
{
  int i;

  for( i = 0; i < num; i++ )
  {
    if( upgrades[ i ] == upgrade )
      return qtrue;
  }

  return qfalse;
}

#define ICON_BORDER 4

/*
===================
CG_DrawWeaponSelect
===================
*/
void CG_DrawWeaponSelect( rectDef_t *rect )
{
  int i;
  int x = rect->x;
  int y = rect->y;
  int width = rect->w;
  int height = rect->h;
  int iconsize;
  int items[ 64 ];
  int numItems = 0, selectedItem;
  int length;
  int selectWindow;
  qboolean vertical;

  // don't display if dead
  if( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )
    return;

  // showing weapon select clears pickup item display, but not the blend blob
  cg.itemPickupTime = 0;

  if( height > width )
  {
    vertical = qtrue;
    iconsize = width;
    length = height / width;
  }
  else if( height <= width )
  {
    vertical = qfalse;
    iconsize = height;
    length = width / height;
  }
  
  selectWindow = length / 2;
  
  for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
  {
    if( !BG_gotWeapon( i, cg.snap->ps.stats ) )
      continue;

    if( i == cg.weaponSelect )
      selectedItem = numItems;

    CG_RegisterWeapon( i );
    items[ numItems ] = i;
    numItems++;
  }

  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( !BG_gotItem( i, cg.snap->ps.stats ) )
      continue;
    
    if( i == cg.weaponSelect - 32 )
      selectedItem = numItems;

    CG_RegisterUpgrade( i );
    items[ numItems ] = i + 32;
    numItems++;
  }

  for( i = 0; i < length; i++ )
  {
    int displacement = i - selectWindow;
    int item = displacement + selectedItem;
    
    if( ( item >= 0 ) && ( item < numItems ) )
    {
      if( items[ item ] <= 32 )
        CG_DrawPic( x + ICON_BORDER, y + ICON_BORDER, iconsize - 2 * ICON_BORDER, iconsize - 2 * ICON_BORDER,
                    cg_weapons[ items[ item ] ].weaponIcon );
      else if( items[ item ] > 32 )
        CG_DrawPic( x + ICON_BORDER, y + ICON_BORDER, iconsize - 2 * ICON_BORDER, iconsize - 2 * ICON_BORDER,
                    cg_upgrades[ items[ item ] - 32 ].upgradeIcon );

      if( displacement == 0 )
        CG_DrawPic( x, y, iconsize, iconsize, cgs.media.selectShader );
    }

    if( vertical )
      y += iconsize;
    else
      x += iconsize;
  }
  
  /*//TA: yuck! :)
  if( y == 10 )
  {
    trap_R_SetColor( NULL );
    return;
  }*/

}


/*
===================
CG_DrawWeaponSelectText
===================
*/
void CG_DrawWeaponSelectText( rectDef_t *rect, float scale, int textStyle )
{
  int x, w;
  char  *name;
  float *color;
  
  color = CG_FadeColor( cg.weaponSelectTime, WEAPON_SELECT_TIME );
  if( !color )
    return;
  
  trap_R_SetColor( color );
  
  // draw the selected name
  if( cg.weaponSelect <= 32 )
  {
    if( cg_weapons[ cg.weaponSelect ].registered )
    {
      if( name = cg_weapons[ cg.weaponSelect ].humanName )
      {
        w = CG_Text_Width( name, scale, 0 );
        x = rect->x + rect->w / 2;
        CG_Text_Paint( x - w / 2, rect->y + rect->h, scale, color, name, 0, 0, textStyle );
      }
    }
  }
  else if( cg.weaponSelect > 32 )
  {
    if( cg_upgrades[ cg.weaponSelect - 32 ].registered )
    {
      if( name = cg_upgrades[ cg.weaponSelect - 32 ].humanName )
      {
        w = CG_Text_Width( name, scale, 0 );
        x = rect->x + rect->w / 2;
        CG_Text_Paint( x - w / 2, rect->y + rect->h, scale, color, name, 0, 0, textStyle );
      }
    }
  }

  trap_R_SetColor( NULL );
}


/*
===============
CG_WeaponSelectable
===============
*/
static qboolean CG_WeaponSelectable( int i )
{
  int ammo, clips, maxclips;

  BG_unpackAmmoArray( i, cg.snap->ps.ammo, cg.snap->ps.powerups, &ammo, &clips, &maxclips );
  
  if ( !ammo && !clips && !BG_FindInfinteAmmoForWeapon( i ) ) {
    return qfalse;
  }
  if ( !BG_gotWeapon( i, cg.snap->ps.stats ) ) {
    return qfalse;
  }

  return qtrue;
}


/*
===============
CG_ItemSelectable
===============
*/
static qboolean CG_ItemSelectable( int i )
{
  if( !BG_gotItem( i, cg.snap->ps.stats ) ) {
    return qfalse;
  }

  return qtrue;
}


/*
===============
CG_NextWeapon_f
===============
*/
void CG_NextWeapon_f( void ) {
  int   i;
  int   original;

  if ( !cg.snap ) {
    return;
  }
  if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
    return;
  }

  cg.weaponSelectTime = cg.time;
  original = cg.weaponSelect;

  for ( i = 0 ; i < 64 ; i++ ) {
    cg.weaponSelect++;
    if ( cg.weaponSelect == 64 ) {
      cg.weaponSelect = 0;
    }
    /*if ( cg.weaponSelect == WP_GAUNTLET ) {
      continue;   // never cycle to gauntlet
    }*/

    if( cg.weaponSelect <= 32 )
    {
      if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
        break;
      }
    }
    else if( cg.weaponSelect > 32 )
    {
      if ( CG_ItemSelectable( cg.weaponSelect - 32 ) ) {
        break;
      }
    }
  }
  if ( i == 64 ) {
    cg.weaponSelect = original;
  }
}

/*
===============
CG_PrevWeapon_f
===============
*/
void CG_PrevWeapon_f( void ) {
  int   i;
  int   original;

  if ( !cg.snap ) {
    return;
  }
  if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
    return;
  }

  cg.weaponSelectTime = cg.time;
  original = cg.weaponSelect;

  for ( i = 0 ; i < 64 ; i++ ) {
    cg.weaponSelect--;
    if ( cg.weaponSelect == -1 ) {
      cg.weaponSelect = 63;
    }
    /*if ( cg.weaponSelect == WP_GAUNTLET ) {
      continue;   // never cycle to gauntlet
    }*/
    if( cg.weaponSelect <= 32 )
    {
      if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
        break;
      }
    }
    else if( cg.weaponSelect > 32 )
    {
      if ( CG_ItemSelectable( cg.weaponSelect - 32 ) ) {
        break;
      }
    }
  }
  if ( i == 64 ) {
    cg.weaponSelect = original;
  }
}

/*
===============
CG_Weapon_f
===============
*/
void CG_Weapon_f( void ) {
  int   num;

  if ( !cg.snap ) {
    return;
  }
  if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
    return;
  }

  num = atoi( CG_Argv( 1 ) );

  if ( num < 1 || num > 31 ) {
    return;
  }

  cg.weaponSelectTime = cg.time;

  if ( !BG_gotWeapon( num, cg.snap->ps.stats ) ) {
    return;   // don't have the weapon
  }

  cg.weaponSelect = num;
}

/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void CG_OutOfAmmoChange( void ) {
  int   i;

  //TA: mwhaha, must manually change weapons
  /*cg.weaponSelectTime = cg.time;

  for ( i = 31 ; i > 0 ; i-- ) {
    if ( CG_WeaponSelectable( i ) ) {
      cg.weaponSelect = i;
      break;
    }
  }*/
}



/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/

/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireWeapon( centity_t *cent ) {
  entityState_t *ent;
  int       c;
  weaponInfo_t  *weap;

  ent = &cent->currentState;
  if ( ent->weapon == WP_NONE ) {
    return;
  }
  if ( ent->weapon >= WP_NUM_WEAPONS ) {
    CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
    return;
  }
  weap = &cg_weapons[ ent->weapon ];

  // mark the entity as muzzle flashing, so when it is added it will
  // append the flash to the weapon model
  cent->muzzleFlashTime = cg.time;

  // lightning gun only does this this on initial press
  if ( ent->weapon == WP_TESLAGEN ) {
    if ( cent->pe.lightningFiring ) {
      return;
    }
  }

  // play quad sound if needed
  /*if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
    trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.media.quadSound );
  }*/

  // play a sound
  for ( c = 0 ; c < 4 ; c++ ) {
    if ( !weap->flashSound[c] ) {
      break;
    }
  }
  if ( c > 0 ) {
    c = rand() % c;
    if ( weap->flashSound[c] )
    {
      trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound[c] );
    }
  }

  // do brass ejection
  if ( weap->ejectBrassFunc && cg_brassTime.integer > 0 ) {
    weap->ejectBrassFunc( cent );
  }
}


/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing
=================
*/
void CG_MissileHitWall( int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType, int damage ) {
  qhandle_t   mod;
  qhandle_t   mark;
  qhandle_t   shader;
  sfxHandle_t   sfx;
  float     radius;
  float     light;
  vec3_t      lightColor;
  localEntity_t *le;
  int       r, i;
  qboolean    alphaFade;
  qboolean    isSprite;
  int       duration;
  vec3_t      sprOrg;
  vec3_t      sprVel;

  mark = 0;
  radius = 32;
  sfx = 0;
  mod = 0;
  shader = 0;
  light = 0;
  lightColor[0] = 1;
  lightColor[1] = 1;
  lightColor[2] = 0;

  // set defaults
  isSprite = qfalse;
  duration = 600;

  switch( weapon )
  {
  default:
  case WP_TESLAGEN:
  case WP_AREA_ZAP:
  case WP_DIRECT_ZAP:
    mod = cgs.media.lightningExplosionModel;
    shader = cgs.media.lightningShader;
    sfx = cgs.media.sfx_lghit;
    mark = cgs.media.energyMarkShader;
    radius = 24;
    break;
  case WP_LOCKBLOB_LAUNCHER:
    sfx = cgs.media.gibBounce1Sound;
    mark = cgs.media.greenBloodMarkShader;
    radius = 64;
    isSprite = qtrue;
    break;
  case WP_FLAMER:
    sfx = cgs.media.sfx_flamerexp;
    mark = cgs.media.burnMarkShader;
    radius = 32;
    break;
  case WP_PLASMAGUN:
    mod = cgs.media.ringFlashModel;
    shader = cgs.media.plasmaExplosionShader;
    sfx = cgs.media.sfx_plasmaexp;
    mark = cgs.media.energyMarkShader;
    radius = 16;
    break;
  case WP_PULSE_RIFLE:
    mod = cgs.media.ringFlashModel;
    shader = cgs.media.plasmaExplosionShader;
    sfx = cgs.media.sfx_plasmaexp;
    mark = cgs.media.energyMarkShader;
    radius = 16;
    break;
  case WP_MASS_DRIVER:
    shader = cgs.media.bulletExplosionShader;
    mark = cgs.media.bulletMarkShader;
    radius = 8;
    break;
  case WP_MACHINEGUN:
  case WP_CHAINGUN:
    mod = cgs.media.bulletFlashModel;
    shader = cgs.media.bulletExplosionShader;
    mark = cgs.media.bulletMarkShader;
    
    r = rand() & 3;
    if ( r == 0 ) {
      sfx = cgs.media.sfx_ric1;
    } else if ( r == 1 ) {
      sfx = cgs.media.sfx_ric2;
    } else {
      sfx = cgs.media.sfx_ric3;
    }

    radius = 8;
    break;
    
 #define LCANON_EJECTION_VEL 300
    
  case WP_LUCIFER_CANON:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.bfgExplosionShader;
    mark = cgs.media.bulletMarkShader;
    radius = 8;
    sfx = cgs.media.sfx_plasmaexp;
    isSprite = qtrue;
    
    for( i = 0; i <= damage / 20; i++ )
    {
      qhandle_t spark;
      vec3_t    velocity;
      vec3_t    accel = { 0.0f, 0.0f, -DEFAULT_GRAVITY };

      VectorMA( origin, 1.0f, dir, origin );

      if( random( ) > 0.5f )
        spark = cgs.media.gibSpark1;
      else
        spark = cgs.media.scannerBlipShader;

      velocity[ 0 ] = ( 2 * random( ) - 1.0f ) * LCANON_EJECTION_VEL;
      velocity[ 1 ] = ( 2 * random( ) - 1.0f ) * LCANON_EJECTION_VEL;
      velocity[ 2 ] = ( 2 * random( ) - 1.0f ) * LCANON_EJECTION_VEL;
      
      CG_LaunchSprite( origin, velocity, accel, 0.9, 1.0f, 40.0f, 255, 0, rand( ) % 360,
                       cg.time, 2000 + ( crandom( ) * 1000 ),
                       spark, qfalse, qfalse );
    }
    break;
  }

  if ( sfx ) {
    trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
  }

  //
  // create the explosion
  //
  if ( mod ) {
    le = CG_MakeExplosion( origin, dir,
                 mod, shader,
                 duration, isSprite );
    le->light = light;
    VectorCopy( lightColor, le->lightColor );
/*    if ( weapon == WP_RAILGUN ) {
      // colorize with client color
      VectorCopy( cgs.clientinfo[clientNum].color1, le->color );
    }*/
  }

  //
  // impact mark
  //
  // plasma fades alpha, all others fade color
  alphaFade = ( mark == cgs.media.energyMarkShader ||
                mark == cgs.media.greenBloodMarkShader );
  
/*  if( weapon == WP_RAILGUN )
  {
    float *color;

    // colorize with client color
    color = cgs.clientinfo[clientNum].color2;
    CG_ImpactMark( mark, origin, dir, random()*360, color[0],color[1], color[2],1, alphaFade, radius, qfalse );
  }
  else*/
  {
    CG_ImpactMark( mark, origin, dir, random()*360, 1,1,1,1, alphaFade, radius, qfalse );
  }
}


/*
=================
CG_MissileHitPlayer
=================
*/
void CG_MissileHitPlayer( int weapon, vec3_t origin, vec3_t dir, int entityNum, int damage ) {
  CG_Bleed( origin, entityNum );

  // some weapons will make an explosion with the blood, while
  // others will just make the blood
  switch ( weapon ) {
/*  case WP_GRENADE_LAUNCHER:
  case WP_ROCKET_LAUNCHER:
    CG_MissileHitWall( weapon, 0, origin, dir, IMPACTSOUND_FLESH );
    break;*/
  default:
    break;
  }
}



/*
============================================================================

SHOTGUN TRACING

============================================================================
*/

/*
================
CG_ShotgunPellet
================
*/
static void CG_ShotgunPellet( vec3_t start, vec3_t end, int skipNum ) {
  trace_t   tr;
  int sourceContentType, destContentType;

  CG_Trace( &tr, start, NULL, NULL, end, skipNum, MASK_SHOT );

  sourceContentType = trap_CM_PointContents( start, 0 );
  destContentType = trap_CM_PointContents( tr.endpos, 0 );

  // FIXME: should probably move this cruft into CG_BubbleTrail
  if ( sourceContentType == destContentType ) {
    if ( sourceContentType & CONTENTS_WATER ) {
      CG_BubbleTrail( start, tr.endpos, 32 );
    }
  } else if ( sourceContentType & CONTENTS_WATER ) {
    trace_t trace;

    trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
    CG_BubbleTrail( start, trace.endpos, 32 );
  } else if ( destContentType & CONTENTS_WATER ) {
    trace_t trace;

    trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
    CG_BubbleTrail( tr.endpos, trace.endpos, 32 );
  }

  if (  tr.surfaceFlags & SURF_NOIMPACT ) {
    return;
  }

  if ( cg_entities[tr.entityNum].currentState.eType == ET_PLAYER ) {
    /*CG_MissileHitPlayer( WP_SHOTGUN, tr.endpos, tr.plane.normal, tr.entityNum );*/
  } else {
    if ( tr.surfaceFlags & SURF_NOIMPACT ) {
      // SURF_NOIMPACT will not make a flame puff or a mark
      return;
    }
    if ( tr.surfaceFlags & SURF_METALSTEPS ) {
      /*CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_METAL );*/
    } else {
      /*CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_DEFAULT );*/
    }
  }
}

/*
================
CG_ShotgunPattern

Perform the same traces the server did to locate the
hit splashes
================
*/
static void CG_ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, int otherEntNum ) {
  int     i;
  float   r, u;
  vec3_t    end;
  vec3_t    forward, right, up;

  // derive the right and up vectors from the forward vector, because
  // the client won't have any other information
  VectorNormalize2( origin2, forward );
  PerpendicularVector( right, forward );
  CrossProduct( forward, right, up );

  // generate the "random" spread pattern
  for ( i = 0 ; i < DEFAULT_SHOTGUN_COUNT ; i++ ) {
    r = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
    u = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
    VectorMA( origin, 8192 * 16, forward, end);
    VectorMA (end, r, right, end);
    VectorMA (end, u, up, end);

    CG_ShotgunPellet( origin, end, otherEntNum );
  }
}

/*
==============
CG_ShotgunFire
==============
*/
void CG_ShotgunFire( entityState_t *es ) {
  vec3_t  v;
  int   contents;

  VectorSubtract( es->origin2, es->pos.trBase, v );
  VectorNormalize( v );
  VectorScale( v, 32, v );
  VectorAdd( es->pos.trBase, v, v );
  if ( cgs.glconfig.hardwareType != GLHW_RAGEPRO ) {
    // ragepro can't alpha fade, so don't even bother with smoke
    vec3_t      up;

    contents = trap_CM_PointContents( es->pos.trBase, 0 );
    if ( !( contents & CONTENTS_WATER ) ) {
      VectorSet( up, 0, 0, 8 );
      CG_SmokePuff( v, up, 32, 1, 1, 1, 0.33f, 900, cg.time, 0, LEF_PUFF_DONT_SCALE, cgs.media.shotgunSmokePuffShader );
    }
  }
  CG_ShotgunPattern( es->pos.trBase, es->origin2, es->eventParm, es->otherEntityNum );
}

/*
============================================================================

BULLETS

============================================================================
*/


/*
===============
CG_Tracer
===============
*/
void CG_Tracer( vec3_t source, vec3_t dest ) {
  vec3_t    forward, right;
  polyVert_t  verts[4];
  vec3_t    line;
  float   len, begin, end;
  vec3_t    start, finish;
  vec3_t    midpoint;

  // tracer
  VectorSubtract( dest, source, forward );
  len = VectorNormalize( forward );

  // start at least a little ways from the muzzle
  if ( len < 100 ) {
    return;
  }
  begin = 50 + random() * (len - 60);
  end = begin + cg_tracerLength.value;
  if ( end > len ) {
    end = len;
  }
  VectorMA( source, begin, forward, start );
  VectorMA( source, end, forward, finish );

  line[0] = DotProduct( forward, cg.refdef.viewaxis[1] );
  line[1] = DotProduct( forward, cg.refdef.viewaxis[2] );

  VectorScale( cg.refdef.viewaxis[1], line[1], right );
  VectorMA( right, -line[0], cg.refdef.viewaxis[2], right );
  VectorNormalize( right );

  VectorMA( finish, cg_tracerWidth.value, right, verts[0].xyz );
  verts[0].st[0] = 0;
  verts[0].st[1] = 1;
  verts[0].modulate[0] = 255;
  verts[0].modulate[1] = 255;
  verts[0].modulate[2] = 255;
  verts[0].modulate[3] = 255;

  VectorMA( finish, -cg_tracerWidth.value, right, verts[1].xyz );
  verts[1].st[0] = 1;
  verts[1].st[1] = 0;
  verts[1].modulate[0] = 255;
  verts[1].modulate[1] = 255;
  verts[1].modulate[2] = 255;
  verts[1].modulate[3] = 255;

  VectorMA( start, -cg_tracerWidth.value, right, verts[2].xyz );
  verts[2].st[0] = 1;
  verts[2].st[1] = 1;
  verts[2].modulate[0] = 255;
  verts[2].modulate[1] = 255;
  verts[2].modulate[2] = 255;
  verts[2].modulate[3] = 255;

  VectorMA( start, cg_tracerWidth.value, right, verts[3].xyz );
  verts[3].st[0] = 0;
  verts[3].st[1] = 0;
  verts[3].modulate[0] = 255;
  verts[3].modulate[1] = 255;
  verts[3].modulate[2] = 255;
  verts[3].modulate[3] = 255;

  trap_R_AddPolyToScene( cgs.media.tracerShader, 4, verts );

  midpoint[0] = ( start[0] + finish[0] ) * 0.5;
  midpoint[1] = ( start[1] + finish[1] ) * 0.5;
  midpoint[2] = ( start[2] + finish[2] ) * 0.5;

  // add the tracer sound
  trap_S_StartSound( midpoint, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.tracerSound );

}


/*
======================
CG_CalcMuzzlePoint
======================
*/
static qboolean CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle ) {
  vec3_t    forward;
  centity_t *cent;
  int     anim;

  if ( entityNum == cg.snap->ps.clientNum ) {
    VectorCopy( cg.snap->ps.origin, muzzle );
    muzzle[2] += cg.snap->ps.viewheight;
    AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
    VectorMA( muzzle, 14, forward, muzzle );
    return qtrue;
  }

  cent = &cg_entities[entityNum];
  if ( !cent->currentValid ) {
    return qfalse;
  }

  VectorCopy( cent->currentState.pos.trBase, muzzle );

  AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
  anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
  if ( anim == LEGS_WALKCR || anim == LEGS_IDLECR ) {
    muzzle[2] += CROUCH_VIEWHEIGHT;
  } else {
    muzzle[2] += DEFAULT_VIEWHEIGHT;
  }

  VectorMA( muzzle, 14, forward, muzzle );

  return qtrue;

}

/*
======================
CG_Bullet

Renders bullet effects.
======================
*/
void CG_Bullet( vec3_t end, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum ) {
  trace_t trace;
  int sourceContentType, destContentType;
  vec3_t    start;

  // if the shooter is currently valid, calc a source point and possibly
  // do trail effects
  if ( sourceEntityNum >= 0 && cg_tracerChance.value > 0 ) {
    if ( CG_CalcMuzzlePoint( sourceEntityNum, start ) ) {
      sourceContentType = trap_CM_PointContents( start, 0 );
      destContentType = trap_CM_PointContents( end, 0 );

      // do a complete bubble trail if necessary
      if ( ( sourceContentType == destContentType ) && ( sourceContentType & CONTENTS_WATER ) ) {
        CG_BubbleTrail( start, end, 32 );
      }
      // bubble trail from water into air
      else if ( ( sourceContentType & CONTENTS_WATER ) ) {
        trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
        CG_BubbleTrail( start, trace.endpos, 32 );
      }
      // bubble trail from air into water
      else if ( ( destContentType & CONTENTS_WATER ) ) {
        trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
        CG_BubbleTrail( trace.endpos, end, 32 );
      }

      // draw a tracer
      if ( random() < cg_tracerChance.value ) {
        CG_Tracer( start, end );
      }
    }
  }

  // impact splash and mark
  if ( flesh ) {
    CG_Bleed( end, fleshEntityNum );
  } else {
    CG_MissileHitWall( WP_MACHINEGUN, 0, end, normal, IMPACTSOUND_DEFAULT, 0 );
  }

}
