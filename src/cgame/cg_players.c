// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_players.c -- handle the media and animation for player entities

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
                    
#include "cg_local.h"

char  *cg_customSoundNames[MAX_CUSTOM_SOUNDS] = {
  "*death1.wav",
  "*death2.wav",
  "*death3.wav",
  "*jump1.wav",
  "*pain25_1.wav",
  "*pain50_1.wav",
  "*pain75_1.wav",
  "*pain100_1.wav",
  "*falling1.wav",
  "*gasp.wav",
  "*drown.wav",
  "*fall1.wav",
  "*taunt.wav"
};


/*
================
CG_CustomSound

================
*/
sfxHandle_t CG_CustomSound( int clientNum, const char *soundName ) {
  clientInfo_t *ci;
  int     i;

  if ( soundName[0] != '*' ) {
    return trap_S_RegisterSound( soundName, qfalse );
  }

  if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
    clientNum = 0;
  }
  ci = &cgs.clientinfo[ clientNum ];

  for ( i = 0 ; i < MAX_CUSTOM_SOUNDS && cg_customSoundNames[i] ; i++ ) {
    if ( !strcmp( soundName, cg_customSoundNames[i] ) ) {
      return ci->sounds[i];
    }
  }

  CG_Error( "Unknown custom sound: %s", soundName );
  return 0;
}



/*
=============================================================================

CLIENT INFO

=============================================================================
*/

/*
======================
CG_ParseAnimationFile

Read a configuration file containing animation coutns and rates
models/players/visor/animation.cfg, etc
======================
*/
static qboolean CG_ParseAnimationFile( const char *filename, clientInfo_t *ci ) {
  char    *text_p, *prev;
  int     len;
  int     i;
  char    *token;
  float   fps;
  int     skip;
  char    text[20000];
  fileHandle_t  f;
  animation_t *animations;

  animations = ci->animations;

  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if ( len <= 0 ) {
    return qfalse;
  }
  if ( len >= sizeof( text ) - 1 ) {
    CG_Printf( "File %s too long\n", filename );
    return qfalse;
  }
  trap_FS_Read( text, len, f );
  text[len] = 0;
  trap_FS_FCloseFile( f );

  // parse the text
  text_p = text;
  skip = 0; // quite the compiler warning

  ci->footsteps = FOOTSTEP_NORMAL;
  VectorClear( ci->headOffset );
  ci->gender = GENDER_MALE;

  // read optional parameters
  while ( 1 ) {
    prev = text_p;  // so we can unget
    token = COM_Parse( &text_p );
    if ( !token ) {
      break;
    }
    if ( !Q_stricmp( token, "footsteps" ) ) {
      token = COM_Parse( &text_p );
      if ( !token ) {
        break;
      }
      if ( !Q_stricmp( token, "default" ) || !Q_stricmp( token, "normal" ) ) {
        ci->footsteps = FOOTSTEP_NORMAL;
      } else if ( !Q_stricmp( token, "boot" ) ) {
        ci->footsteps = FOOTSTEP_BOOT;
      } else if ( !Q_stricmp( token, "flesh" ) ) {
        ci->footsteps = FOOTSTEP_FLESH;
      } else if ( !Q_stricmp( token, "mech" ) ) {
        ci->footsteps = FOOTSTEP_MECH;
      } else if ( !Q_stricmp( token, "energy" ) ) {
        ci->footsteps = FOOTSTEP_ENERGY;
      } else {
        CG_Printf( "Bad footsteps parm in %s: %s\n", filename, token );
      }
      continue;
    } else if ( !Q_stricmp( token, "headoffset" ) ) {
      for ( i = 0 ; i < 3 ; i++ ) {
        token = COM_Parse( &text_p );
        if ( !token ) {
          break;
        }
        ci->headOffset[i] = atof( token );
      }
      continue;
    } else if ( !Q_stricmp( token, "sex" ) ) {
      token = COM_Parse( &text_p );
      if ( !token ) {
        break;
      }
      if ( token[0] == 'f' || token[0] == 'F' ) {
        ci->gender = GENDER_FEMALE;
      } else if ( token[0] == 'n' || token[0] == 'N' ) {
        ci->gender = GENDER_NEUTER;
      } else {
        ci->gender = GENDER_MALE;
      }
      continue;
    }

    // if it is a number, start parsing animations
    if ( token[0] >= '0' && token[0] <= '9' ) {
      text_p = prev;  // unget the token
      break;
    }
    Com_Printf( "unknown token '%s' is %s\n", token, filename );
  }

  // read information for each frame
  for ( i = 0 ; i < MAX_PLAYER_ANIMATIONS ; i++ ) {

    token = COM_Parse( &text_p );
    if ( !*token ) {
#ifdef NEW_ANIMS
      if( i >= TORSO_GETFLAG && i <= TORSO_NEGATIVE ) {
        animations[i].firstFrame = animations[TORSO_GESTURE].firstFrame;
        animations[i].frameLerp = animations[TORSO_GESTURE].frameLerp;
        animations[i].initialLerp = animations[TORSO_GESTURE].initialLerp;
        animations[i].loopFrames = animations[TORSO_GESTURE].loopFrames;
        animations[i].numFrames = animations[TORSO_GESTURE].numFrames;
        animations[i].reversed = qfalse;
        animations[i].flipflop = qfalse;
        continue;
      }
#endif
      break;
    }
    animations[i].firstFrame = atoi( token );
    // leg only frames are adjusted to not count the upper body only frames
    if ( i == LEGS_WALKCR ) {
      skip = animations[LEGS_WALKCR].firstFrame - animations[TORSO_GESTURE].firstFrame;
    }
    if ( i >= LEGS_WALKCR && i<TORSO_GETFLAG ) {
      animations[i].firstFrame -= skip;
    }

    token = COM_Parse( &text_p );
    if ( !*token ) {
      break;
    }
    animations[i].numFrames = atoi( token );
    animations[i].reversed = qfalse;
    animations[i].flipflop = qfalse;
    // if numFrames is negative the animation is reversed
    if (animations[i].numFrames < 0) {
      animations[i].numFrames = -animations[i].numFrames;
      animations[i].reversed = qtrue;
    }

    token = COM_Parse( &text_p );
    if ( !*token ) {
      break;
    }
    animations[i].loopFrames = atoi( token );

    token = COM_Parse( &text_p );
    if ( !*token ) {
      break;
    }
    fps = atof( token );
    if ( fps == 0 ) {
      fps = 1;
    }
    animations[i].frameLerp = 1000 / fps;
    animations[i].initialLerp = 1000 / fps;
  }

  if ( i != MAX_PLAYER_ANIMATIONS ) {
    CG_Printf( "Error parsing animation file: %s", filename );
    return qfalse;
  }
  // crouch backward animation
  memcpy(&animations[LEGS_BACKCR], &animations[LEGS_WALKCR], sizeof(animation_t));
  animations[LEGS_BACKCR].reversed = qtrue;
  // walk backward animation
  memcpy(&animations[LEGS_BACKWALK], &animations[LEGS_WALK], sizeof(animation_t));
  animations[LEGS_BACKWALK].reversed = qtrue;
  // flag moving fast
  animations[FLAG_RUN].firstFrame = 0;
  animations[FLAG_RUN].numFrames = 16;
  animations[FLAG_RUN].loopFrames = 16;
  animations[FLAG_RUN].frameLerp = 1000 / 15;
  animations[FLAG_RUN].initialLerp = 1000 / 15;
  animations[FLAG_RUN].reversed = qfalse;
  // flag not moving or moving slowly
  animations[FLAG_STAND].firstFrame = 16;
  animations[FLAG_STAND].numFrames = 5;
  animations[FLAG_STAND].loopFrames = 0;
  animations[FLAG_STAND].frameLerp = 1000 / 20;
  animations[FLAG_STAND].initialLerp = 1000 / 20;
  animations[FLAG_STAND].reversed = qfalse;
  // flag speeding up
  animations[FLAG_STAND2RUN].firstFrame = 16;
  animations[FLAG_STAND2RUN].numFrames = 5;
  animations[FLAG_STAND2RUN].loopFrames = 1;
  animations[FLAG_STAND2RUN].frameLerp = 1000 / 15;
  animations[FLAG_STAND2RUN].initialLerp = 1000 / 15;
  animations[FLAG_STAND2RUN].reversed = qtrue;
  //
  // new anims changes
  //
//  animations[TORSO_GETFLAG].flipflop = qtrue;
//  animations[TORSO_GUARDBASE].flipflop = qtrue;
//  animations[TORSO_PATROL].flipflop = qtrue;
//  animations[TORSO_AFFIRMATIVE].flipflop = qtrue;
//  animations[TORSO_NEGATIVE].flipflop = qtrue;
  //
  return qtrue;
}

