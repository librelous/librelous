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

level_locals_t  level;

typedef struct
{
  vmCvar_t  *vmCvar;
  char    *cvarName;
  char    *defaultString;
  int     cvarFlags;
  int     modificationCount;  // for tracking changes
  qboolean  trackChange;  // track this variable, and announce if changed
  qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t   g_entities[ MAX_GENTITIES ];
gclient_t   g_clients[ MAX_CLIENTS ];

vmCvar_t  g_fraglimit;
vmCvar_t  g_timelimit;
vmCvar_t  g_capturelimit;
vmCvar_t  g_friendlyFire;
vmCvar_t  g_password;
vmCvar_t  g_needpass;
vmCvar_t  g_maxclients;
vmCvar_t  g_maxGameClients;
vmCvar_t  g_dedicated;
vmCvar_t  g_speed;
vmCvar_t  g_gravity;
vmCvar_t  g_cheats;
vmCvar_t  g_knockback;
vmCvar_t  g_quadfactor;
vmCvar_t  g_forcerespawn;
vmCvar_t  g_inactivity;
vmCvar_t  g_debugMove;
vmCvar_t  g_debugDamage;
vmCvar_t  g_debugAlloc;
vmCvar_t  g_weaponRespawn;
vmCvar_t  g_weaponTeamRespawn;
vmCvar_t  g_motd;
vmCvar_t  g_synchronousClients;
vmCvar_t  g_warmup;
vmCvar_t  g_doWarmup;
vmCvar_t  g_restarted;
vmCvar_t  g_log;
vmCvar_t  g_logSync;
vmCvar_t  g_blood;
vmCvar_t  g_podiumDist;
vmCvar_t  g_podiumDrop;
vmCvar_t  g_allowVote;
vmCvar_t  g_teamAutoJoin;
vmCvar_t  g_teamForceBalance;
vmCvar_t  g_banIPs;
vmCvar_t  g_filterBan;
vmCvar_t  g_smoothClients;
vmCvar_t  pmove_fixed;
vmCvar_t  pmove_msec;
vmCvar_t  g_rankings;
vmCvar_t  g_listEntity;

//TA
vmCvar_t  g_humanBuildPoints;
vmCvar_t  g_alienBuildPoints;
vmCvar_t  g_humanStage;
vmCvar_t  g_humanMaxStage;
vmCvar_t  g_humanStage2Threshold;
vmCvar_t  g_humanStage3Threshold;
vmCvar_t  g_alienStage;
vmCvar_t  g_alienMaxStage;
vmCvar_t  g_alienStage2Threshold;
vmCvar_t  g_alienStage3Threshold;

static cvarTable_t   gameCvarTable[ ] =
{
  // don't override the cheat state set by the system
  { &g_cheats, "sv_cheats", "", 0, 0, qfalse },

  // noset vars
  { NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
  { NULL, "gamedate", __DATE__ , CVAR_ROM, 0, qfalse  },
  { &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
  { NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

  // latched vars

  { &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
  { &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

  // change anytime vars
  { &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

  { &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

  { &g_friendlyFire, "g_friendlyFire", "1", CVAR_ARCHIVE, 0, qtrue  },

  { &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
  { &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },

  { &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
  { &g_doWarmup, "g_doWarmup", "0", 0, 0, qtrue  },
  { &g_log, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  },
  { &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0, qfalse  },

  { &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

  { &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
  { &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  },

  { &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

  { &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

  { &g_speed, "g_speed", "320", 0, 0, qtrue  },
  { &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
  { &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
  { &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
  { &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
  { &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
  { &g_forcerespawn, "g_forcerespawn", "20", 0, 0, qtrue },
  { &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
  { &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
  { &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
  { &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
  { &g_motd, "g_motd", "", 0, 0, qfalse },
  { &g_blood, "com_blood", "1", 0, 0, qfalse },

  { &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
  { &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

  { &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },

  { &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
  { &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse},
  { &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse},
  
  { &g_humanBuildPoints, "g_humanBuildPoints", "1000", 0, 0, qfalse  },
  { &g_alienBuildPoints, "g_alienBuildPoints", "1000", 0, 0, qfalse  },
  { &g_humanStage, "g_humanStage", "0", 0, 0, qfalse  },
  { &g_humanMaxStage, "g_humanMaxStage", "2", 0, 0, qfalse  },
  { &g_humanStage2Threshold, "g_humanStage2Threshold", "50", 0, 0, qfalse  },
  { &g_humanStage3Threshold, "g_humanStage3Threshold", "100", 0, 0, qfalse  },
  { &g_alienStage, "g_alienStage", "0", 0, 0, qfalse  },
  { &g_alienMaxStage, "g_alienMaxStage", "2", 0, 0, qfalse  },
  { &g_alienStage2Threshold, "g_alienStage2Threshold", "50", 0, 0, qfalse  },
  { &g_alienStage3Threshold, "g_alienStage3Threshold", "100", 0, 0, qfalse  },

  { &g_rankings, "g_rankings", "0", 0, 0, qfalse}
};

static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[ 0 ] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );

void countSpawns( void );
void calculateBuildPoints( void );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4,
                         int arg5, int arg6, int arg7, int arg8, int arg9,
                         int arg10, int arg11 )
{
  switch( command )
  {
    case GAME_INIT:
      G_InitGame( arg0, arg1, arg2 );
      return 0;
      
    case GAME_SHUTDOWN:
      G_ShutdownGame( arg0 );
      return 0;
      
    case GAME_CLIENT_CONNECT:
      return (int)ClientConnect( arg0, arg1, arg2 );
      
    case GAME_CLIENT_THINK:
      ClientThink( arg0 );
      return 0;
      
    case GAME_CLIENT_USERINFO_CHANGED:
      ClientUserinfoChanged( arg0 );
      return 0;
      
    case GAME_CLIENT_DISCONNECT:
      ClientDisconnect( arg0 );
      return 0;
      
    case GAME_CLIENT_BEGIN:
      ClientBegin( arg0 );
      return 0;
      
    case GAME_CLIENT_COMMAND:
      ClientCommand( arg0 );
      return 0;
      
    case GAME_RUN_FRAME:
      G_RunFrame( arg0 );
      return 0;
      
    case GAME_CONSOLE_COMMAND:
      return ConsoleCommand( );
  }

  return -1;
}


void QDECL G_Printf( const char *fmt, ... )
{
  va_list argptr;
  char    text[ 1024 ];
  char    clientText[ 1048 ];

  va_start( argptr, fmt );
  vsprintf( text, fmt, argptr );
  va_end( argptr );

  if( !g_dedicated.integer )
  {
    Com_sprintf( clientText, 1048, "gprintf \"%s\"", text );
    trap_SendServerCommand( -1, clientText );
  }
    
  trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, fmt );
  vsprintf( text, fmt, argptr );
  va_end( argptr );

  trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void )
{
  gentity_t *e, *e2;
  int       i, j;
  int       c, c2;

  c = 0;
  c2 = 0;
  
  for( i = 1, e = g_entities+i; i < level.num_entities; i++, e++ )
  {
    if( !e->inuse )
      continue;
    
    if( !e->team )
      continue;
    
    if( e->flags & FL_TEAMSLAVE )
      continue;
    
    e->teammaster = e;
    c++;
    c2++;
    
    for( j = i + 1, e2 = e + 1; j < level.num_entities; j++, e2++ )
    {
      if( !e2->inuse )
        continue;
      
      if( !e2->team )
        continue;
      
      if( e2->flags & FL_TEAMSLAVE )
        continue;
      
      if( !strcmp( e->team, e2->team ) )
      {
        c2++;
        e2->teamchain = e->teamchain;
        e->teamchain = e2;
        e2->teammaster = e;
        e2->flags |= FL_TEAMSLAVE;

        // make sure that targets only point at the master
        if( e2->targetname )
        {
          e->targetname = e2->targetname;
          e2->targetname = NULL;
        }
      }
    }
  }

  G_Printf( "%i teams with %i entities\n", c, c2 );
}

void G_RemapTeamShaders( )
{
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
  int         i;
  cvarTable_t *cv;
  qboolean    remapped = qfalse;

  for( i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++ )
  {
    trap_Cvar_Register( cv->vmCvar, cv->cvarName,
      cv->defaultString, cv->cvarFlags );
    
    if( cv->vmCvar )
      cv->modificationCount = cv->vmCvar->modificationCount;

    if( cv->teamShader )
      remapped = qtrue;
  }

  if( remapped )
    G_RemapTeamShaders( );

  // check some things
  level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
  int         i;
  cvarTable_t *cv;
  qboolean    remapped = qfalse;

  for( i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++ )
  {
    if( cv->vmCvar )
    {
      trap_Cvar_Update( cv->vmCvar );

      if( cv->modificationCount != cv->vmCvar->modificationCount )
      {
        cv->modificationCount = cv->vmCvar->modificationCount;

        if( cv->trackChange )
          trap_SendServerCommand( -1, va( "print \"Server: %s changed to %s\n\"",
            cv->cvarName, cv->vmCvar->string ) );

        if( cv->teamShader )
          remapped = qtrue;
      }
    }
  }

  if( remapped )
    G_RemapTeamShaders( );
}


/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart )
{
  int i;

  G_Printf( "------- Game Initialization -------\n" );
  G_Printf( "gamename: %s\n", GAMEVERSION );
  G_Printf( "gamedate: %s\n", __DATE__ );

  srand( randomSeed );

  G_RegisterCvars( );

  G_ProcessIPBans( );

  G_InitMemory( );

  // set some level globals
  memset( &level, 0, sizeof( level ) );
  level.time = levelTime;
  level.startTime = levelTime;

  level.snd_fry = G_SoundIndex( "sound/player/fry.wav" ); // FIXME standing in lava / slime

  if( g_log.string[ 0 ] )
  {
    if( g_logSync.integer )
      trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
    else
      trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
      
    if( !level.logFile )
      G_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
    else
    {
      char serverinfo[ MAX_INFO_STRING ];

      trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
  
      G_LogPrintf( "------------------------------------------------------------\n" );
      G_LogPrintf( "InitGame: %s\n", serverinfo );
    }
  }
  else
    G_Printf( "Not logging to disk.\n" );

  // initialize all entities for this game
  memset( g_entities, 0, MAX_GENTITIES * sizeof( g_entities[ 0 ] ) );
  level.gentities = g_entities;

  // initialize all clients for this game
  level.maxclients = g_maxclients.integer;
  memset( g_clients, 0, MAX_CLIENTS * sizeof( g_clients[ 0 ] ) );
  level.clients = g_clients;

  // set client fields on player ents
  for( i = 0; i < level.maxclients; i++ )
    g_entities[ i ].client = level.clients + i;

  // always leave room for the max number of clients,
  // even if they aren't all used, so numbers inside that
  // range are NEVER anything but clients
  level.num_entities = MAX_CLIENTS;

  // let the server system know where the entites are
  trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
    &level.clients[ 0 ].ps, sizeof( level.clients[ 0 ] ) );

  // parse the key/value pairs and spawn gentities
  G_SpawnEntitiesFromString( );

  // general initialization
  G_FindTeams( );

  //TA:
  G_InitDamageLocations( );

  //reset stages
  trap_Cvar_Set( "g_alienStage", va( "%d", S1 ) );
  trap_Cvar_Set( "g_humanStage", va( "%d", S1 ) );

  G_Printf( "-----------------------------------\n" );

  G_RemapTeamShaders( );

  //TA: so the server counts the spawns without a client attached
  countSpawns( );
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
  G_Printf( "==== ShutdownGame ====\n" );

  if( level.logFile )
  {
    G_LogPrintf( "ShutdownGame:\n" );
    G_LogPrintf( "------------------------------------------------------------\n" );
    trap_FS_FCloseFile( level.logFile );
  }

  // write all the client session data so we can get it back
  G_WriteSessionData( );
}



//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error( int level, const char *error, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, error );
  vsprintf( text, error, argptr );
  va_end( argptr );

  G_Error( "%s", text );
}

void QDECL Com_Printf( const char *msg, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, msg );
  vsprintf( text, msg, argptr );
  va_end( argptr );

  G_Printf( "%s", text );
}

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/


/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b )
{
  gclient_t *ca, *cb;

  ca = &level.clients[ *(int *)a ];
  cb = &level.clients[ *(int *)b ];

  // sort special clients last
  if( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 )
    return 1;
  
  if( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  )
    return -1;

  // then connecting clients
  if( ca->pers.connected == CON_CONNECTING )
    return 1;

  if( cb->pers.connected == CON_CONNECTING )
    return -1;


  // then spectators
  if( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR )
  {
    if( ca->sess.spectatorTime < cb->sess.spectatorTime )
      return -1;

    if( ca->sess.spectatorTime > cb->sess.spectatorTime )
      return 1;

    return 0;
  }
  
  if( ca->sess.sessionTeam == TEAM_SPECTATOR )
    return 1;
  
  if( cb->sess.sessionTeam == TEAM_SPECTATOR )
    return -1;

  // then sort by score
  if( ca->ps.persistant[ PERS_SCORE ] > cb->ps.persistant[ PERS_SCORE ] )
    return -1;

  if( ca->ps.persistant[ PERS_SCORE ] < cb->ps.persistant[ PERS_SCORE ] )
    return 1;

  return 0;
}

/*
============
countSpawns

Counts the number of spawns for each team
============
*/
void countSpawns( void )
{
  int i;
  gentity_t *ent;

  level.numAlienSpawns = 0;
  level.numHumanSpawns = 0;
  
  for ( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    if( ent->s.modelindex == BA_A_SPAWN && ent->health > 0 )
      level.numAlienSpawns++;
      
    if( ent->s.modelindex == BA_H_SPAWN && ent->health > 0 )
      level.numHumanSpawns++;
  }
}


/*
============
calculateBuildPoints

Recalculate the quantity of building points available to the teams
============
*/
void calculateBuildPoints( void )
{
  int       i;
  int       bclass;
  gentity_t *ent;
  int       localHTP = g_humanBuildPoints.integer,
            localATP = g_alienBuildPoints.integer;

  level.humanBuildPoints = level.humanBuildPointsPowered = localHTP;
  level.alienBuildPoints = localATP;

  for ( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if (!ent->inuse)
      continue;

    bclass = BG_FindBuildNumForEntityName( ent->classname );
    
    if( bclass != BA_NONE )
    {
      if( BG_FindTeamForBuildable( bclass ) == BIT_HUMANS )
      {
        level.humanBuildPoints -= BG_FindBuildPointsForBuildable( bclass );

        if( ent->powered )
          level.humanBuildPointsPowered -= BG_FindBuildPointsForBuildable( bclass );
      }
      else
      {
        level.alienBuildPoints -= BG_FindBuildPointsForBuildable( bclass );
      }
    }
  }

  if( level.humanBuildPoints < 0 )
  {
    localHTP -= level.humanBuildPoints;
    level.humanBuildPointsPowered -= level.humanBuildPoints;
    level.humanBuildPoints = 0;
  }
  
  if( level.alienBuildPoints < 0 )
  {
    localATP -= level.alienBuildPoints;
    level.alienBuildPoints = 0;
  }
  
  trap_SetConfigstring( CS_BUILDPOINTS,
                        va( "%d %d %d %d %d", level.alienBuildPoints,
                                              localATP,
                                              level.humanBuildPoints,
                                              localHTP,
                                              level.humanBuildPointsPowered ) );

  //may as well pump the stages here too
  trap_SetConfigstring( CS_STAGES, va( "%d %d",
        g_alienStage.integer, g_humanStage.integer ) );
}

/*
============
CalculateStages
============
*/
void CalculateStages( void )
{
  if( level.alienKills >= g_alienStage2Threshold.integer &&
      g_alienStage.integer == S1 && g_alienMaxStage.integer > S1 )
    trap_Cvar_Set( "g_alienStage", va( "%d", S2 ) );
  if( level.alienKills >= g_alienStage3Threshold.integer && 
      g_alienStage.integer == S2 && g_alienMaxStage.integer > S2 )
    trap_Cvar_Set( "g_alienStage", va( "%d", S3 ) );

  if( level.humanKills >= g_humanStage2Threshold.integer &&
      g_humanStage.integer == S1 && g_humanMaxStage.integer > S1 )
    trap_Cvar_Set( "g_humanStage", va( "%d", S2 ) );
  if( level.humanKills >= g_humanStage3Threshold.integer &&
      g_humanStage.integer == S2 && g_humanMaxStage.integer > S2 )
    trap_Cvar_Set( "g_humanStage", va( "%d", S3 ) );
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void )
{
  int       i;
  int       rank;
  int       score;
  int       newScore;
  gclient_t *cl;
  
  level.follow1 = -1;
  level.follow2 = -1;
  level.numConnectedClients = 0;
  level.numNonSpectatorClients = 0;
  level.numPlayingClients = 0;
  level.numVotingClients = 0;   // don't count bots
  level.numAlienClients = 0;
  level.numHumanClients = 0;
  level.numLiveAlienClients = 0;
  level.numLiveHumanClients = 0;

  for( i = 0; i < TEAM_NUM_TEAMS; i++ )
    level.numteamVotingClients[ i ] = 0;

  for( i = 0; i < level.maxclients; i++ )
  {
    if ( level.clients[ i ].pers.connected != CON_DISCONNECTED )
    {
      level.sortedClients[ level.numConnectedClients ] = i;
      level.numConnectedClients++;

      //TA: so we know when the game ends and for team leveling
      if( level.clients[ i ].ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
      {
        level.numAlienClients++;
        if( level.clients[ i ].sess.sessionTeam != TEAM_SPECTATOR )
          level.numLiveAlienClients++;
      }

      if( level.clients[ i ].ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
      {
        level.numHumanClients++;
        if( level.clients[ i ].sess.sessionTeam != TEAM_SPECTATOR )
          level.numLiveHumanClients++;
      }
      ////////////////

      if( level.clients[ i ].sess.sessionTeam != TEAM_SPECTATOR )
      {
        level.numNonSpectatorClients++;

        // decide if this should be auto-followed
        if( level.clients[ i ].pers.connected == CON_CONNECTED )
        {
          level.numPlayingClients++;
          if( !(g_entities[ i ].r.svFlags & SVF_BOT) )
            level.numVotingClients++;
          
          if( level.clients[ i ].ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
            level.numteamVotingClients[ 0 ]++;
          else if( level.clients[ i ].ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
            level.numteamVotingClients[ 1 ]++;

          if( level.follow1 == -1 )
            level.follow1 = i;
          else if( level.follow2 == -1 )
            level.follow2 = i;
        }

      }
    }
  }

  qsort( level.sortedClients, level.numConnectedClients,
    sizeof( level.sortedClients[ 0 ] ), SortRanks );

  // set the rank value for all clients that are connected and not spectators
  rank = -1;
  score = 0;
  for( i = 0;  i < level.numPlayingClients; i++ )
  {
    cl = &level.clients[ level.sortedClients[ i ] ];
    newScore = cl->ps.persistant[ PERS_SCORE ];
    
    if( i == 0 || newScore != score )
    {
      rank = i;
      // assume we aren't tied until the next client is checked
      level.clients[ level.sortedClients[ i ] ].ps.persistant[ PERS_RANK ] = rank;
    }
    else
    {
      // we are tied with the previous client
      level.clients[ level.sortedClients[ i - 1 ] ].ps.persistant[ PERS_RANK ] = rank;
      level.clients[ level.sortedClients[ i ] ].ps.persistant[ PERS_RANK ] = rank;
    }
    
    score = newScore;
  }

  // set the CS_SCORES1/2 configstrings, which will be visible to everyone
  if( level.numConnectedClients == 0 )
  {
    trap_SetConfigstring( CS_SCORES1, va( "%i", SCORE_NOT_PRESENT ) );
    trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
  }
  else if( level.numConnectedClients == 1 )
  {
    trap_SetConfigstring( CS_SCORES1, va( "%i",
          level.clients[ level.sortedClients[ 0 ] ].ps.persistant[ PERS_SCORE ] ) );
    trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
  }
  else
  {
    trap_SetConfigstring( CS_SCORES1, va( "%i",
          level.clients[ level.sortedClients[ 0 ] ].ps.persistant[ PERS_SCORE ] ) );
    trap_SetConfigstring( CS_SCORES2, va( "%i",
          level.clients[ level.sortedClients[ 1 ] ].ps.persistant[ PERS_SCORE ] ) );
  }

  // see if it is time to end the level
  CheckExitRules( );

  // if we are at the intermission, send the new info to everyone
  if( level.intermissiontime )
    SendScoreboardMessageToAllClients( );
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void )
{
  int   i;

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_CONNECTED )
      ScoreboardMessage( g_entities + i );
  }
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent )
{
  // move to the spot
  VectorCopy( level.intermission_origin, ent->s.origin );
  VectorCopy( level.intermission_origin, ent->client->ps.origin );
  VectorCopy( level.intermission_angle, ent->client->ps.viewangles );
  ent->client->ps.pm_type = PM_INTERMISSION;

  // clean up powerup info
  memset( ent->client->ps.powerups, 0, sizeof( ent->client->ps.powerups ) );

  ent->client->ps.eFlags = 0;
  ent->s.eFlags = 0;
  ent->s.eType = ET_GENERAL;
  ent->s.modelindex = 0;
  ent->s.loopSound = 0;
  ent->s.event = 0;
  ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void )
{
  gentity_t *ent, *target;
  vec3_t    dir;

  // find the intermission spot
  ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );
  
  if( !ent )
  { // the map creator forgot to put in an intermission point...
    SelectSpawnPoint( vec3_origin, level.intermission_origin, level.intermission_angle );
  }
  else
  {
    VectorCopy( ent->s.origin, level.intermission_origin );
    VectorCopy( ent->s.angles, level.intermission_angle );
    // if it has a target, look towards it
    if( ent->target )
    {
      target = G_PickTarget( ent->target );
      
      if( target )
      {
        VectorSubtract( target->s.origin, level.intermission_origin, dir );
        vectoangles( dir, level.intermission_angle );
      }
    }
  }

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void )
{
  int     i;
  gentity_t *client;

  if( level.intermissiontime )
    return;   // already active

  level.intermissiontime = level.time;
  FindIntermissionPoint( );

  // move all clients to the intermission point
  for( i = 0; i < level.maxclients; i++ )
  {
    client = g_entities + i;
    
    if( !client->inuse )
      continue;
    
    // respawn if dead
    if( client->health <= 0 )
      respawn(client);
    
    MoveClientToIntermission( client );
  }

  // send the current scoring to all clients
  SendScoreboardMessageToAllClients( );
}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel( void )
{
  int       i;
  gclient_t *cl;

  trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
  level.changemap = NULL;
  level.intermissiontime = 0;

  // reset all the scores so we don't enter the intermission again
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    cl = level.clients + i;
    if( cl->pers.connected != CON_CONNECTED )
      continue;

    cl->ps.persistant[ PERS_SCORE ] = 0;
  }

  // we need to do this here before chaning to CON_CONNECTING
  G_WriteSessionData( );

  // change all client states to connecting, so the early players into the
  // next level will know the others aren't done reconnecting
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_CONNECTED )
      level.clients[ i ].pers.connected = CON_CONNECTING;
  }

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... )
{
  va_list argptr;
  char    string[ 1024 ];
  int     min, tens, sec;

  sec = level.time / 1000;

  min = sec / 60;
  sec -= min * 60;
  tens = sec / 10;
  sec -= tens * 10;

  Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

  va_start( argptr, fmt );
  vsprintf( string +7 , fmt,argptr );
  va_end( argptr );

  if( g_dedicated.integer )
    G_Printf( "%s", string + 7 );

  if( !level.logFile )
    return;

  trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string )
{
  int         i, numSorted;
  gclient_t   *cl;

  G_LogPrintf( "Exit: %s\n", string );

  level.intermissionQueued = level.time;

  // this will keep the clients from playing any voice sounds
  // that will get cut off when the queued intermission starts
  trap_SetConfigstring( CS_INTERMISSION, "1" );

  // don't send more than 32 scores (FIXME?)
  numSorted = level.numConnectedClients;
  if( numSorted > 32 )
    numSorted = 32;

  for( i = 0; i < numSorted; i++ )
  {
    int   ping;

    cl = &level.clients[ level.sortedClients[ i ] ];

    if( cl->sess.sessionTeam == TEAM_SPECTATOR )
      continue;
    
    if( cl->pers.connected == CON_CONNECTING )
      continue;

    ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

    G_LogPrintf( "score: %i  ping: %i  client: %i %s\n",
      cl->ps.persistant[ PERS_SCORE ], ping, level.sortedClients[ i ],
      cl->pers.netname );

  }
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void )
{
  int       ready, notReady;
  int       i;
  gclient_t *cl;
  int       readyMask;

  // see which players are ready
  ready = 0;
  notReady = 0;
  readyMask = 0;
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    cl = level.clients + i;
    if( cl->pers.connected != CON_CONNECTED )
      continue;
    
    if( g_entities[ cl->ps.clientNum ].r.svFlags & SVF_BOT )
      continue;

    if( cl->readyToExit )
    {
      ready++;
      if( i < 16 )
        readyMask |= 1 << i;
    }
    else
      notReady++;
  }

  trap_SetConfigstring( CS_CLIENTS_READY, va( "%d", readyMask ) );

  // never exit in less than five seconds
  if( level.time < level.intermissiontime + 5000 )
    return;

  // if nobody wants to go, clear timer
  if( !ready )
  {
    level.readyToExit = qfalse;
    return;
  }

  // if everyone wants to go, go now
  if( !notReady )
  {
    ExitLevel( );
    return;
  }

  // the first person to ready starts the ten second timeout
  if( !level.readyToExit )
  {
    level.readyToExit = qtrue;
    level.exitTime = level.time;
  }

  // if we have waited ten seconds since at least one player
  // wanted to exit, go ahead
  if( level.time < level.exitTime + 10000 )
    return;

  ExitLevel( );
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void )
{
  int   a, b;

  if( level.numPlayingClients < 2 )
    return qfalse;

  a = level.clients[ level.sortedClients[ 0 ] ].ps.persistant[ PERS_SCORE ];
  b = level.clients[ level.sortedClients[ 1 ] ].ps.persistant[ PERS_SCORE ];

  return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void )
{
  int     i;
  gclient_t *cl;

  // if at the intermission, wait for all non-bots to
  // signal ready, then go to next level
  if( level.intermissiontime )
  {
    CheckIntermissionExit( );
    return;
  }

  if( level.intermissionQueued )
  {
    if( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME )
    {
      level.intermissionQueued = 0;
      BeginIntermission( );
    }
    
    return;
  }

  if( g_timelimit.integer && !level.warmupTime )
  {
    if( level.time - level.startTime >= g_timelimit.integer * 60000 )
    {
      trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"" );
      LogExit( "Timelimit hit." );
      return;
    }
  }

  //TA: end the game on these conditions
  if( ( level.time > level.startTime + 1000 ) &&
      ( level.numAlienSpawns == 0 ) &&
      ( level.numLiveAlienClients == 0 ) )
  {
    //aliens lose
    trap_SendServerCommand( -1, "print \"Humans win.\n\"");
    LogExit( "Humans win." );
    return;
  }
  else if( ( level.time > level.startTime + 1000 ) &&
           ( level.numHumanSpawns == 0 ) &&
           ( level.numLiveHumanClients == 0 ) )
  {
    //humans lose
    trap_SendServerCommand( -1, "print \"Aliens win.\n\"");
    LogExit( "Aliens win." );
    return;
  }

  if( level.numPlayingClients < 2 )
    return;
}



/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
==================
CheckVote
==================
*/
void CheckVote( void )
{
  if( level.voteExecuteTime && level.voteExecuteTime < level.time )
  {
    level.voteExecuteTime = 0;
    trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
  }
  
  if( !level.voteTime )
    return;
  
  if( level.time - level.voteTime >= VOTE_TIME )
  {
    trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
  }
  else
  {
    if( level.voteYes > level.numVotingClients / 2 )
    {
      // execute the command, then remove the vote
      trap_SendServerCommand( -1, "print \"Vote passed.\n\"" );
      level.voteExecuteTime = level.time + 3000;
    }
    else if( level.voteNo >= level.numVotingClients / 2 )
    {
      // same behavior as a timeout
      trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
    }
    else
    {
      // still waiting for a majority
      return;
    }
  }
  
  level.voteTime = 0;
  trap_SetConfigstring( CS_VOTE_TIME, "" );
}


/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( int team )
{
  int cs_offset;

  if ( team == PTE_HUMANS )
    cs_offset = 0;
  else if ( team == PTE_ALIENS )
    cs_offset = 1;
  else
    return;

  if( !level.teamVoteTime[ cs_offset ] )
    return;
  
  if( level.time - level.teamVoteTime[ cs_offset ] >= VOTE_TIME )
  {
    trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
  }
  else
  {
    if( level.teamVoteYes[ cs_offset ] > level.numteamVotingClients[ cs_offset ] / 2 )
    {
      // execute the command, then remove the vote
      trap_SendServerCommand( -1, "print \"Team vote passed.\n\"" );
      //
      trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.teamVoteString[ cs_offset ] ) );
    }
    else if( level.teamVoteNo[ cs_offset ] >= level.numteamVotingClients[ cs_offset ] / 2 )
    {
      // same behavior as a timeout
      trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
    }
    else
    {
      // still waiting for a majority
      return;
    }
  }
  
  level.teamVoteTime[ cs_offset ] = 0;
  trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );
}


/*
==================
CheckCvars
==================
*/
void CheckCvars( void )
{
  static int lastMod = -1;

  if( g_password.modificationCount != lastMod )
  {
    lastMod = g_password.modificationCount;
    
    if( *g_password.string && Q_stricmp( g_password.string, "none" ) )
      trap_Cvar_Set( "g_needpass", "1" );
    else
      trap_Cvar_Set( "g_needpass", "0" );
  }
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink( gentity_t *ent )
{
  float thinktime;

  thinktime = ent->nextthink;
  if( thinktime <= 0 )
    return;
  
  if( thinktime > level.time )
    return;

  ent->nextthink = 0;
  if( !ent->think )
    G_Error( "NULL ent->think" );
  
  ent->think( ent );
}

/*
=============
G_EvaluateAcceleration

Calculates the acceleration for an entity
=============
*/
void G_EvaluateAcceleration( gentity_t *ent, int msec )
{
  vec3_t  deltaVelocity;
  vec3_t  deltaAccel;

  VectorSubtract( ent->s.pos.trDelta, ent->oldVelocity, deltaVelocity );
  VectorScale( deltaVelocity, 1.0f / (float)msec, ent->acceleration );

  VectorSubtract( ent->acceleration, ent->oldAccel, deltaAccel );
  VectorScale( deltaAccel, 1.0f / (float)msec, ent->jerk );

  VectorCopy( ent->s.pos.trDelta, ent->oldVelocity );
  VectorCopy( ent->acceleration, ent->oldAccel );
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime )
{
  int       i;
  gentity_t *ent;
  int       msec;
  int       start, end;
  
  // if we are waiting for the level to restart, do nothing
  if( level.restarted )
    return;

  level.framenum++;
  level.previousTime = level.time;
  level.time = levelTime;
  msec = level.time - level.previousTime;

  // get any cvar changes
  G_UpdateCvars( );

  //
  // go through all allocated objects
  //
  start = trap_Milliseconds( );
  ent = &g_entities[ 0 ];
  
  for( i = 0; i < level.num_entities; i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    // clear events that are too old
    if( level.time - ent->eventTime > EVENT_VALID_MSEC )
    {
      if( ent->s.event )
      {
        ent->s.event = 0; // &= EV_EVENT_BITS;
        if ( ent->client )
        {
          ent->client->ps.externalEvent = 0;
          //ent->client->ps.events[0] = 0;
          //ent->client->ps.events[1] = 0;
        }
      }
      
      if( ent->freeAfterEvent )
      {
        // tempEntities or dropped items completely go away after their event
        G_FreeEntity( ent );
        continue;
      }
      else if( ent->unlinkAfterEvent )
      {
        // items that will respawn will hide themselves after their pickup event
        ent->unlinkAfterEvent = qfalse;
        trap_UnlinkEntity( ent );
      }
    }

    // temporary entities don't think
    if( ent->freeAfterEvent )
      continue;

    //TA: calculate the acceleration of this entity
    if( ent->evaluateAcceleration )
      G_EvaluateAcceleration( ent, msec );

    if( !ent->r.linked && ent->neverFree )
      continue;

    if( ent->s.eType == ET_MISSILE )
    {
      G_RunMissile( ent );
      continue;
    }

    if( ent->s.eType == ET_BUILDABLE || ent->s.eType == ET_CORPSE || ent->physicsObject )
    {
      G_Physics( ent, msec );
      continue;
    }
    
    if( ent->s.eType == ET_MOVER )
    {
      G_RunMover( ent );
      continue;
    }
    
    if( i < MAX_CLIENTS )
    {
      G_RunClient( ent );
      continue;
    }

    G_RunThink( ent );
  }
  end = trap_Milliseconds();

  start = trap_Milliseconds();
  
  // perform final fixups on the players
  ent = &g_entities[ 0 ];
  
  for( i = 0; i < level.maxclients; i++, ent++ )
  {
    if( ent->inuse )
      ClientEndFrame( ent );
  }
  
  end = trap_Milliseconds();

  //TA:
  countSpawns( );
  calculateBuildPoints( );
  CalculateStages( );

  // see if it is time to end the level
  CheckExitRules( );

  // update to team status?
  CheckTeamStatus( );

  // cancel vote if timed out
  CheckVote( );

  // check team votes
  CheckTeamVote( PTE_HUMANS );
  CheckTeamVote( PTE_ALIENS );

  // for tracking changes
  CheckCvars( );

  if( g_listEntity.integer )
  {
    for( i = 0; i < MAX_GENTITIES; i++ )
      G_Printf( "%4i: %s\n", i, g_entities[ i ].classname );

    trap_Cvar_Set( "g_listEntity", "0" );
  }
}