/*
==========================
CG_RegisterClientSkin
==========================
*/
static qboolean CG_RegisterClientSkin( clientInfo_t *ci, const char *modelName, const char *skinName ) {
  char    filename[MAX_QPATH];

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/lower_%s.skin", modelName, skinName );
  ci->legsSkin = trap_R_RegisterSkin( filename );
  if (!ci->legsSkin) {
    Com_Printf( "Leg skin load failure: %s\n", filename );
  }

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/upper_%s.skin", modelName, skinName );
  ci->torsoSkin = trap_R_RegisterSkin( filename );
  if (!ci->torsoSkin) {
    Com_Printf( "Torso skin load failure: %s\n", filename );
  }

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/head_%s.skin", modelName, skinName );
  ci->headSkin = trap_R_RegisterSkin( filename );
  if (!ci->headSkin) {
    Com_Printf( "Head skin load failure: %s\n", filename );
  }

  if ( !ci->legsSkin || !ci->torsoSkin || !ci->headSkin ) {
    return qfalse;
  }

  return qtrue;
}

/*
==========================
CG_RegisterClientModelname
==========================
*/
static qboolean CG_RegisterClientModelname( clientInfo_t *ci, const char *modelName, const char *skinName ) {
  char    filename[MAX_QPATH];

  // load cmodels before models so filecache works

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/lower.md3", modelName );
  ci->legsModel = trap_R_RegisterModel( filename );
  if ( !ci->legsModel ) {
    Com_Printf( "Failed to load model file %s\n", filename );
    return qfalse;
  }

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/upper.md3", modelName );
  ci->torsoModel = trap_R_RegisterModel( filename );
  if ( !ci->torsoModel ) {
    Com_Printf( "Failed to load model file %s\n", filename );
    return qfalse;
  }

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/head.md3", modelName );
  ci->headModel = trap_R_RegisterModel( filename );
  if ( !ci->headModel ) {
    Com_Printf( "Failed to load model file %s\n", filename );
    return qfalse;
  }

  // if any skins failed to load, return failure
  if ( !CG_RegisterClientSkin( ci, modelName, skinName ) ) {
    Com_Printf( "Failed to load skin file: %s : %s\n", modelName, skinName );
    return qfalse;
  }

  // load the animations
  Com_sprintf( filename, sizeof( filename ), "models/players/%s/animation.cfg", modelName );
  if ( !CG_ParseAnimationFile( filename, ci ) ) {
    Com_Printf( "Failed to load animation file %s\n", filename );
    return qfalse;
  }

  Com_sprintf( filename, sizeof( filename ), "models/players/%s/icon_%s.tga", modelName, skinName );
  ci->modelIcon = trap_R_RegisterShaderNoMip( filename );
  if ( !ci->modelIcon ) {
    Com_Printf( "Failed to load icon file: %s\n", filename );
    return qfalse;
  }

  return qtrue;
}

/*
====================
CG_ColorFromString
====================
*/
static void CG_ColorFromString( const char *v, vec3_t color ) {
  int val;

  VectorClear( color );

  val = atoi( v );

  if ( val < 1 || val > 7 ) {
    VectorSet( color, 1, 1, 1 );
    return;
  }

  if ( val & 1 ) {
    color[2] = 1.0f;
  }
  if ( val & 2 ) {
    color[1] = 1.0f;
  }
  if ( val & 4 ) {
    color[0] = 1.0f;
  }
}


/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void CG_LoadClientInfo( clientInfo_t *ci ) {
  const char  *dir, *fallback;
  int     i;
  const char  *s;
  int     clientNum;

  if ( !CG_RegisterClientModelname( ci, ci->modelName, ci->skinName ) ) {
    if ( cg_buildScript.integer ) {
      CG_Error( "CG_RegisterClientModelname( %s, %s ) failed", ci->modelName, ci->skinName );
    }

    // fall back
    if ( cgs.gametype >= GT_TEAM ) {
      // keep skin name
      if ( !CG_RegisterClientModelname( ci, DEFAULT_MODEL, ci->skinName ) ) {
        CG_Error( "DEFAULT_MODEL / skin (%s/%s) failed to register",
          DEFAULT_MODEL, ci->skinName );
      }
    } else {
      if ( !CG_RegisterClientModelname( ci, DEFAULT_MODEL, "default" ) ) {
        CG_Error( "DEFAULT_MODEL (%s) failed to register", DEFAULT_MODEL );
      }
    }
  }

  // sounds
  dir = ci->modelName;
  fallback = DEFAULT_MODEL;

  for ( i = 0 ; i < MAX_CUSTOM_SOUNDS ; i++ ) {
    s = cg_customSoundNames[i];
    if ( !s ) {
      break;
    }
    ci->sounds[i] = trap_S_RegisterSound( va("sound/player/%s/%s", dir, s + 1), qfalse );
    if ( !ci->sounds[i] ) {
      ci->sounds[i] = trap_S_RegisterSound( va("sound/player/%s/%s", fallback, s + 1), qfalse );
    }
  }

  ci->deferred = qfalse;

  // reset any existing players and bodies, because they might be in bad
  // frames for this new model
  if( clientNum <= MAX_CLIENTS )
  {
    clientNum = ci - cgs.clientinfo;
    for ( i = 0 ; i < MAX_GENTITIES ; i++ ) {
      if ( cg_entities[i].currentState.clientNum == clientNum
        && cg_entities[i].currentState.eType == ET_PLAYER ) {
        CG_ResetPlayerEntity( &cg_entities[i] );
      }
    }
  }
}

/*
======================
CG_CopyClientInfoModel
======================
*/
static void CG_CopyClientInfoModel( clientInfo_t *from, clientInfo_t *to ) {
  VectorCopy( from->headOffset, to->headOffset );
  to->footsteps = from->footsteps;
  to->gender = from->gender;

  to->legsModel = from->legsModel;
  to->legsSkin = from->legsSkin;
  to->torsoModel = from->torsoModel;
  to->torsoSkin = from->torsoSkin;
  to->headModel = from->headModel;
  to->headSkin = from->headSkin;
  to->modelIcon = from->modelIcon;

  memcpy( to->animations, from->animations, sizeof( to->animations ) );
  memcpy( to->sounds, from->sounds, sizeof( to->sounds ) );
}


/*
======================
CG_GetCorpseNum
======================
*/
static int CG_GetCorpseNum( int pclass ) {
  int           i;
  clientInfo_t  *match;
  char          *modelName;

  modelName = BG_FindModelNameForClass( pclass );

  for ( i = 0 ; i < MAX_CLIENTS; i++ )
  {
    match = &cgs.corpseinfo[ i ];
    Com_Printf( "%d: %s %s\n", i, modelName, match->modelName );
    if ( !match->infoValid )
      continue;
      
    if ( match->deferred )
      continue;
    
    if ( !Q_stricmp( modelName, match->modelName )
      /*&& !Q_stricmp( modelName, match->skinName )*/ ) {
      // this clientinfo is identical, so use it's handles
      return i;
    }
  }

  //something has gone horribly wrong
  return -1;
}


/*
======================
CG_ScanForExistingClientInfo
======================
*/
static qboolean CG_ScanForExistingClientInfo( clientInfo_t *ci ) {
  int   i;
  clientInfo_t  *match;

  for ( i = 0 ; i < cgs.maxclients ; i++ ) {
    match = &cgs.clientinfo[ i ];
    if ( !match->infoValid ) {
      continue;
    }
    if ( match->deferred ) {
      continue;
    }
    if ( !Q_stricmp( ci->modelName, match->modelName )
      && !Q_stricmp( ci->skinName, match->skinName ) ) {
      // this clientinfo is identical, so use it's handles

      ci->deferred = qfalse;

      CG_CopyClientInfoModel( match, ci );

      return qtrue;
    }
  }

  // nothing matches, so defer the load
  return qfalse;
}

/*
======================
CG_SetDeferredClientInfo

We aren't going to load it now, so grab some other
client's info to use until we have some spare time.
======================
*/
static void CG_SetDeferredClientInfo( clientInfo_t *ci ) {
  int   i;
  clientInfo_t  *match;

  // if we are in teamplay, only grab a model if the skin is correct
  if ( cgs.gametype >= GT_TEAM ) {
    for ( i = 0 ; i < cgs.maxclients ; i++ ) {
      match = &cgs.clientinfo[ i ];
      if ( !match->infoValid ) {
        continue;
      }
      if ( Q_stricmp( ci->skinName, match->skinName ) ) {
        continue;
      }
      ci->deferred = qtrue;
      CG_CopyClientInfoModel( match, ci );
      return;
    }

    // load the full model, because we don't ever want to show
    // an improper team skin.  This will cause a hitch for the first
    // player, when the second enters.  Combat shouldn't be going on
    // yet, so it shouldn't matter
    CG_LoadClientInfo( ci );
    return;
  }

  // find the first valid clientinfo and grab its stuff
  for ( i = 0 ; i < cgs.maxclients ; i++ ) {
    match = &cgs.clientinfo[ i ];
    if ( !match->infoValid ) {
      continue;
    }

    ci->deferred = qtrue;
    CG_CopyClientInfoModel( match, ci );
    return;
  }

  // we should never get here...
  CG_Printf( "CG_SetDeferredClientInfo: no valid clients!\n" );

  CG_LoadClientInfo( ci );
}


/*
======================
CG_PrecacheClientInfo
======================
*/
void CG_PrecacheClientInfo( int clientNum ) {
  clientInfo_t *ci;
  clientInfo_t newInfo;
  const char  *configstring;
  const char  *v;
  char    *slash;

  ci = &cgs.corpseinfo[ clientNum - MAX_CLIENTS - 1 ];

  //CG_Printf( "%d %d\n", clientNum, (clientNum - MAX_CLIENTS ) );

  configstring = CG_ConfigString( clientNum + CS_PLAYERS );
  if ( !configstring[0] ) {
    return;   // player just left
  }

  // build into a temp buffer so the defer checks can use
  // the old value
  memset( &newInfo, 0, sizeof( newInfo ) );

  // isolate the player's name
  v = Info_ValueForKey(configstring, "n");
  Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );

  // colors
  v = Info_ValueForKey( configstring, "c1" );
  CG_ColorFromString( v, newInfo.color );

  // bot skill
  v = Info_ValueForKey( configstring, "skill" );
  newInfo.botSkill = atoi( v );

  // handicap
  v = Info_ValueForKey( configstring, "hc" );
  newInfo.handicap = atoi( v );

  // wins
  v = Info_ValueForKey( configstring, "w" );
  newInfo.wins = atoi( v );

  // losses
  v = Info_ValueForKey( configstring, "l" );
  newInfo.losses = atoi( v );

  // team
  v = Info_ValueForKey( configstring, "t" );
  newInfo.team = atoi( v );

  // team task
  v = Info_ValueForKey( configstring, "tt" );
  newInfo.teamTask = atoi(v);

  // team leader
  v = Info_ValueForKey( configstring, "tl" );
  newInfo.teamLeader = atoi(v);

  v = Info_ValueForKey( configstring, "g_redteam" );
  Q_strncpyz(newInfo.redTeam, v, MAX_TEAMNAME);

  v = Info_ValueForKey( configstring, "g_blueteam" );
  Q_strncpyz(newInfo.blueTeam, v, MAX_TEAMNAME);
                    
  // model
  v = Info_ValueForKey( configstring, "model" );
  Q_strncpyz( newInfo.modelName, v, sizeof( newInfo.modelName ) );

  slash = strchr( newInfo.modelName, '/' );
  if ( !slash ) {
    // modelName didn not include a skin name
    Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
  } else {
    Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
    // truncate modelName
    *slash = 0;
  }

  //CG_Printf( "PCI: %s\n", v );

  // head model
  v = Info_ValueForKey( configstring, "hmodel" );
  Q_strncpyz( newInfo.headModelName, v, sizeof( newInfo.headModelName ) );

  slash = strchr( newInfo.headModelName, '/' );
  if ( !slash ) {
    // modelName didn not include a skin name
    Q_strncpyz( newInfo.headSkinName, "default", sizeof( newInfo.headSkinName ) );
  } else {
    Q_strncpyz( newInfo.headSkinName, slash + 1, sizeof( newInfo.headSkinName ) );
    // truncate modelName
    *slash = 0;
  }

  newInfo.deferred = qfalse;
  newInfo.infoValid = qtrue;
  CG_LoadClientInfo( &newInfo );
  *ci = newInfo;
}


/*
======================
CG_NewClientInfo
======================
*/
void CG_NewClientInfo( int clientNum ) {
  clientInfo_t *ci;
  clientInfo_t newInfo;
  const char  *configstring;
  const char  *v;
  char    *slash;

  ci = &cgs.clientinfo[clientNum];

  configstring = CG_ConfigString( clientNum + CS_PLAYERS );
  if ( !configstring[0] ) {
    memset( ci, 0, sizeof( *ci ) );
    return;   // player just left
  }

  // build into a temp buffer so the defer checks can use
  // the old value
  memset( &newInfo, 0, sizeof( newInfo ) );

  // isolate the player's name
  v = Info_ValueForKey(configstring, "n");
  Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );

  // colors
  v = Info_ValueForKey( configstring, "c1" );
  CG_ColorFromString( v, newInfo.color );

  // bot skill
  v = Info_ValueForKey( configstring, "skill" );
  newInfo.botSkill = atoi( v );

  // handicap
  v = Info_ValueForKey( configstring, "hc" );
  newInfo.handicap = atoi( v );

  // wins
  v = Info_ValueForKey( configstring, "w" );
  newInfo.wins = atoi( v );

  // losses
  v = Info_ValueForKey( configstring, "l" );
  newInfo.losses = atoi( v );

  // team
  v = Info_ValueForKey( configstring, "t" );
  newInfo.team = atoi( v );

  // team task
  v = Info_ValueForKey( configstring, "tt" );
  newInfo.teamTask = atoi(v);

  // team leader
  v = Info_ValueForKey( configstring, "tl" );
  newInfo.teamLeader = atoi(v);

  v = Info_ValueForKey( configstring, "g_redteam" );
  Q_strncpyz(newInfo.redTeam, v, MAX_TEAMNAME);

  v = Info_ValueForKey( configstring, "g_blueteam" );
  Q_strncpyz(newInfo.blueTeam, v, MAX_TEAMNAME);
                    
  // model
  v = Info_ValueForKey( configstring, "model" );
  Q_strncpyz( newInfo.modelName, v, sizeof( newInfo.modelName ) );

  slash = strchr( newInfo.modelName, '/' );
  if ( !slash ) {
    // modelName didn not include a skin name
    Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
  } else {
    Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
    // truncate modelName
    *slash = 0;
  }

  //CG_Printf( "NCI: %s\n", v );

  // head model
  v = Info_ValueForKey( configstring, "hmodel" );
  Q_strncpyz( newInfo.headModelName, v, sizeof( newInfo.headModelName ) );

  slash = strchr( newInfo.headModelName, '/' );
  if ( !slash ) {
    // modelName didn not include a skin name
    Q_strncpyz( newInfo.headSkinName, "default", sizeof( newInfo.headSkinName ) );
  } else {
    Q_strncpyz( newInfo.headSkinName, slash + 1, sizeof( newInfo.headSkinName ) );
    // truncate modelName
    *slash = 0;
  }

  // scan for an existing clientinfo that matches this modelname
  // so we can avoid loading checks if possible
  if ( !CG_ScanForExistingClientInfo( &newInfo ) ) {
    qboolean  forceDefer;

    forceDefer = trap_MemoryRemaining() < 4000000;

    // if we are defering loads, just have it pick the first valid
    //TA: we should only defer models when ABSOLUTELY TOTALLY necessary since models are precached
    if ( forceDefer ) //|| ( cg_deferPlayers.integer && !cg_buildScript.integer && !cg.loading ) )
    {
      // keep whatever they had if it won't violate team skins
      if ( ci->infoValid &&
        ( cgs.gametype < GT_TEAM || !Q_stricmp( newInfo.skinName, ci->skinName ) ) )
      {
        CG_CopyClientInfoModel( ci, &newInfo );
        newInfo.deferred = qtrue;
      }
      else
      {
        // use whatever is available
        CG_SetDeferredClientInfo( &newInfo );
      }
      
      // if we are low on memory, leave them with this model
      if ( forceDefer )
      {
        CG_Printf( "Memory is low.  Using deferred model.\n" );
        newInfo.deferred = qfalse;
      }
    }
    else
    {
      CG_LoadClientInfo( &newInfo );
    }
  }

  // replace whatever was there with the new one
  newInfo.infoValid = qtrue;
  *ci = newInfo;
}



/*
======================
CG_LoadDeferredPlayers

Called each frame when a player is dead
and the scoreboard is up
so deferred players can be loaded
======================
*/
void CG_LoadDeferredPlayers( void ) {
  int   i;
  clientInfo_t  *ci;

  // scan for a deferred player to load
  for ( i = 0, ci = cgs.clientinfo ; i < cgs.maxclients ; i++, ci++ ) {
    if ( ci->infoValid && ci->deferred ) {
      // if we are low on memory, leave it deferred
      if ( trap_MemoryRemaining() < 4000000 ) {
        CG_Printf( "Memory is low.  Using deferred model.\n" );
        ci->deferred = qfalse;
        continue;
      }
      CG_LoadClientInfo( ci );
//      break;
    }
  }
}

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/


/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_SetLerpFrameAnimation( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
  animation_t *anim;

  lf->animationNumber = newAnimation;
  newAnimation &= ~( ANIM_TOGGLEBIT | ANIM_WALLCLIMBING );

  if ( newAnimation < 0 || newAnimation >= MAX_PLAYER_TOTALANIMATIONS ) {
    CG_Error( "Bad animation number: %i", newAnimation );
  }

  anim = &ci->animations[ newAnimation ];

  lf->animation = anim;
  lf->animationTime = lf->frameTime + anim->initialLerp;

  if ( cg_debugAnim.integer ) {
    CG_Printf( "Anim: %i\n", newAnimation );
  }
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
static void CG_RunLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale ) {
  int     f, numFrames;
  animation_t *anim;

  // debugging tool to get no animations
  if ( cg_animSpeed.integer == 0 ) {
    lf->oldFrame = lf->frame = lf->backlerp = 0;
    return;
  }

  // see if the animation sequence is switching
  if ( newAnimation != lf->animationNumber || !lf->animation ) {
    CG_SetLerpFrameAnimation( ci, lf, newAnimation );
  }

  // if we have passed the current frame, move it to
  // oldFrame and calculate a new frame
  if ( cg.time >= lf->frameTime ) {
    lf->oldFrame = lf->frame;
    lf->oldFrameTime = lf->frameTime;

    // get the next frame based on the animation
    anim = lf->animation;
    if ( !anim->frameLerp ) {
      return;   // shouldn't happen
    }
    if ( cg.time < lf->animationTime ) {
      lf->frameTime = lf->animationTime;    // initial lerp
    } else {
      lf->frameTime = lf->oldFrameTime + anim->frameLerp;
    }
    f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
    f *= speedScale;    // adjust for haste, etc
    numFrames = anim->numFrames;
    if (anim->flipflop) {
      numFrames *= 2;
    }
    if ( f >= numFrames ) {
      f -= numFrames;
      if ( anim->loopFrames ) {
        f %= anim->loopFrames;
        f += anim->numFrames - anim->loopFrames;
      } else {
        f = numFrames - 1;
        // the animation is stuck at the end, so it
        // can immediately transition to another sequence
        lf->frameTime = cg.time;
      }
    }
    if ( anim->reversed ) {
      lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
    }
    else if (anim->flipflop && f>=anim->numFrames) {
      lf->frame = anim->firstFrame + anim->numFrames - 1 - (f%anim->numFrames);
    }
    else {
      lf->frame = anim->firstFrame + f;
    }
    if ( cg.time > lf->frameTime ) {
      lf->frameTime = cg.time;
      if ( cg_debugAnim.integer ) {
        CG_Printf( "Clamp lf->frameTime\n");
      }
    }
  }

  if ( lf->frameTime > cg.time + 200 ) {
    lf->frameTime = cg.time;
  }

  if ( lf->oldFrameTime > cg.time ) {
    lf->oldFrameTime = cg.time;
  }
  // calculate current lerp value
  if ( lf->frameTime == lf->oldFrameTime ) {
    lf->backlerp = 0;
  } else {
    lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
  }
}


/*
===============
CG_ClearLerpFrame
===============
*/
static void CG_ClearLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber ) {
  lf->frameTime = lf->oldFrameTime = cg.time;
  CG_SetLerpFrameAnimation( ci, lf, animationNumber );
  lf->oldFrame = lf->frame = lf->animation->firstFrame;
}


/*
===============
CG_PlayerAnimation
===============
*/
static void CG_PlayerAnimation( centity_t *cent, int *legsOld, int *legs, float *legsBackLerp,
            int *torsoOld, int *torso, float *torsoBackLerp ) {
  clientInfo_t  *ci;
  int       clientNum;
  float     speedScale;

  clientNum = cent->currentState.clientNum;

  if ( cg_noPlayerAnims.integer ) {
    *legsOld = *legs = *torsoOld = *torso = 0;
    return;
  }

  /*if ( cent->currentState.powerups & ( 1 << PW_HASTE ) ) {
    speedScale = 1.5;
  } else*/ {
    speedScale = 1;
  }

  ci = &cgs.clientinfo[ clientNum ];

  // do the shuffle turn frames locally
  if ( cent->pe.legs.yawing && ( cent->currentState.legsAnim & ~( ANIM_TOGGLEBIT | ANIM_WALLCLIMBING ) ) == LEGS_IDLE ) {
    CG_RunLerpFrame( ci, &cent->pe.legs, LEGS_TURN, speedScale );
  } else {
    CG_RunLerpFrame( ci, &cent->pe.legs, cent->currentState.legsAnim, speedScale );
  }

  *legsOld = cent->pe.legs.oldFrame;
  *legs = cent->pe.legs.frame;
  *legsBackLerp = cent->pe.legs.backlerp;

  CG_RunLerpFrame( ci, &cent->pe.torso, cent->currentState.torsoAnim, speedScale );

  *torsoOld = cent->pe.torso.oldFrame;
  *torso = cent->pe.torso.frame;
  *torsoBackLerp = cent->pe.torso.backlerp;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/

/*
==================
CG_SwingAngles
==================
*/
static void CG_SwingAngles( float destination, float swingTolerance, float clampTolerance,
          float speed, float *angle, qboolean *swinging ) {
  float swing;
  float move;
  float scale;

  if ( !*swinging ) {
    // see if a swing should be started
    swing = AngleSubtract( *angle, destination );
    if ( swing > swingTolerance || swing < -swingTolerance ) {
      *swinging = qtrue;
    }
  }

  if ( !*swinging ) {
    return;
  }

  // modify the speed depending on the delta
  // so it doesn't seem so linear
  swing = AngleSubtract( destination, *angle );
  scale = fabs( swing );
  if ( scale < swingTolerance * 0.5 ) {
    scale = 0.5;
  } else if ( scale < swingTolerance ) {
    scale = 1.0;
  } else {
    scale = 2.0;
  }

  // swing towards the destination angle
  if ( swing >= 0 ) {
    move = cg.frametime * scale * speed;
    if ( move >= swing ) {
      move = swing;
      *swinging = qfalse;
    }
    *angle = AngleMod( *angle + move );
  } else if ( swing < 0 ) {
    move = cg.frametime * scale * -speed;
    if ( move <= swing ) {
      move = swing;
      *swinging = qfalse;
    }
    *angle = AngleMod( *angle + move );
  }

  // clamp to no more than tolerance
  swing = AngleSubtract( destination, *angle );
  if ( swing > clampTolerance ) {
    *angle = AngleMod( destination - (clampTolerance - 1) );
  } else if ( swing < -clampTolerance ) {
    *angle = AngleMod( destination + (clampTolerance - 1) );
  }
}

/*
=================
CG_AddPainTwitch
=================
*/
static void CG_AddPainTwitch( centity_t *cent, vec3_t torsoAngles ) {
  int   t;
  float f;

  t = cg.time - cent->pe.painTime;
  if ( t >= PAIN_TWITCH_TIME ) {
    return;
  }

  f = 1.0 - (float)t / PAIN_TWITCH_TIME;

  if ( cent->pe.painDirection ) {
    torsoAngles[ROLL] += 20 * f;
  } else {
    torsoAngles[ROLL] -= 20 * f;
  }
}


/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
static void CG_PlayerAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
  vec3_t    legsAngles, torsoAngles, headAngles;
  float   dest;
  static  int movementOffsets[8] = { 0, 22, 45, -22, 0, 22, -45, -22 };
  vec3_t    velocity;
  float   speed;
  int     dir;

  VectorCopy( cent->lerpAngles, headAngles );
  headAngles[YAW] = AngleMod( headAngles[YAW] );
  VectorClear( legsAngles );
  VectorClear( torsoAngles );

  // --------- yaw -------------

  // allow yaw to drift a bit
  if ( ( cent->currentState.legsAnim & ~( ANIM_TOGGLEBIT | ANIM_WALLCLIMBING ) ) != LEGS_IDLE
    || ( cent->currentState.torsoAnim & ~(ANIM_TOGGLEBIT | ANIM_WALLCLIMBING ) ) != TORSO_STAND  ) {
    // if not standing still, always point all in the same direction
    cent->pe.torso.yawing = qtrue;  // always center
    cent->pe.torso.pitching = qtrue;  // always center
    cent->pe.legs.yawing = qtrue; // always center
  }

  // adjust legs for movement dir
  if ( cent->currentState.eFlags & EF_DEAD ) {
    // don't let dead bodies twitch
    dir = 0;
  } else {
    //TA: did use angles2.. now uses time2.. looks a bit funny but time2 isn't used othwise
    dir = cent->currentState.time2;
    if ( dir < 0 || dir > 7 ) {
      CG_Error( "Bad player movement angle" );
    }
  }
  legsAngles[YAW] = headAngles[YAW] + movementOffsets[ dir ];
  torsoAngles[YAW] = headAngles[YAW] + 0.25 * movementOffsets[ dir ];

  // torso
  CG_SwingAngles( torsoAngles[YAW], 25, 90, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );
  CG_SwingAngles( legsAngles[YAW], 40, 90, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );

  torsoAngles[YAW] = cent->pe.torso.yawAngle;
  legsAngles[YAW] = cent->pe.legs.yawAngle;

  // --------- pitch -------------

  // only show a fraction of the pitch angle in the torso
  if ( headAngles[PITCH] > 180 ) {
    dest = (-360 + headAngles[PITCH]) * 0.75f;
  } else {
    dest = headAngles[PITCH] * 0.75f;
  }
  CG_SwingAngles( dest, 15, 30, 0.1f, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
  torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

  // --------- roll -------------


  // lean towards the direction of travel
  VectorCopy( cent->currentState.pos.trDelta, velocity );
  speed = VectorNormalize( velocity );
  if ( speed ) {
    vec3_t  axis[3];
    float side;

    speed *= 0.05f;

    AnglesToAxis( legsAngles, axis );
    side = speed * DotProduct( velocity, axis[1] );
    legsAngles[ROLL] -= side;

    side = speed * DotProduct( velocity, axis[0] );
    legsAngles[PITCH] += side;
  }

  // pain twitch
  CG_AddPainTwitch( cent, torsoAngles );

  // pull the angles back out of the hierarchial chain
  AnglesSubtract( headAngles, torsoAngles, headAngles );
  AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
  AnglesToAxis( legsAngles, legs );
  AnglesToAxis( torsoAngles, torso );
  AnglesToAxis( headAngles, head );
}


//==========================================================================

/*
===============
CG_HasteTrail
===============
*/
static void CG_HasteTrail( centity_t *cent ) {
  localEntity_t *smoke;
  vec3_t      origin;
  int       anim;

  if ( cent->trailTime > cg.time ) {
    return;
  }
  anim = cent->pe.legs.animationNumber & ~( ANIM_TOGGLEBIT | ANIM_WALLCLIMBING );
  if ( anim != LEGS_RUN && anim != LEGS_BACK ) {
    return;
  }

  cent->trailTime += 100;
  if ( cent->trailTime < cg.time ) {
    cent->trailTime = cg.time;
  }

  VectorCopy( cent->lerpOrigin, origin );
  origin[2] -= 16;

  smoke = CG_SmokePuff( origin, vec3_origin,
          8,
          1, 1, 1, 1,
          500,
          cg.time,
          0,
          0,
          cgs.media.hastePuffShader );

  // use the optimized local entity add
  smoke->leType = LE_SCALE_FADE;
}

/*
===============
CG_TrailItem
===============
*/
static void CG_TrailItem( centity_t *cent, qhandle_t hModel ) {
  refEntity_t   ent;
  vec3_t      angles;
  vec3_t      axis[3];

  VectorCopy( cent->lerpAngles, angles );
  angles[PITCH] = 0;
  angles[ROLL] = 0;
  AnglesToAxis( angles, axis );

  memset( &ent, 0, sizeof( ent ) );
  VectorMA( cent->lerpOrigin, -16, axis[0], ent.origin );
  ent.origin[2] += 16;
  angles[YAW] += 90;
  AnglesToAxis( angles, ent.axis );

  ent.hModel = hModel;
  trap_R_AddRefEntityToScene( &ent );
}


/*
===============
CG_PlayerPowerups
===============
*/
#ifdef NEW_ANIMS
static void CG_PlayerPowerups( centity_t *cent, refEntity_t *torso ) {
#else
static void CG_PlayerPowerups( centity_t *cent ) {
#endif
  int   powerups;
  clientInfo_t  *ci;

  /*powerups = cent->currentState.powerups;
  if ( !powerups ) {
    return;
  }

  // quad gives a dlight
  if ( powerups & ( 1 << PW_QUAD ) ) {
    trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 0.2, 1 );
  }

  // flight plays a looped sound
  if ( powerups & ( 1 << PW_FLIGHT ) ) {
    trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.flightSound );
  }

  // redflag
  if ( powerups & ( 1 << PW_REDFLAG ) ) {
    CG_TrailItem( cent, cgs.media.redFlagModel );
    trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 1, 0.2, 0.2 );
  }

  // blueflag
  if ( powerups & ( 1 << PW_BLUEFLAG ) ) {
    CG_TrailItem( cent, cgs.media.blueFlagModel );
    trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 0.2, 1 );
  }

  // haste leaves smoke trails
  if ( powerups & ( 1 << PW_HASTE ) ) {
    CG_HasteTrail( cent );
  }*/
}


/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
===============
*/
static void CG_PlayerFloatSprite( centity_t *cent, qhandle_t shader ) {
  int       rf;
  refEntity_t   ent;

  if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
    rf = RF_THIRD_PERSON;   // only show in mirrors
  } else {
    rf = 0;
  }

  memset( &ent, 0, sizeof( ent ) );
  VectorCopy( cent->lerpOrigin, ent.origin );
  ent.origin[2] += 48;
  ent.reType = RT_SPRITE;
  ent.customShader = shader;
  ent.radius = 10;
  ent.renderfx = rf;
  ent.shaderRGBA[0] = 255;
  ent.shaderRGBA[1] = 255;
  ent.shaderRGBA[2] = 255;
  ent.shaderRGBA[3] = 255;
  trap_R_AddRefEntityToScene( &ent );
}



/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
*/
static void CG_PlayerSprites( centity_t *cent ) {
  int   team;

  if ( cent->currentState.eFlags & EF_CONNECTION ) {
    CG_PlayerFloatSprite( cent, cgs.media.connectionShader );
    return;
  }

  if ( cent->currentState.eFlags & EF_TALK ) {
    CG_PlayerFloatSprite( cent, cgs.media.balloonShader );
    return;
  }

  if ( cent->currentState.eFlags & EF_AWARD_IMPRESSIVE ) {
    CG_PlayerFloatSprite( cent, cgs.media.medalImpressive );
    return;
  }

  if ( cent->currentState.eFlags & EF_AWARD_EXCELLENT ) {
    CG_PlayerFloatSprite( cent, cgs.media.medalExcellent );
    return;
  }

  if ( cent->currentState.eFlags & EF_AWARD_GAUNTLET ) {
    CG_PlayerFloatSprite( cent, cgs.media.medalGauntlet );
    return;
  }

  team = cgs.clientinfo[ cent->currentState.clientNum ].team;
  if ( !(cent->currentState.eFlags & EF_DEAD) &&
    cg.snap->ps.persistant[PERS_TEAM] == team &&
    cgs.gametype >= GT_TEAM) {
    if (cg_drawFriend.integer) {
      CG_PlayerFloatSprite( cent, cgs.media.friendShader );
    }
    return;
  }
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define SHADOW_DISTANCE   128
static qboolean CG_PlayerShadow( centity_t *cent, float *shadowPlane ) {
  vec3_t    end, mins = {-15, -15, 0}, maxs = {15, 15, 2};
  trace_t   trace;
  float   alpha;

  *shadowPlane = 0;

  if ( cg_shadows.integer == 0 ) {
    return qfalse;
  }

  // no shadows when invisible
  /*if ( cent->currentState.powerups & ( 1 << PW_INVIS ) ) {
    return qfalse;
  }*/

  // send a trace down from the player to the ground
  VectorCopy( cent->lerpOrigin, end );
  end[2] -= SHADOW_DISTANCE;

  trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, mins, maxs, 0, MASK_PLAYERSOLID );

  // no shadow if too high
  if ( trace.fraction == 1.0 ) {
    return qfalse;
  }

  *shadowPlane = trace.endpos[2] + 1;

  if ( cg_shadows.integer != 1 ) {  // no mark for stencil or projection shadows
    return qtrue;
  }

  // fade the shadow out with height
  alpha = 1.0 - trace.fraction;

  // add the mark as a temporary, so it goes directly to the renderer
  // without taking a spot in the cg_marks array
  CG_ImpactMark( cgs.media.shadowMarkShader, trace.endpos, trace.plane.normal,
    cent->pe.legs.yawAngle, alpha,alpha,alpha,1, qfalse, 24, qtrue );

  return qtrue;
}


/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void CG_PlayerSplash( centity_t *cent ) {
  vec3_t    start, end;
  trace_t   trace;
  int     contents;
  polyVert_t  verts[4];

  if ( !cg_shadows.integer ) {
    return;
  }

  VectorCopy( cent->lerpOrigin, end );
  end[2] -= 24;

  // if the feet aren't in liquid, don't make a mark
  // this won't handle moving water brushes, but they wouldn't draw right anyway...
  contents = trap_CM_PointContents( end, 0 );
  if ( !( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) ) {
    return;
  }

  VectorCopy( cent->lerpOrigin, start );
  start[2] += 32;

  // if the head isn't out of liquid, don't make a mark
  contents = trap_CM_PointContents( start, 0 );
  if ( contents & ( CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
    return;
  }

  // trace down to find the surface
  trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) );

  if ( trace.fraction == 1.0 ) {
    return;
  }

  // create a mark polygon
  VectorCopy( trace.endpos, verts[0].xyz );
  verts[0].xyz[0] -= 32;
  verts[0].xyz[1] -= 32;
  verts[0].st[0] = 0;
  verts[0].st[1] = 0;
  verts[0].modulate[0] = 255;
  verts[0].modulate[1] = 255;
  verts[0].modulate[2] = 255;
  verts[0].modulate[3] = 255;

  VectorCopy( trace.endpos, verts[1].xyz );
  verts[1].xyz[0] -= 32;
  verts[1].xyz[1] += 32;
  verts[1].st[0] = 0;
  verts[1].st[1] = 1;
  verts[1].modulate[0] = 255;
  verts[1].modulate[1] = 255;
  verts[1].modulate[2] = 255;
  verts[1].modulate[3] = 255;

  VectorCopy( trace.endpos, verts[2].xyz );
  verts[2].xyz[0] += 32;
  verts[2].xyz[1] += 32;
  verts[2].st[0] = 1;
  verts[2].st[1] = 1;
  verts[2].modulate[0] = 255;
  verts[2].modulate[1] = 255;
  verts[2].modulate[2] = 255;
  verts[2].modulate[3] = 255;

  VectorCopy( trace.endpos, verts[3].xyz );
  verts[3].xyz[0] += 32;
  verts[3].xyz[1] -= 32;
  verts[3].st[0] = 1;
  verts[3].st[1] = 0;
  verts[3].modulate[0] = 255;
  verts[3].modulate[1] = 255;
  verts[3].modulate[2] = 255;
  verts[3].modulate[3] = 255;

  trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}



/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int team ) {

  /*if ( powerups & ( 1 << PW_INVIS ) ) {
    ent->customShader = cgs.media.invisShader;
    trap_R_AddRefEntityToScene( ent );
  } else {*/
    trap_R_AddRefEntityToScene( ent );

    /*if ( powerups & ( 1 << PW_QUAD ) )
    {
      if (team == TEAM_HUMANS)
        ent->customShader = cgs.media.redQuadShader;
      else
        ent->customShader = cgs.media.quadShader;
      trap_R_AddRefEntityToScene( ent );
    }
    if ( powerups & ( 1 << PW_REGEN ) ) {
      if ( ( ( cg.time / 100 ) % 10 ) == 1 ) {
        ent->customShader = cgs.media.regenShader;
        trap_R_AddRefEntityToScene( ent );
      }
    }
    if ( powerups & ( 1 << PW_BATTLESUIT ) ) {
      ent->customShader = cgs.media.battleSuitShader;
      trap_R_AddRefEntityToScene( ent );
    }
  }*/
}


/*
=================
CG_LightVerts
=================
*/
int CG_LightVerts( vec3_t normal, int numVerts, polyVert_t *verts )
{
  int       i, j;
  float     incoming;
  vec3_t      ambientLight;
  vec3_t      lightDir;
  vec3_t      directedLight;

  trap_R_LightForPoint( verts[0].xyz, ambientLight, directedLight, lightDir );

  for (i = 0; i < numVerts; i++) {
    incoming = DotProduct (normal, lightDir);
    if ( incoming <= 0 ) {
      verts[i].modulate[0] = ambientLight[0];
      verts[i].modulate[1] = ambientLight[1];
      verts[i].modulate[2] = ambientLight[2];
      verts[i].modulate[3] = 255;
      continue;
    } 
    j = ( ambientLight[0] + incoming * directedLight[0] );
    if ( j > 255 ) {
      j = 255;
    }
    verts[i].modulate[0] = j;

    j = ( ambientLight[1] + incoming * directedLight[1] );
    if ( j > 255 ) {
      j = 255;
    }
    verts[i].modulate[1] = j;

    j = ( ambientLight[2] + incoming * directedLight[2] );
    if ( j > 255 ) {
      j = 255;
    }
    verts[i].modulate[2] = j;

    verts[i].modulate[3] = 255;
  }
  return qtrue;
}


/*
=================
CG_LightFromDirection
=================
*/
int CG_LightFromDirection( vec3_t point, vec3_t direction )
{
  int       i, j;
  float     incoming;
  vec3_t      ambientLight;
  vec3_t      lightDir;
  vec3_t      directedLight;
  vec3_t      result;

  trap_R_LightForPoint( point, ambientLight, directedLight, lightDir );

  incoming = DotProduct (direction, lightDir);
  if ( incoming <= 0 ) {
    result[0] = ambientLight[0];
    result[1] = ambientLight[1];
    result[2] = ambientLight[2];
    return (int)((float)( result[0] + result[1] + result[2] ) / 3.0f );
  }
  
  j = ( ambientLight[0] + incoming * directedLight[0] );
  if ( j > 255 ) {
    j = 255;
  }
  result[0] = j;

  j = ( ambientLight[1] + incoming * directedLight[1] );
  if ( j > 255 ) {
    j = 255;
  }
  result[1] = j;

  j = ( ambientLight[2] + incoming * directedLight[2] );
  if ( j > 255 ) {
    j = 255;
  }
  result[2] = j;

  return (int)((float)( result[0] + result[1] + result[2] ) / 3.0f );
}


/*
=================
CG_AmbientLight
=================
*/
int CG_AmbientLight( vec3_t point )
{
  vec3_t      ambientLight;
  vec3_t      lightDir;
  vec3_t      directedLight;
  vec3_t      result;

  trap_R_LightForPoint( point, ambientLight, directedLight, lightDir );

  result[0] = ambientLight[0];
  result[1] = ambientLight[1];
  result[2] = ambientLight[2];
  return (int)((float)( result[0] + result[1] + result[2] ) / 3.0f );
}


/*
===============
CG_Player
===============
*/
void CG_Player( centity_t *cent ) {
  clientInfo_t  *ci;
  refEntity_t   legs;
  refEntity_t   torso;
  refEntity_t   head;
  int       clientNum;
  int       renderfx;
  qboolean    shadow;
  float     shadowPlane;

  // the client number is stored in clientNum.  It can't be derived
  // from the entity number, because a single client may have
  // multiple corpses on the level using the same clientinfo
  clientNum = cent->currentState.clientNum;
  if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
    CG_Error( "Bad clientNum on player entity");
  }
  ci = &cgs.clientinfo[ clientNum ];

  // it is possible to see corpses from disconnected players that may
  // not have valid clientinfo
  if ( !ci->infoValid ) {
    return;
  }

  // get the player model information
  renderfx = 0;
  if ( cent->currentState.number == cg.snap->ps.clientNum) {
    if (!cg.renderingThirdPerson) {
      renderfx = RF_THIRD_PERSON;     // only draw in mirrors
    } else {
      if (cg_cameraMode.integer) {
        return;
      }
    }
  }

  memset( &legs, 0, sizeof(legs) );
  memset( &torso, 0, sizeof(torso) );
  memset( &head, 0, sizeof(head) );

  // get the rotation information
  CG_PlayerAngles( cent, legs.axis, torso.axis, head.axis );

  // get the animation state (after rotation, to allow feet shuffle)
  CG_PlayerAnimation( cent, &legs.oldframe, &legs.frame, &legs.backlerp,
     &torso.oldframe, &torso.frame, &torso.backlerp );

#ifndef NEW_ANIMS
  // add powerups floating behind the player
  //CG_PlayerPowerups( cent );
#endif

  // add the talk baloon or disconnect icon
  CG_PlayerSprites( cent );

  // add the shadow
  //TA: but only for humans
  if( ( cent->currentState.powerups & 0xFF ) == PTE_HUMANS )
    shadow = CG_PlayerShadow( cent, &shadowPlane );

  // add a water splash if partially in and out of water
  CG_PlayerSplash( cent );

  if ( cg_shadows.integer == 3 && shadow ) {
    renderfx |= RF_SHADOW_PLANE;
  }
  renderfx |= RF_LIGHTING_ORIGIN;     // use the same origin for all

  //
  // add the legs
  //
  legs.hModel = ci->legsModel;
  legs.customSkin = ci->legsSkin;

  VectorCopy( cent->lerpOrigin, legs.origin );

  VectorCopy( cent->lerpOrigin, legs.lightingOrigin );
  legs.shadowPlane = shadowPlane;
  legs.renderfx = renderfx;
  VectorCopy (legs.origin, legs.oldorigin); // don't positionally lerp at all

  //TA: rotate the model so it sits on a wall
  if( cent->currentState.legsAnim & ANIM_WALLCLIMBING &&
     !( cent->currentState.eFlags & EF_DEAD ) &&
     !( cg.intermissionStarted ) )
  {
    vec3_t forward, surfNormal;
    trace_t tr;

    VectorCopy( cent->currentState.angles2, surfNormal );

    /*CG_Printf( "%d: ", cent->currentState.number );
    CG_Printf( "%f ", surfNormal[ 0 ] );
    CG_Printf( "%f ", surfNormal[ 1 ] );
    CG_Printf( "%f ", surfNormal[ 2 ] );
    CG_Printf( "\n" );*/
    
    AngleVectors( cent->lerpAngles, forward, NULL, NULL );
    VectorCopy( surfNormal, legs.axis[2] );
    ProjectPointOnPlane( legs.axis[0], forward, legs.axis[2] );
    if( !VectorNormalize( legs.axis[0] ) )
    {
      AngleVectors( cent->lerpAngles, NULL, NULL, forward );
      ProjectPointOnPlane( legs.axis[0], forward, legs.axis[2] );
      VectorNormalize( legs.axis[0] );
    }
    CrossProduct( legs.axis[0], legs.axis[2], legs.axis[1] );
    legs.axis[1][0] = -legs.axis[1][0];
    legs.axis[1][1] = -legs.axis[1][1];
    legs.axis[1][2] = -legs.axis[1][2];

    VectorCopy( legs.origin, legs.lightingOrigin );
    VectorCopy( legs.origin, legs.oldorigin ); // don't positionally lerp at all
  }


  //CG_AddRefEntityWithPowerups( &legs, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &legs );

  // if the model failed, allow the default nullmodel to be displayed
  if (!legs.hModel) {
    return;
  }

  //
  // add the torso
  //
  torso.hModel = ci->torsoModel;
  if (!torso.hModel) {
    return;
  }

  torso.customSkin = ci->torsoSkin;

  VectorCopy( cent->lerpOrigin, torso.lightingOrigin );

  CG_PositionRotatedEntityOnTag( &torso, &legs, ci->legsModel, "tag_torso");

  if( cent->currentState.legsAnim & ANIM_WALLCLIMBING &&
      !( cent->currentState.eFlags & EF_DEAD ) &&
      !( cg.intermissionStarted ) )
    AnglesToAxis( cent->lerpAngles, torso.axis );
    
  torso.shadowPlane = shadowPlane;
  torso.renderfx = renderfx;

  //CG_AddRefEntityWithPowerups( &torso, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &torso );

  //
  // add the head
  //
  head.hModel = ci->headModel;
  if (!head.hModel) {
    return;
  }
  head.customSkin = ci->headSkin;

  VectorCopy( cent->lerpOrigin, head.lightingOrigin );

  CG_PositionRotatedEntityOnTag( &head, &torso, ci->torsoModel, "tag_head");

  if( cent->currentState.legsAnim & ANIM_WALLCLIMBING &&
      !( cent->currentState.eFlags & EF_DEAD ) &&
      !( cg.intermissionStarted ) )
    AnglesToAxis( cent->lerpAngles, head.axis );

  head.shadowPlane = shadowPlane;
  head.renderfx = renderfx;

  //CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &head );

  //
  // add the gun / barrel / flash
  //
  CG_AddPlayerWeapon( &torso, NULL, cent );
}

/*
===============
CG_Corpse
===============
*/
void CG_Corpse( centity_t *cent )
{
  clientInfo_t  *ci;
  refEntity_t   legs;
  refEntity_t   torso;
  refEntity_t   head;
  int       clientNum;
  int       renderfx;
  qboolean    shadow;
  float     shadowPlane;
  vec3_t    origin, liveZ, deadZ;

  //if this is the first time the function has been run set cent->corpseNum
  cent->corpseNum = CG_GetCorpseNum( cent->currentState.clientNum );
  if ( cent->corpseNum < 0 || cent->corpseNum >= MAX_CLIENTS )
  {
    CG_Error( "Bad corpseNum on corpse entity: %d", cent->corpseNum );
  }

  ci = &cgs.corpseinfo[ cent->corpseNum ];
    
  // it is possible to see corpses from disconnected players that may
  // not have valid clientinfo
  if ( !ci->infoValid ) {
    return;
  }

  memset( &legs, 0, sizeof(legs) );
  memset( &torso, 0, sizeof(torso) );
  memset( &head, 0, sizeof(head) );

  VectorCopy( cent->lerpOrigin, origin );
  BG_FindBBoxForClass( cent->currentState.clientNum, liveZ, NULL, NULL, deadZ, NULL );
  origin[ 2 ] -= ( liveZ[ 2 ] - deadZ[ 2 ] );

  // get the rotation information
  CG_PlayerAngles( cent, legs.axis, torso.axis, head.axis );

  //set the correct frame (should always be dead)
  if ( cg_noPlayerAnims.integer )
    legs.oldframe = legs.frame = torso.oldframe = torso.frame = 0;
  else
  {
    CG_RunLerpFrame( ci, &cent->pe.legs, cent->currentState.legsAnim, 1 );
    legs.oldframe = cent->pe.legs.oldFrame;
    legs.frame = cent->pe.legs.frame;
    legs.backlerp = cent->pe.legs.backlerp;

    CG_RunLerpFrame( ci, &cent->pe.torso, cent->currentState.torsoAnim, 1 );
    torso.oldframe = cent->pe.torso.oldFrame;
    torso.frame = cent->pe.torso.frame;
    torso.backlerp = cent->pe.torso.backlerp;
  }
              

  // add the shadow
  shadow = CG_PlayerShadow( cent, &shadowPlane );

  // get the player model information
  renderfx = 0;
  if ( cg_shadows.integer == 3 && shadow ) {
    renderfx |= RF_SHADOW_PLANE;
  }
  renderfx |= RF_LIGHTING_ORIGIN;     // use the same origin for all

  //
  // add the legs
  //
  legs.hModel = ci->legsModel;
  legs.customSkin = ci->legsSkin;

  VectorCopy( origin, legs.origin );

  VectorCopy( origin, legs.lightingOrigin );
  legs.shadowPlane = shadowPlane;
  legs.renderfx = renderfx;
  VectorCopy (legs.origin, legs.oldorigin); // don't positionally lerp at all

  //CG_AddRefEntityWithPowerups( &legs, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &legs );

  // if the model failed, allow the default nullmodel to be displayed
  if (!legs.hModel) {
    return;
  }

  //
  // add the torso
  //
  torso.hModel = ci->torsoModel;
  if (!torso.hModel) {
    return;
  }

  torso.customSkin = ci->torsoSkin;

  VectorCopy( origin, torso.lightingOrigin );

  CG_PositionRotatedEntityOnTag( &torso, &legs, ci->legsModel, "tag_torso");

  torso.shadowPlane = shadowPlane;
  torso.renderfx = renderfx;

  //CG_AddRefEntityWithPowerups( &torso, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &torso );

  //
  // add the head
  //
  head.hModel = ci->headModel;
  if (!head.hModel) {
    return;
  }
  head.customSkin = ci->headSkin;

  VectorCopy( origin, head.lightingOrigin );

  CG_PositionRotatedEntityOnTag( &head, &torso, ci->torsoModel, "tag_head");

  head.shadowPlane = shadowPlane;
  head.renderfx = renderfx;

  //CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team );
  trap_R_AddRefEntityToScene( &head );
}


//=====================================================================

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void CG_ResetPlayerEntity( centity_t *cent ) {
  cent->errorTime = -99999;   // guarantee no error decay added
  cent->extrapolated = qfalse;

  CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim );
  CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim );

  BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
  BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

  VectorCopy( cent->lerpOrigin, cent->rawOrigin );
  VectorCopy( cent->lerpAngles, cent->rawAngles );

  memset( &cent->pe.legs, 0, sizeof( cent->pe.legs ) );
  cent->pe.legs.yawAngle = cent->rawAngles[YAW];
  cent->pe.legs.yawing = qfalse;
  cent->pe.legs.pitchAngle = 0;
  cent->pe.legs.pitching = qfalse;

  memset( &cent->pe.torso, 0, sizeof( cent->pe.legs ) );
  cent->pe.torso.yawAngle = cent->rawAngles[YAW];
  cent->pe.torso.yawing = qfalse;
  cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
  cent->pe.torso.pitching = qfalse;

  if ( cg_debugPosition.integer ) {
    CG_Printf("%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
  }
}

