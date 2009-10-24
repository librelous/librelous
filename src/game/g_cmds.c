/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

/*
==================
G_SanitiseString

Remove color codes and non-alphanumeric characters from a string
==================
*/
void G_SanitiseString( char *in, char *out, int len )
{
  qboolean skip = qtrue;
  int spaces = 0;

  len--;

  while( *in && len > 0 )
  {
    // strip leading white space
    if( *in == ' ' )
    {
      if( skip )
      {
        in++;
        continue;
      }
      spaces++;
    }
    else
    {
      spaces = 0;
      skip = qfalse;
    }

    if( Q_IsColorString( in ) )
    {
      in += 2;    // skip color code
      continue;
    }

    if( isalnum( *in ) )
    {
        *out++ = tolower( *in );
        len--;
    }
    in++;
  }
  out -= spaces;
  *out = 0;
}

/*
==================
G_ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int G_ClientNumberFromString( char *s )
{
  gclient_t *cl;
  int       i;
  char      s2[ MAX_NAME_LENGTH ];
  char      n2[ MAX_NAME_LENGTH ];

  // numeric values are just slot numbers
  for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
  if( !s[ i ] )
  {
    i = atoi( s );

    if( i < 0 || i >= level.maxclients )
      return -1;

    cl = &level.clients[ i ];

    if( cl->pers.connected == CON_DISCONNECTED )
      return -1;

    return i;
  }

  // check for a name match
  G_SanitiseString( s, s2, sizeof( s2 ) );

  for( i = 0, cl = level.clients; i < level.maxclients; i++, cl++ )
  {
    if( cl->pers.connected == CON_DISCONNECTED )
      continue;

    G_SanitiseString( cl->pers.netname, n2, sizeof( n2 ) );

    if( !strcmp( n2, s2 ) )
      return i;
  }

  return -1;
}


/*
==================
G_MatchOnePlayer

This is a companion function to G_ClientNumbersFromString()

err will be populated with an error message.
==================
*/
void G_MatchOnePlayer( int *plist, int num, char *err, int len )
{
  gclient_t *cl;
  int i;
  char line[ MAX_NAME_LENGTH + 10 ] = {""};

  err[ 0 ] = '\0';
  if( num == 0 )
  {
    Q_strcat( err, len, "no connected player by that name or slot #" );
  }
  else if( num > 1 )
  {
    Q_strcat( err, len, "more than one player name matches. "
            "be more specific or use the slot #:\n" );
    for( i = 0; i < num; i++ )
    {
      cl = &level.clients[ plist[ i ] ];
      if( cl->pers.connected == CON_DISCONNECTED )
        continue;
      Com_sprintf( line, sizeof( line ), "%2i - %s^7\n",
        plist[ i ], cl->pers.netname );
      if( strlen( err ) + strlen( line ) > len )
        break;
      Q_strcat( err, len, line );
    }
  }
}

/*
==================
G_ClientNumbersFromString

Sets plist to an array of integers that represent client numbers that have
names that are a partial match for s.

Returns number of matching clientids up to max.
==================
*/
int G_ClientNumbersFromString( char *s, int *plist, int max )
{
  gclient_t *p;
  int i, found = 0;
  char n2[ MAX_NAME_LENGTH ] = {""};
  char s2[ MAX_NAME_LENGTH ] = {""};

  if( max == 0 )
    return 0;

  // if a number is provided, it is a clientnum
  for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
  if( !s[ i ] )
  {
    i = atoi( s );
    if( i >= 0 && i < level.maxclients )
    {
      p = &level.clients[ i ];
      if( p->pers.connected != CON_DISCONNECTED )
      {
        *plist = i;
        return 1;
      }
    }
    // we must assume that if only a number is provided, it is a clientNum
    return 0;
  }

  // now look for name matches
  G_SanitiseString( s, s2, sizeof( s2 ) );
  if( strlen( s2 ) < 1 )
    return 0;
  for( i = 0; i < level.maxclients && found < max; i++ )
  {
    p = &level.clients[ i ];
    if( p->pers.connected == CON_DISCONNECTED )
    {
      continue;
    }
    G_SanitiseString( p->pers.netname, n2, sizeof( n2 ) );
    if( strstr( n2, s2 ) )
    {
      *plist++ = i;
      found++;
    }
  }
  return found;
}

/*
==================
ScoreboardMessage

==================
*/
void ScoreboardMessage( gentity_t *ent )
{
  char      entry[ 1024 ];
  char      string[ 1400 ];
  int       stringlength;
  int       i, j;
  gclient_t *cl;
  int       numSorted;
  weapon_t  weapon = WP_NONE;
  upgrade_t upgrade = UP_NONE;

  // send the latest information on all clients
  string[ 0 ] = 0;
  stringlength = 0;

  numSorted = level.numConnectedClients;

  for( i = 0; i < numSorted; i++ )
  {
    int   ping;

    cl = &level.clients[ level.sortedClients[ i ] ];

    if( cl->pers.connected == CON_CONNECTING )
      ping = -1;
    else
      ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

    if( cl->sess.spectatorState == SPECTATOR_NOT &&
        ( ent->client->pers.teamSelection == TEAM_NONE ||
          cl->pers.teamSelection == ent->client->pers.teamSelection ) )
    {
      weapon = cl->ps.weapon;

      if( BG_InventoryContainsUpgrade( UP_BATTLESUIT, cl->ps.stats ) )
        upgrade = UP_BATTLESUIT;
      else if( BG_InventoryContainsUpgrade( UP_JETPACK, cl->ps.stats ) )
        upgrade = UP_JETPACK;
      else if( BG_InventoryContainsUpgrade( UP_BATTPACK, cl->ps.stats ) )
        upgrade = UP_BATTPACK;
      else if( BG_InventoryContainsUpgrade( UP_HELMET, cl->ps.stats ) )
        upgrade = UP_HELMET;
      else if( BG_InventoryContainsUpgrade( UP_LIGHTARMOUR, cl->ps.stats ) )
        upgrade = UP_LIGHTARMOUR;
      else
        upgrade = UP_NONE;
    }
    else
    {
      weapon = WP_NONE;
      upgrade = UP_NONE;
    }

    Com_sprintf( entry, sizeof( entry ),
      " %d %d %d %d %d %d", level.sortedClients[ i ], cl->ps.persistant[ PERS_SCORE ],
      ping, ( level.time - cl->pers.enterTime ) / 60000, weapon, upgrade );

    j = strlen( entry );

    if( stringlength + j > 1024 )
      break;

    strcpy( string + stringlength, entry );
    stringlength += j;
  }

  trap_SendServerCommand( ent-g_entities, va( "scores %i %i%s",
    level.alienKills, level.humanKills, string ) );
}


/*
==================
ConcatArgs
==================
*/
char *ConcatArgs( int start )
{
  int         i, c, tlen;
  static char line[ MAX_STRING_CHARS ];
  int         len;
  char        arg[ MAX_STRING_CHARS ];

  len = 0;
  c = trap_Argc( );

  for( i = start; i < c; i++ )
  {
    trap_Argv( i, arg, sizeof( arg ) );
    tlen = strlen( arg );

    if( len + tlen >= MAX_STRING_CHARS - 1 )
      break;

    memcpy( line + len, arg, tlen );
    len += tlen;

    if( len == MAX_STRING_CHARS - 1 )
      break;

    if( i != c - 1 )
    {
      line[ len ] = ' ';
      len++;
    }
  }

  line[ len ] = 0;

  return line;
}


/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f( gentity_t *ent )
{
  char      *name;
  qboolean  give_all = qfalse;

  name = ConcatArgs( 1 );
  if( Q_stricmp( name, "all" ) == 0 )
    give_all = qtrue;

  if( give_all || Q_stricmp( name, "health" ) == 0 )
  {
    ent->health = ent->client->ps.stats[ STAT_MAX_HEALTH ];
    BG_AddUpgradeToInventory( UP_MEDKIT, ent->client->ps.stats );
  }

  if( give_all || Q_stricmpn( name, "funds", 5 ) == 0 )
  {
    float credits = atof( name + 6 );

    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      credits *= ALIEN_CREDITS_PER_KILL;

    if( give_all )
      credits = ALIEN_MAX_CREDITS;

    G_AddCreditToClient( ent->client, credits, qtrue );
  }

  if( give_all || Q_stricmp( name, "stamina" ) == 0 )
    ent->client->ps.stats[ STAT_STAMINA ] = MAX_STAMINA;

  if( Q_stricmp( name, "poison" ) == 0 )
  {
    if( ent->client->pers.teamSelection == TEAM_HUMANS )
    {
      ent->client->ps.stats[ STAT_STATE ] |= SS_POISONED;
      ent->client->lastPoisonTime = level.time;
      ent->client->lastPoisonClient = ent;
    }
    else
    {
      ent->client->ps.stats[ STAT_STATE ] |= SS_BOOSTED;
      ent->client->boostedTime = level.time;
    }
  }
  
  if( Q_stricmp( name, "gas" ) == 0 )
  {
    ent->client->ps.eFlags |= EF_POISONCLOUDED;
    ent->client->lastPoisonCloudedTime = level.time;
    trap_SendServerCommand( ent->client->ps.clientNum,
                              "poisoncloud" );
  }

  if( give_all || Q_stricmp( name, "ammo" ) == 0 )
  {
    gclient_t *client = ent->client;

    if( client->ps.weapon != WP_ALEVEL3_UPG &&
        BG_Weapon( client->ps.weapon )->infiniteAmmo )
      return;

    client->ps.ammo = BG_Weapon( client->ps.weapon )->maxAmmo;
    client->ps.clips = BG_Weapon( client->ps.weapon )->maxClips;

    if( BG_Weapon( client->ps.weapon )->usesEnergy &&
        BG_InventoryContainsUpgrade( UP_BATTPACK, client->ps.stats ) )
      client->ps.ammo = (int)( (float)client->ps.ammo * BATTPACK_MODIFIER );
  }
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f( gentity_t *ent )
{
  char  *msg;

  ent->flags ^= FL_GODMODE;

  if( !( ent->flags & FL_GODMODE ) )
    msg = "godmode OFF\n";
  else
    msg = "godmode ON\n";

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent )
{
  char  *msg;

  ent->flags ^= FL_NOTARGET;

  if( !( ent->flags & FL_NOTARGET ) )
    msg = "notarget OFF\n";
  else
    msg = "notarget ON\n";

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent )
{
  char  *msg;

  if( ent->client->noclip )
    msg = "noclip OFF\n";
  else
    msg = "noclip ON\n";

  ent->client->noclip = !ent->client->noclip;

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent )
{
  BeginIntermission( );
  trap_SendServerCommand( ent - g_entities, "clientLevelShot" );
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
  if( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Leave the Hovel first (use your destroy key)\n\"" );
    return;
  }

  if( g_cheats.integer )
  {
    ent->flags &= ~FL_GODMODE;
    ent->client->ps.stats[ STAT_HEALTH ] = ent->health = 0;
    player_die( ent, ent, ent, 100000, MOD_SUICIDE );
  }
  else
  {
    if( ent->suicideTime == 0 )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You will suicide in 20 seconds\n\"" );
      ent->suicideTime = level.time + 20000;
    }
    else if( ent->suicideTime > level.time )
    {
      trap_SendServerCommand( ent-g_entities, "print \"Suicide cancelled\n\"" );
      ent->suicideTime = 0;
    }
  }
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent )
{
  team_t    team;
  team_t    oldteam = ent->client->pers.teamSelection;
  char      s[ MAX_TOKEN_CHARS ];
  qboolean  force = G_admin_permission(ent, ADMF_FORCETEAMCHANGE);
  int       aliens = level.numAlienClients;
  int       humans = level.numHumanClients;

  // stop team join spam
  if( level.time - ent->client->pers.teamChangeTime < 1000 )
    return;

  if( oldteam == TEAM_ALIENS )
    aliens--;
  else if( oldteam == TEAM_HUMANS )
    humans--;

  // disallow joining teams during warmup
  if( g_doWarmup.integer && ( ( level.warmupTime - level.time ) / 1000 ) > 0 )
  {
    G_TriggerMenu( ent - g_entities, MN_WARMUP );
    return;
  }

  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"team: %i\n\"",
      oldteam ) );
    return;
  }

  if( !Q_stricmp( s, "auto" ) )
  {
    if( level.humanTeamLocked && level.alienTeamLocked )
      team = TEAM_NONE;
    else if( level.humanTeamLocked || humans > aliens )
      team = TEAM_ALIENS;

    else if( level.alienTeamLocked || aliens > humans )
      team = TEAM_HUMANS;
    else
      team = TEAM_ALIENS + ( rand( ) % 2 );
  }
  else switch( G_TeamFromString( s ) )
  {
    case TEAM_NONE:
      team = TEAM_NONE;
      break;

    case TEAM_ALIENS:
      if( level.alienTeamLocked )
      {
        G_TriggerMenu( ent - g_entities, MN_A_TEAMLOCKED );
        return;
      }
      else if( level.humanTeamLocked )
        force = qtrue;

      if( !force && g_teamForceBalance.integer && aliens > humans )
      {
        G_TriggerMenu( ent - g_entities, MN_A_TEAMFULL );
        return;
      }

      team = TEAM_ALIENS;
      break;

    case TEAM_HUMANS:
      if( level.humanTeamLocked )
      {
        G_TriggerMenu( ent - g_entities, MN_H_TEAMLOCKED );
        return;
      }
      else if( level.alienTeamLocked )
        force = qtrue;

      if( !force && g_teamForceBalance.integer && humans > aliens )
      {
        G_TriggerMenu( ent - g_entities, MN_H_TEAMFULL );
        return;
      }

      team = TEAM_HUMANS;
      break;

    default:
      trap_SendServerCommand( ent-g_entities,
        va( "print \"Unknown team: %s\n\"", s ) );
      return;
  }

  // stop team join spam
  if( oldteam == team )
    return;

  if( team != TEAM_NONE && g_maxGameClients.integer &&
    level.numPlayingClients >= g_maxGameClients.integer )
  {
    G_TriggerMenu( ent - g_entities, MN_PLAYERLIMIT );
    return;
  }

  // guard against build timer exploit
  if( oldteam != TEAM_NONE && ent->client->sess.spectatorState == SPECTATOR_NOT &&
     ( ent->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_BUILDER0 ||
       ent->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_BUILDER0_UPG ||
       BG_InventoryContainsWeapon( WP_HBUILD, ent->client->ps.stats ) ) &&
      ent->client->ps.stats[ STAT_MISC ] > 0 )
  {
    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      G_TriggerMenu( ent->client->ps.clientNum, MN_A_TEAMCHANGEBUILDTIMER );
    else
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_TEAMCHANGEBUILDTIMER );
    return;
  }

  // Apply the change
  G_ChangeTeam( ent, team );
}


/*
==================
G_Say
==================
*/
static qboolean G_SayTo( gentity_t *ent, gentity_t *other, saymode_t mode, const char *message )
{
  if( !other )
    return qfalse;

  if( !other->inuse )
    return qfalse;

  if( !other->client )
    return qfalse;

  if( other->client->pers.connected != CON_CONNECTED )
    return qfalse;

  if( ( ent && !OnSameTeam( ent, other ) ) &&
      ( mode == SAY_TEAM || mode == SAY_AREA || mode == SAY_TPRIVMSG ) )
  {
    if( other->client->pers.teamSelection != TEAM_NONE )
      return qfalse;

    // specs with ADMF_SPEC_ALLCHAT flag can see team chat
    if( !G_admin_permission( other, ADMF_SPEC_ALLCHAT ) && mode != SAY_TPRIVMSG )
      return qfalse;
  }

  trap_SendServerCommand( other-g_entities, va( "chat %d %d\"%s\"",
    ent ? ent-g_entities : -1,
    mode,
    message ) );

  return qtrue;
}

void G_Say( gentity_t *ent, saymode_t mode, const char *chatText )
{
  int         j;
  gentity_t   *other;
  // don't let text be too long for malicious reasons
  char        text[ MAX_SAY_TEXT ];

  // check if blocked by g_specChat 0
  if( ( !g_specChat.integer ) && ( mode != SAY_TEAM ) &&
      ( ent ) && ( ent->client->pers.teamSelection == TEAM_NONE ) && 
      ( !G_admin_permission( ent, ADMF_NOCENSORFLOOD ) ) ) 
  {
    trap_SendServerCommand( ent-g_entities, "print \"say: Global chatting for "
      "spectators has been disabled. You may only use team chat.\n\"" );
    mode = SAY_TEAM;
  }

  switch( mode )
  {
    case SAY_ALL:
      G_LogPrintf( "Say: %d \"%s" S_COLOR_WHITE "\": " S_COLOR_GREEN "%s\n",
        ( ent ) ? ent - g_entities : -1,
        ( ent ) ? ent->client->pers.netname : "console", chatText );
      break;
    case SAY_TEAM:
      // console say_team is handled in g_svscmds, not here
      if( !ent || !ent->client )
        Com_Error( ERR_FATAL, "SAY_TEAM by non-client entity\n" );
      G_LogPrintf( "SayTeam: %d \"%s" S_COLOR_WHITE "\": " S_COLOR_CYAN "%s\n",
        ent - g_entities, ent->client->pers.netname, chatText );
      break;
    case SAY_RAW:
      if( ent )
        Com_Error( ERR_FATAL, "SAY_RAW by client entity\n" );
      G_LogPrintf( "Chat: -1 \"console\": %s\n", chatText );
    default:
      break;
  }

  Q_strncpyz( text, chatText, sizeof( text ) );

  // send it to all the apropriate clients
  for( j = 0; j < level.maxclients; j++ )
  {
    other = &g_entities[ j ];
    G_SayTo( ent, other, mode, text );
  }

  if( g_adminParseSay.integer )
  {
    G_admin_cmd_check ( ent, qtrue );
  }
}

/*
==================
Cmd_ActionMessage_f

This is just a wrapper for convenience when speaking from the console
==================
*/
static void Cmd_ActionMessage_f( gentity_t *ent )
{
  char text[ MAX_SAY_TEXT ];

  Com_sprintf( text, sizeof( text ), "/me %s", ConcatArgs( 1 ) );

  G_Say( ent, SAY_ALL, text );
}

/*
==================
Cmd_SayArea_f
==================
*/
static void Cmd_SayArea_f( gentity_t *ent )
{
  int    entityList[ MAX_GENTITIES ];
  int    num, i;
  vec3_t range = { 1000.0f, 1000.0f, 1000.0f };
  vec3_t mins, maxs;
  char   *msg;
  char cmd[ sizeof( "say_team" ) ];
  int skiparg = 0;

  if( ent->client->pers.teamSelection == TEAM_NONE )
  {
    G_TriggerMenu( ent - g_entities, MN_CMD_TEAM );
    return;
  }

  if( ent->client->ps.stats[ STAT_HEALTH ] <= 0 ||
      ent->client->sess.spectatorState != SPECTATOR_NOT )
  {
    G_TriggerMenu( ent - g_entities, MN_CMD_LIVING );
    return;
  }

  // Skip say/say_team if this was used from one of those
  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skiparg = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( va( "usage: %s [message]\n", cmd ) );
    return;
  }

  msg = G_SayConcatArgs( 1 + skiparg );

  for(i = 0; i < 3; i++ )
    range[ i ] = g_sayAreaRange.value;

  G_LogPrintf( "SayArea: %d \"%s" S_COLOR_WHITE "\": " S_COLOR_BLUE "%s\n",
    ent - g_entities, ent->client->pers.netname, msg );

  VectorAdd( ent->s.origin, range, maxs );
  VectorSubtract( ent->s.origin, range, mins );

  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
    G_SayTo( ent, &g_entities[ entityList[ i ] ], SAY_AREA, msg );
  
  //Send to ADMF_SPEC_ALLCHAT candidates
  for( i = 0; i < level.maxclients; i++ )
  {
    if( g_entities[ i ].client->pers.teamSelection == TEAM_NONE &&
        G_admin_permission( &g_entities[ i ], ADMF_SPEC_ALLCHAT ) )
    {
      G_SayTo( ent, &g_entities[ i ], SAY_AREA, msg );   
    }
  }
}


/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent )
{
  char    *p;
  char    *args;
  saymode_t mode = SAY_ALL;

  args = G_SayConcatArgs( 0 );
  if( Q_stricmpn( args, "say_team ", 9 ) == 0 )
    mode = SAY_TEAM;

  // support parsing /m out of say text since some people have a hard
  // time figuring out what the console is.
  if( !Q_stricmpn( args, "say /m ", 7 ) ||
      !Q_stricmpn( args, "say_team /m ", 12 ) ||
      !Q_stricmpn( args, "say /mt ", 8 ) ||
      !Q_stricmpn( args, "say_team /mt ", 13 ) )
  {
    Cmd_PrivateMessage_f( ent );
    return;
  }

  // support parsing /a out of say text for the same reason
  if( !Q_stricmpn( args, "say /a ", 7 ) ||
      !Q_stricmpn( args, "say_team /a ", 12 ) )
  {
    Cmd_AdminMessage_f( ent );
    return;
  }

  // support parsing /say_area out of say text for the same reason
  if( !Q_stricmpn( args, "say /say_area ", 14 ) ||
      !Q_stricmpn( args, "say_team /say_area ", 19 ) )
  {
    Cmd_SayArea_f( ent );
    return;
  }

  if( trap_Argc( ) < 2 )
    return;

  p = ConcatArgs( 1 );

  G_Say( ent, mode, p );
}

/*
==================
Cmd_VSay_f
==================
*/
void Cmd_VSay_f( gentity_t *ent )
{
  char            arg[MAX_TOKEN_CHARS];
  voiceChannel_t  vchan;
  voice_t         *voice;
  voiceCmd_t      *cmd;
  voiceTrack_t    *track;
  int             cmdNum = 0;
  int             trackNum = 0;
  char            voiceName[ MAX_VOICE_NAME_LEN ] = {"default"};
  char            voiceCmd[ MAX_VOICE_CMD_LEN ] = {""};
  char            vsay[ 12 ] = {""};
  weapon_t        weapon;

  if( !ent || !ent->client )
    Com_Error( ERR_FATAL, "Cmd_VSay_f() called by non-client entity\n" );

  trap_Argv( 0, arg, sizeof( arg ) );
  if( trap_Argc( ) < 2 )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"usage: %s command [text] \n\"", arg ) );
    return;
  }
  if( !level.voices )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice system is not installed on this server\n\"", arg ) );
    return;
  }
  if( !g_voiceChats.integer )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice system administratively disabled on this server\n\"",
      arg ) );
    return;
  }
  if( !Q_stricmp( arg, "vsay" ) )
    vchan = VOICE_CHAN_ALL;
  else if( !Q_stricmp( arg, "vsay_team" ) )
    vchan = VOICE_CHAN_TEAM;
  else if( !Q_stricmp( arg, "vsay_local" ) )
    vchan = VOICE_CHAN_LOCAL;
  else
    return;
  Q_strncpyz( vsay, arg, sizeof( vsay ) );

  if( ent->client->pers.voice[ 0 ] )
    Q_strncpyz( voiceName, ent->client->pers.voice, sizeof( voiceName ) );
  voice = BG_VoiceByName( level.voices, voiceName );
  if( !voice )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice '%s' not found\n\"", vsay, voiceName ) );
    return;
  }

  trap_Argv( 1, voiceCmd, sizeof( voiceCmd ) ) ;
  cmd = BG_VoiceCmdFind( voice->cmds, voiceCmd, &cmdNum );
  if( !cmd )
  {
    trap_SendServerCommand( ent-g_entities, va(
     "print \"%s: command '%s' not found in voice '%s'\n\"",
      vsay, voiceCmd, voiceName ) );
    return;
  }

  // filter non-spec humans by their primary weapon as well
  weapon = WP_NONE;
  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
  {
    weapon = BG_PrimaryWeapon( ent->client->ps.stats );
  }

  track = BG_VoiceTrackFind( cmd->tracks, ent->client->pers.teamSelection,
    ent->client->pers.classSelection, weapon, (int)ent->client->voiceEnthusiasm,
    &trackNum );
  if( !track )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: no available track for command '%s', team %d, "
      "class %d, weapon %d, and enthusiasm %d in voice '%s'\n\"",
      vsay, voiceCmd, ent->client->pers.teamSelection,
      ent->client->pers.classSelection, weapon,
      (int)ent->client->voiceEnthusiasm, voiceName ) );
    return;
  }

  if( !Q_stricmp( ent->client->lastVoiceCmd, cmd->cmd ) )
    ent->client->voiceEnthusiasm++;

  Q_strncpyz( ent->client->lastVoiceCmd, cmd->cmd,
    sizeof( ent->client->lastVoiceCmd ) );

  // optional user supplied text
  trap_Argv( 2, arg, sizeof( arg ) );

  switch( vchan )
  {
    case VOICE_CHAN_ALL:
    case VOICE_CHAN_LOCAL:
      trap_SendServerCommand( -1, va(
        "voice %d %d %d %d \"%s\"\n",
        ent-g_entities, vchan, cmdNum, trackNum, arg ) );
      break;
    case VOICE_CHAN_TEAM:
      G_TeamCommand( ent->client->pers.teamSelection, va(
        "voice %d %d %d %d \"%s\"\n",
        ent-g_entities, vchan, cmdNum, trackNum, arg ) );
      break;
    default:
      break;
  }
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent )
{
  if( !ent->client )
    return;
  trap_SendServerCommand( ent - g_entities,
                          va( "print \"origin: %f %f %f\n\"",
                              ent->s.origin[ 0 ], ent->s.origin[ 1 ],
                              ent->s.origin[ 2 ] ) );
}

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent )
{
  char   cmd[ MAX_TOKEN_CHARS ],
         vote[ MAX_TOKEN_CHARS ],
         arg[ MAX_TOKEN_CHARS ];
  char   name[ MAX_NAME_LENGTH ] = "";
  int    clientNum = -1;
  team_t team;

  trap_Argv( 0, cmd, sizeof( cmd ) );
  trap_Argv( 1, vote, sizeof( vote ) );
  trap_Argv( 2, arg, sizeof( arg ) );

  if( !Q_stricmp( cmd, "callteamvote" ) )
    team = ent->client->pers.teamSelection;
  else
    team = TEAM_NONE;

  if( !g_allowVote.integer )
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"%s: voting not allowed here\n\"", cmd ) );
    return;
  }

  if( level.voteTime[ team ] )
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"%s: a vote is already in progress\n\"", cmd ) );
    return;
  }

  if( level.voteExecuteTime[ team ] )
    G_ExecuteVote( team );

  level.voteThreshold[ team ] = 50;

  if( g_voteLimit.integer > 0 &&
    ent->client->pers.voteCount >= g_voteLimit.integer &&
    !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: you have already called the maximum number of votes (%d)\n\"",
      cmd, g_voteLimit.integer ) );
    return;
  }

  // kick, mute, unmute, denybuild, allowbuild
  if( !Q_stricmp( vote, "kick" ) ||
      !Q_stricmp( vote, "mute" ) || !Q_stricmp( vote, "unmute" ) ||
      !Q_stricmp( vote, "denybuild" ) || !Q_stricmp( vote, "allowbuild" ) )
  {
    int  clientNums[ MAX_CLIENTS ];
    int  matches;
    char err[ MAX_STRING_CHARS ];

    if( !arg[ 0 ] )
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"%s: no target\n\"", cmd ) );
      return;
    }

    // with a little extra work only players from the right team are considered
    matches = G_ClientNumbersFromString( arg, clientNums, MAX_CLIENTS );
    if( matches == 1 )
      clientNum = clientNums[ 0 ];
    else
      clientNum = G_ClientNumberFromString( arg );

    if( clientNum != -1 )
    {
      G_SanitiseString( level.clients[ clientNum ].pers.netname, name, sizeof( name ) );
    }
    else if( matches > 1 )
    {
      G_MatchOnePlayer( clientNums, matches, err, sizeof( err ) );
      ADMP( va( "%s: %s\n", cmd, err ) );
      return;
    }
    else
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"%s: invalid player\n\"", cmd ) );
      return;
    }

    if( !Q_stricmp( vote, "kick" ) || !Q_stricmp( vote, "mute" ) ||
        !Q_stricmp( vote, "denybuild" ) )
    {
      if( G_admin_permission( g_entities + clientNum, ADMF_IMMUNITY ) )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: admin is immune\n\"", cmd ) );
        return;
      }
    }
  }

  if( !Q_stricmp( vote, "kick" ) )
  {
    if( level.clients[ clientNum ].pers.localClient )
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"%s: admin is immune\n\"", cmd ) );
      return;
    }

    Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
      "!ban %s \"1s%s\" vote kick", level.clients[ clientNum ].pers.ip,
      g_adminTempBan.string );
    Com_sprintf( level.voteDisplayString[ team ],
      sizeof( level.voteDisplayString[ team ] ),
      "Kick player '%s'", name );
  }
  else if( team == TEAM_NONE )
  {
    if( !Q_stricmp( vote, "mute" ) )
    {
      if( level.clients[ clientNum ].pers.muted )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: player is already muted\n\"", cmd ) );
        return;
      }

      Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
        "!mute %d", clientNum );
      Com_sprintf( level.voteDisplayString[ team ],
        sizeof( level.voteDisplayString[ team ] ),
        "Mute player '%s'", name );
    }
    else if( !Q_stricmp( vote, "unmute" ) )
    {
      if( !level.clients[ clientNum ].pers.muted )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: player is not currently muted\n\"", cmd ) );
        return;
      }

      Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
        "!unmute %d", clientNum );
      Com_sprintf( level.voteDisplayString[ team ],
        sizeof( level.voteDisplayString[ team ] ),
        "Unmute player '%s'", name );
    }
    else if( !Q_stricmp( vote, "map_restart" ) )
    {
      strcpy( level.voteString[ team ], vote );
      strcpy( level.voteDisplayString[ team ], "Restart current map" );
    }
    else if( !Q_stricmp( vote, "map" ) )
    {
      if( !G_MapExists( arg ) )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: 'maps/%s.bsp' could not be found on the server\n\"",
            cmd, arg ) );
        return;
      }

      Com_sprintf( level.voteString[ team ], sizeof( level.voteString ),
        "%s \"%s\"", vote, arg );
      Com_sprintf( level.voteDisplayString[ team ],
        sizeof( level.voteDisplayString[ team ] ),
        "Change to map '%s'", arg );
    }
    else if( !Q_stricmp( vote, "nextmap" ) )
    {
      if( G_MapExists( g_nextMap.string ) )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: the next map is already set to '%s'\n\"",
            cmd, g_nextMap.string ) );
        return;
      }

      if( !G_MapExists( arg ) )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"%s: 'maps/%s.bsp' could not be found on the server\n\"",
            cmd, arg ) );
        return;
      }

      Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
        "set g_nextMap \"%s\"", arg );
      Com_sprintf( level.voteDisplayString[ team ],
        sizeof( level.voteDisplayString[ team ] ),
        "Set the next map to '%s'", arg );
    }
    else if( !Q_stricmp( vote, "draw" ) )
    {
      strcpy( level.voteString[ team ], "evacuation" );
      strcpy( level.voteDisplayString[ team ], "End match in a draw" );
    }
    else if( !Q_stricmp( vote, "sudden_death" ) )
    {
      if(!g_suddenDeathVotePercent.integer)
      {
        trap_SendServerCommand( ent-g_entities, 
              "print \"Sudden Death votes have been disabled\n\"" );
        return;
      } 
      if( G_TimeTilSuddenDeath( ) <= 0 ) 
      {
        trap_SendServerCommand( ent - g_entities, 
              va( "print \"callvote: Sudden Death has already begun\n\"") );
        return;
      }
      if( level.suddenDeathBeginTime > 0 &&
          G_TimeTilSuddenDeath() <= g_suddenDeathVoteDelay.integer * 1000 )
      {
        trap_SendServerCommand( ent - g_entities, 
              va( "print \"callvote: Sudden Death is imminent\n\"") );
        return;
      }
      level.voteThreshold[ team ] = g_suddenDeathVotePercent.integer;
      Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
        "suddendeath %d", g_suddenDeathVoteDelay.integer );
      strcpy( level.voteDisplayString[ team ],
        va( "Begin sudden death in %d seconds", g_suddenDeathVoteDelay.integer ) );
    }
    else
    {
      trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string\n\"" );
      trap_SendServerCommand( ent-g_entities, "print \"Valid vote commands are: "
        "map, nextmap, map_restart, draw, sudden_death, kick, mute and unmute\n" );
      return;
    }
  }
  else if( !Q_stricmp( vote, "denybuild" ) )
  {
    if( level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"%s: player already lost building rights\n\"", cmd ) );
      return;
    }

    Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
      "!denybuild %d", clientNum );
    Com_sprintf( level.voteDisplayString[ team ],
      sizeof( level.voteDisplayString[ team ] ),
      "Take away building rights from '%s'", name );
  }
  else if( !Q_stricmp( vote, "allowbuild" ) )
  {
    if( !level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"%s: player already has building rights\n\"", cmd ) );
      return;
    }

    Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
      "!allowbuild %d", clientNum );
    Com_sprintf( level.voteDisplayString[ team ],
      sizeof( level.voteDisplayString[ team ] ),
      "Allow '%s' to build", name );
  }
  else if( !Q_stricmp( vote, "admitdefeat" ) )
  {
    Com_sprintf( level.voteString[ team ], sizeof( level.voteString[ team ] ),
      "admitdefeat %d", team );
    strcpy( level.voteDisplayString[ team ], "Admit Defeat" );
  }
  else
  {
    trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string\n\"" );
    trap_SendServerCommand( ent-g_entities,
       "print \"Valid team vote commands are: "
       "kick, denybuild, allowbuild and admitdefeat\n\"" );
    return;
  }

  G_LogPrintf( "%s: %d \"%s" S_COLOR_WHITE "\": %s\n",
    team == TEAM_NONE ? "CallVote" : "CallTeamVote",
    ent - g_entities, ent->client->pers.netname, level.voteString[ team ] );

  if( team == TEAM_NONE )
  {
    trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE
      " called a vote: %s\n\"", ent->client->pers.netname,
      level.voteDisplayString[ team ] ) );
  }
  else
  {
    G_TeamCommand( team, va( "print \"%s" S_COLOR_WHITE
      " called a team vote: %s\n\"", ent->client->pers.netname,
      level.voteDisplayString[ team ] ) );
  }

  level.voteTime[ team ] = level.time;
  trap_SetConfigstring( CS_VOTE_TIME + team,
    va( "%d", level.voteTime[ team ] ) );
  trap_SetConfigstring( CS_VOTE_STRING + team,
    level.voteDisplayString[ team ] );

  ent->client->pers.voteCount++;
  ent->client->pers.vote[ team ] = qtrue;
  G_Vote( ent, team, qtrue );
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent )
{
  char cmd[ MAX_TOKEN_CHARS ], vote[ MAX_TOKEN_CHARS ];
  team_t team = ent->client->pers.teamSelection;

  trap_Argv( 0, cmd, sizeof( cmd ) );
  if( Q_stricmp( cmd, "teamvote" ) )
    team = TEAM_NONE;

  if( !level.voteTime[ team ] )
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"%s: no vote in progress\n\"", cmd ) );
    return;
  }

  if( ent->client->pers.voted[ team ] )
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"%s: vote already cast\n\"", cmd ) );
    return;
  }

  trap_SendServerCommand( ent-g_entities,
    va( "print \"%s: vote cast\n\"", cmd ) );

  trap_Argv( 1, vote, sizeof( vote ) );
  ent->client->pers.vote[ team ] =
    ( tolower( vote[ 0 ] ) == 'y' || vote[ 0 ] == '1' );
  G_Vote( ent, team, qtrue );
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent )
{
  vec3_t  origin, angles;
  char    buffer[ MAX_TOKEN_CHARS ];
  int     i;

  if( trap_Argc( ) != 5 )
  {
    trap_SendServerCommand( ent-g_entities, "print \"usage: setviewpos x y z yaw\n\"" );
    return;
  }

  VectorClear( angles );

  for( i = 0; i < 3; i++ )
  {
    trap_Argv( i + 1, buffer, sizeof( buffer ) );
    origin[ i ] = atof( buffer );
  }

  trap_Argv( 4, buffer, sizeof( buffer ) );
  angles[ YAW ] = atof( buffer );

  TeleportPlayer( ent, origin, angles );
}

#define AS_OVER_RT3         ((ALIENSENSE_RANGE*0.5f)/M_ROOT3)

static qboolean G_RoomForClassChange( gentity_t *ent, class_t class,
                                      vec3_t newOrigin )
{
  vec3_t    fromMins, fromMaxs;
  vec3_t    toMins, toMaxs;
  vec3_t    temp;
  trace_t   tr;
  float     nudgeHeight;
  float     maxHorizGrowth;
  class_t   oldClass = ent->client->ps.stats[ STAT_CLASS ];

  BG_ClassBoundingBox( oldClass, fromMins, fromMaxs, NULL, NULL, NULL );
  BG_ClassBoundingBox( class, toMins, toMaxs, NULL, NULL, NULL );

  VectorCopy( ent->s.origin, newOrigin );

  // find max x/y diff
  maxHorizGrowth = toMaxs[ 0 ] - fromMaxs[ 0 ];
  if( toMaxs[ 1 ] - fromMaxs[ 1 ] > maxHorizGrowth )
    maxHorizGrowth = toMaxs[ 1 ] - fromMaxs[ 1 ];
  if( toMins[ 0 ] - fromMins[ 0 ] > -maxHorizGrowth )
    maxHorizGrowth = -( toMins[ 0 ] - fromMins[ 0 ] );
  if( toMins[ 1 ] - fromMins[ 1 ] > -maxHorizGrowth )
    maxHorizGrowth = -( toMins[ 1 ] - fromMins[ 1 ] );

  if( maxHorizGrowth > 0.0f )
  {
    // test by moving the player up the max required on a 60 degree slope
    nudgeHeight = maxHorizGrowth * 2.0f;
  }
  else
  {
    // player is shrinking, so there's no need to nudge them upwards
    nudgeHeight = 0.0f;
  }

  // find what the new origin would be on a level surface
  newOrigin[ 2 ] -= toMins[ 2 ] - fromMins[ 2 ];

  //compute a place up in the air to start the real trace
  VectorCopy( newOrigin, temp );
  temp[ 2 ] += nudgeHeight;
  trap_Trace( &tr, newOrigin, toMins, toMaxs, temp, ent->s.number, MASK_PLAYERSOLID );

  //trace down to the ground so that we can evolve on slopes
  VectorCopy( newOrigin, temp );
  temp[ 2 ] += ( nudgeHeight * tr.fraction );
  trap_Trace( &tr, temp, toMins, toMaxs, newOrigin, ent->s.number, MASK_PLAYERSOLID );
  VectorCopy( tr.endpos, newOrigin );

  //make REALLY sure
  trap_Trace( &tr, newOrigin, toMins, toMaxs, newOrigin,
    ent->s.number, MASK_PLAYERSOLID );

  //check there is room to evolve
  return ( !tr.startsolid && tr.fraction == 1.0f );
}

/*
=================
Cmd_Class_f
=================
*/
void Cmd_Class_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int       clientNum;
  int       i;
  vec3_t    infestOrigin;
  class_t   currentClass = ent->client->pers.classSelection;
  class_t   newClass;
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { AS_OVER_RT3, AS_OVER_RT3, AS_OVER_RT3 };
  vec3_t    mins, maxs;
  int       num;
  gentity_t *other;
  int       oldBoostTime = -1;
  vec3_t    oldVel;

  clientNum = ent->client - level.clients;
  trap_Argv( 1, s, sizeof( s ) );
  newClass = BG_ClassByName( s )->number;

  if( ent->client->sess.spectatorState != SPECTATOR_NOT )
  {
    if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
      G_StopFollowing( ent );
    if( ent->client->pers.teamSelection == TEAM_ALIENS )
    {
      if( newClass != PCL_ALIEN_BUILDER0 &&
          newClass != PCL_ALIEN_BUILDER0_UPG &&
          newClass != PCL_ALIEN_LEVEL0 )
      {
        G_TriggerMenuArgs( ent->client->ps.clientNum, MN_A_CLASSNOTSPAWN, newClass );
        return;
      }

      if( !BG_ClassIsAllowed( newClass ) )
      {
        G_TriggerMenuArgs( ent->client->ps.clientNum, MN_A_CLASSNOTALLOWED, newClass );
        return;
      }

      if( !BG_ClassAllowedInStage( newClass, g_alienStage.integer ) )
      {
        G_TriggerMenuArgs( ent->client->ps.clientNum, MN_A_CLASSNOTATSTAGE, newClass );
        return;
      }

      // spawn from an egg
      if( G_PushSpawnQueue( &level.alienSpawnQueue, clientNum ) )
      {
        ent->client->pers.classSelection = newClass;
        ent->client->ps.stats[ STAT_CLASS ] = newClass;
      }
    }
    else if( ent->client->pers.teamSelection == TEAM_HUMANS )
    {
      //set the item to spawn with
      if( !Q_stricmp( s, BG_Weapon( WP_MACHINEGUN )->name ) &&
          BG_WeaponIsAllowed( WP_MACHINEGUN ) )
      {
        ent->client->pers.humanItemSelection = WP_MACHINEGUN;
      }
      else if( !Q_stricmp( s, BG_Weapon( WP_HBUILD )->name ) &&
               BG_WeaponIsAllowed( WP_HBUILD ) )
      {
        ent->client->pers.humanItemSelection = WP_HBUILD;
      }
      else
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNSPAWNITEM );
        return;
      }
      // spawn from a telenode
      if( G_PushSpawnQueue( &level.humanSpawnQueue, clientNum ) )
      {
        ent->client->pers.classSelection = PCL_HUMAN;
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
      }
    }
    return;
  }

  if( ent->health <= 0 )
    return;

  if( ent->client->pers.teamSelection == TEAM_ALIENS &&
      !( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING ) )
  {
    if( newClass == PCL_NONE )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_A_UNKNOWNCLASS );
      return;
    }

    //if we are not currently spectating, we are attempting evolution
    if( ent->client->pers.classSelection != PCL_NONE )
    {
      int cost;

      //check that we have an overmind
      if( !G_Overmind( ) )
      {
        G_TriggerMenu( clientNum, MN_A_NOOVMND_EVOLVE );
        return;
      }

      //check there are no humans nearby
      VectorAdd( ent->client->ps.origin, range, maxs );
      VectorSubtract( ent->client->ps.origin, range, mins );

      num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
      for( i = 0; i < num; i++ )
      {
        other = &g_entities[ entityList[ i ] ];

        if( ( other->client && other->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ) ||
            ( other->s.eType == ET_BUILDABLE && other->buildableTeam == TEAM_HUMANS &&
              other->powered ) )
        {
          G_TriggerMenu( clientNum, MN_A_TOOCLOSE );
          return;
        }
      }
      
      //check that we are not wallwalking
      if( ent->client->ps.eFlags & EF_WALLCLIMB )
      {
        G_TriggerMenu( clientNum, MN_A_EVOLVEWALLWALK );
        return;
      }

      //guard against selling the HBUILD weapons exploit
      if( ent->client->sess.spectatorState == SPECTATOR_NOT &&
          ( currentClass == PCL_ALIEN_BUILDER0 ||
            currentClass == PCL_ALIEN_BUILDER0_UPG ) &&
          ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_A_EVOLVEBUILDTIMER );
        return;
      }

      cost = BG_ClassCanEvolveFromTo( currentClass, newClass,
                                      ent->client->pers.credit,
                                      g_alienStage.integer, 0 );

      if( G_RoomForClassChange( ent, newClass, infestOrigin ) )
      {
        if( cost >= 0 )
        {

          ent->client->pers.evolveHealthFraction = (float)ent->client->ps.stats[ STAT_HEALTH ] /
            (float)BG_Class( currentClass )->health;

          if( ent->client->pers.evolveHealthFraction < 0.0f )
            ent->client->pers.evolveHealthFraction = 0.0f;
          else if( ent->client->pers.evolveHealthFraction > 1.0f )
            ent->client->pers.evolveHealthFraction = 1.0f;

          //remove credit
          G_AddCreditToClient( ent->client, -cost, qtrue );
          ent->client->pers.classSelection = newClass;
          ClientUserinfoChanged( clientNum );
          VectorCopy( infestOrigin, ent->s.pos.trBase );
          VectorCopy( ent->client->ps.velocity, oldVel );

          if( ent->client->ps.stats[ STAT_STATE ] & SS_BOOSTED )
            oldBoostTime = ent->client->boostedTime;

          ClientSpawn( ent, ent, ent->s.pos.trBase, ent->s.apos.trBase );

          VectorCopy( oldVel, ent->client->ps.velocity );
          if( oldBoostTime > 0 )
          {
            ent->client->boostedTime = oldBoostTime;
            ent->client->ps.stats[ STAT_STATE ] |= SS_BOOSTED;
          }
        }
        else
          G_TriggerMenuArgs( clientNum, MN_A_CANTEVOLVE, newClass );
      }
      else
        G_TriggerMenu( clientNum, MN_A_NOEROOM );
    }
  }
  else if( ent->client->pers.teamSelection == TEAM_HUMANS )
    G_TriggerMenu( clientNum, MN_H_DEADTOCLASS );
}


/*
=================
Cmd_Destroy_f
=================
*/
void Cmd_Destroy_f( gentity_t *ent )
{
  vec3_t      forward, end;
  trace_t     tr;
  gentity_t   *traceEnt;
  char        cmd[ 12 ];
  qboolean    deconstruct = qtrue;

  if( ent->client->pers.denyBuild )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_REVOKED );
    return;
  }

  trap_Argv( 0, cmd, sizeof( cmd ) );
  if( Q_stricmp( cmd, "destroy" ) == 0 )
    deconstruct = qfalse;

  if( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
    G_Damage( ent->client->hovel, ent, ent, forward, ent->s.origin, 10000, 0, MOD_SUICIDE );

  AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
  VectorMA( ent->client->ps.origin, 100, forward, end );

  trap_Trace( &tr, ent->client->ps.origin, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
  traceEnt = &g_entities[ tr.entityNum ];

  if( tr.fraction < 1.0f &&
      ( traceEnt->s.eType == ET_BUILDABLE ) &&
      ( traceEnt->buildableTeam == ent->client->pers.teamSelection ) &&
      ( ( ent->client->ps.weapon >= WP_ABUILD ) &&
        ( ent->client->ps.weapon <= WP_HBUILD ) ) )
  {
    // Always let the builder prevent the explosion 
    if( traceEnt->health <= 0 )
    {
      G_QueueBuildPoints( traceEnt );
      G_FreeEntity( traceEnt );
      return;
    }

    // Cancel deconstruction
    if( deconstruct && g_markDeconstruct.integer && traceEnt->deconstruct )
    {
      traceEnt->deconstruct = qfalse;
      return;
    }

    // Prevent destruction of the last spawn
    if( !g_cheats.integer &&
        ( !g_markDeconstruct.integer || !G_FindPower( traceEnt ) ) )
    {
      if( ent->client->pers.teamSelection == TEAM_ALIENS &&
          traceEnt->s.modelindex == BA_A_SPAWN )
      {
        if( level.numAlienSpawns <= 1 )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_B_LASTSPAWN );
          return;
        }
      }
      else if( ent->client->pers.teamSelection == TEAM_HUMANS &&
               traceEnt->s.modelindex == BA_H_SPAWN )
      {
        if( level.numHumanSpawns <= 1 )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_B_LASTSPAWN );
          return;
        }
      }
    }

    // Don't allow destruction of hovel with granger inside
    if( traceEnt->s.modelindex == BA_A_HOVEL && traceEnt->active )
      return;

    // Don't allow destruction of buildables that cannot be rebuilt
    if( G_TimeTilSuddenDeath( ) <= 0 )
      return;

    if( !g_markDeconstruct.integer ||
        ( ent->client->pers.teamSelection == TEAM_HUMANS &&
          !G_FindPower( traceEnt ) ) )
    {
      if( ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_AddEvent( ent, EV_BUILD_DELAY, ent->client->ps.clientNum );
        return;
      }
    }

    if( traceEnt->health > 0 )
    {
      if( !deconstruct )
      {
        G_Damage( traceEnt, ent, ent, forward, tr.endpos,
                  traceEnt->health, 0, MOD_SUICIDE );
      }
      else if( g_markDeconstruct.integer &&
               ( ent->client->pers.teamSelection != TEAM_HUMANS ||
                 G_FindPower( traceEnt ) ) )
      {
        traceEnt->deconstruct     = qtrue; // Mark buildable for deconstruction
        traceEnt->deconstructTime = level.time;
      }
      else
      {
        if( !g_cheats.integer ) // add a bit to the build timer
        {
            ent->client->ps.stats[ STAT_MISC ] +=
              BG_Buildable( traceEnt->s.modelindex )->buildTime / 4;
        }
        G_LogDestruction( traceEnt, ent, MOD_DECONSTRUCT );
        G_FreeEntity( traceEnt );
      }
    }
  }
}

/*
=================
Cmd_ActivateItem_f

Activate an item
=================
*/
void Cmd_ActivateItem_f( gentity_t *ent )
{
  char  s[ MAX_TOKEN_CHARS ];
  int   upgrade, weapon;
  
  trap_Argv( 1, s, sizeof( s ) );
  
  // "weapon" aliased to whatever weapon you have
  if( !Q_stricmp( "weapon", s ) )
  {
    if( ent->client->ps.weapon == WP_BLASTER &&
        BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      G_ForceWeaponChange( ent, WP_NONE );  
    return;
  }
  
  upgrade = BG_UpgradeByName( s )->number;
  weapon = BG_WeaponByName( s )->number;

  if( upgrade != UP_NONE && BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    BG_ActivateUpgrade( upgrade, ent->client->ps.stats );
  else if( weapon != WP_NONE &&
           BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
  {
    if( ent->client->ps.weapon != weapon &&
        BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      G_ForceWeaponChange( ent, weapon );
  }
  else
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
}


/*
=================
Cmd_DeActivateItem_f

Deactivate an item
=================
*/
void Cmd_DeActivateItem_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  upgrade_t upgrade;

  trap_Argv( 1, s, sizeof( s ) );
  upgrade = BG_UpgradeByName( s )->number;

  if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    BG_DeactivateUpgrade( upgrade, ent->client->ps.stats );
  else
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
}


/*
=================
Cmd_ToggleItem_f
=================
*/
void Cmd_ToggleItem_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  weapon_t  weapon;
  upgrade_t upgrade;

  trap_Argv( 1, s, sizeof( s ) );
  upgrade = BG_UpgradeByName( s )->number;
  weapon = BG_WeaponByName( s )->number;

  if( weapon != WP_NONE )
  {
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      return;

    //special case to allow switching between
    //the blaster and the primary weapon
    if( ent->client->ps.weapon != WP_BLASTER )
      weapon = WP_BLASTER;
    else
      weapon = WP_NONE;

    G_ForceWeaponChange( ent, weapon );
  }
  else if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
  {
    if( BG_UpgradeIsActive( upgrade, ent->client->ps.stats ) )
      BG_DeactivateUpgrade( upgrade, ent->client->ps.stats );
    else
      BG_ActivateUpgrade( upgrade, ent->client->ps.stats );
  }
  else
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
}

/*
=================
Cmd_Buy_f
=================
*/
void Cmd_Buy_f( gentity_t *ent )
{
  char s[ MAX_TOKEN_CHARS ];
  weapon_t  weapon;
  upgrade_t upgrade;
  qboolean  energyOnly;

  trap_Argv( 1, s, sizeof( s ) );

  weapon = BG_WeaponByName( s )->number;
  upgrade = BG_UpgradeByName( s )->number;

  // Only give energy from reactors or repeaters
  if( G_BuildableRange( ent->client->ps.origin, 100, BA_H_ARMOURY ) )
    energyOnly = qfalse;
  else if( upgrade == UP_AMMO &&
           BG_Weapon( ent->client->ps.stats[ STAT_WEAPON ] )->usesEnergy &&
           ( G_BuildableRange( ent->client->ps.origin, 100, BA_H_REACTOR ) ||
             G_BuildableRange( ent->client->ps.origin, 100, BA_H_REPEATER ) ) )
    energyOnly = qtrue;
  else
  {
    if( upgrade == UP_AMMO &&
        BG_Weapon( ent->client->ps.weapon )->usesEnergy )
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOENERGYAMMOHERE );
    else
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
    return;
  }
  
  if( weapon != WP_NONE )
  {
    //already got this?
    if( BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_ITEMHELD );
      return;
    }

    // Only humans can buy stuff
    if( BG_Weapon( weapon )->team != TEAM_HUMANS )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy alien items\n\"" );
      return;
    }

    //are we /allowed/ to buy this?
    if( !BG_Weapon( weapon )->purchasable )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item\n\"" );
      return;
    }

    //are we /allowed/ to buy this?
    if( !BG_WeaponAllowedInStage( weapon, g_humanStage.integer ) || !BG_WeaponIsAllowed( weapon ) )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item\n\"" );
      return;
    }

    //can afford this?
    if( BG_Weapon( weapon )->price > (short)ent->client->pers.credit )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOFUNDS );
      return;
    }

    //have space to carry this?
    if( BG_Weapon( weapon )->slots & BG_SlotsForInventory( ent->client->ps.stats ) )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOSLOTS );
      return;
    }

    // In some instances, weapons can't be changed
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      return;

    ent->client->ps.stats[ STAT_WEAPON ] = weapon;
    ent->client->ps.ammo = BG_Weapon( weapon )->maxAmmo;
    ent->client->ps.clips = BG_Weapon( weapon )->maxClips;

    if( BG_Weapon( weapon )->usesEnergy &&
        BG_InventoryContainsUpgrade( UP_BATTPACK, ent->client->ps.stats ) )
      ent->client->ps.ammo *= BATTPACK_MODIFIER;

    G_ForceWeaponChange( ent, weapon );

    //set build delay/pounce etc to 0
    ent->client->ps.stats[ STAT_MISC ] = 0;

    //subtract from funds
    G_AddCreditToClient( ent->client, -(short)BG_Weapon( weapon )->price, qfalse );
  }
  else if( upgrade != UP_NONE )
  {
    //already got this?
    if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_ITEMHELD );
      return;
    }

    //can afford this?
    if( BG_Upgrade( upgrade )->price > (short)ent->client->pers.credit )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOFUNDS );
      return;
    }

    //have space to carry this?
    if( BG_Upgrade( upgrade )->slots & BG_SlotsForInventory( ent->client->ps.stats ) )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOSLOTS );
      return;
    }

    // Only humans can buy stuff
    if( BG_Upgrade( upgrade )->team != TEAM_HUMANS )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy alien items\n\"" );
      return;
    }

    //are we /allowed/ to buy this?
    if( !BG_Upgrade( upgrade )->purchasable )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item\n\"" );
      return;
    }

    //are we /allowed/ to buy this?
    if( !BG_UpgradeAllowedInStage( upgrade, g_humanStage.integer ) || !BG_UpgradeIsAllowed( upgrade ) )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item\n\"" );
      return;
    }

    if( upgrade == UP_AMMO )
      G_GiveClientMaxAmmo( ent, energyOnly );
    else
    {
      if( upgrade == UP_BATTLESUIT )
      {
        vec3_t newOrigin;

        if( !G_RoomForClassChange( ent, PCL_HUMAN_BSUIT, newOrigin ) )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITON );
          return;
        }
        VectorCopy( newOrigin, ent->client->ps.origin );
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN_BSUIT;
        ent->client->pers.classSelection = PCL_HUMAN_BSUIT;
        ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
      }

      //add to inventory
      BG_AddUpgradeToInventory( upgrade, ent->client->ps.stats );
    }

    if( upgrade == UP_BATTPACK )
      G_GiveClientMaxAmmo( ent, qtrue );

    //subtract from funds
    G_AddCreditToClient( ent->client, -(short)BG_Upgrade( upgrade )->price, qfalse );
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNITEM );

  //update ClientInfo
  ClientUserinfoChanged( ent->client->ps.clientNum );
}


/*
=================
Cmd_Sell_f
=================
*/
void Cmd_Sell_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int       i;
  weapon_t  weapon;
  upgrade_t upgrade;

  trap_Argv( 1, s, sizeof( s ) );

  //no armoury nearby
  if( !G_BuildableRange( ent->client->ps.origin, 100, BA_H_ARMOURY ) )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
    return;
  }

  if( !Q_stricmpn( s, "weapon", 6 ) )
    weapon = ent->client->ps.stats[ STAT_WEAPON ];
  else
    weapon = BG_WeaponByName( s )->number;

  upgrade = BG_UpgradeByName( s )->number;

  if( weapon != WP_NONE )
  {
    weapon_t selected = BG_GetPlayerWeapon( &ent->client->ps );
  
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      return;
  
    //are we /allowed/ to sell this?
    if( !BG_Weapon( weapon )->purchasable )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't sell this weapon\n\"" );
      return;
    }

    //remove weapon if carried
    if( BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
    {
      //guard against selling the HBUILD weapons exploit
      if( weapon == WP_HBUILD && ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ARMOURYBUILDTIMER );
        return;
      }

      ent->client->ps.stats[ STAT_WEAPON ] = WP_NONE;

      //add to funds
      G_AddCreditToClient( ent->client, (short)BG_Weapon( weapon )->price, qfalse );
    }

    //if we have this weapon selected, force a new selection
    if( weapon == selected )
      G_ForceWeaponChange( ent, WP_NONE );
  }
  else if( upgrade != UP_NONE )
  {
    //are we /allowed/ to sell this?
    if( !BG_Upgrade( upgrade )->purchasable )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't sell this item\n\"" );
      return;
    }
    //remove upgrade if carried
    if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    {
      // shouldn't really need to test for this, but just to be safe
      if( upgrade == UP_BATTLESUIT )
      {
        vec3_t newOrigin;

        if( !G_RoomForClassChange( ent, PCL_HUMAN, newOrigin ) )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITOFF );
          return;
        }
        VectorCopy( newOrigin, ent->client->ps.origin );
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
        ent->client->pers.classSelection = PCL_HUMAN;
        ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
      }

      //add to inventory
      BG_RemoveUpgradeFromInventory( upgrade, ent->client->ps.stats );

      if( upgrade == UP_BATTPACK )
        G_GiveClientMaxAmmo( ent, qtrue );

      //add to funds
      G_AddCreditToClient( ent->client, (short)BG_Upgrade( upgrade )->price, qfalse );
    }
  }
  else if( !Q_stricmp( s, "weapons" ) )
  {
    weapon_t selected = BG_GetPlayerWeapon( &ent->client->ps );

    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      return;

    for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    {
      //guard against selling the HBUILD weapons exploit
      if( i == WP_HBUILD && ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ARMOURYBUILDTIMER );
        continue;
      }

      if( BG_InventoryContainsWeapon( i, ent->client->ps.stats ) &&
          BG_Weapon( i )->purchasable )
      {
        ent->client->ps.stats[ STAT_WEAPON ] = WP_NONE;

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Weapon( i )->price, qfalse );
      }

      //if we have this weapon selected, force a new selection
      if( i == selected )
        G_ForceWeaponChange( ent, WP_NONE );
    }
  }
  else if( !Q_stricmp( s, "upgrades" ) )
  {
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
      //remove upgrade if carried
      if( BG_InventoryContainsUpgrade( i, ent->client->ps.stats ) &&
          BG_Upgrade( i )->purchasable )
      {

        // shouldn't really need to test for this, but just to be safe
        if( i == UP_BATTLESUIT )
        {
          vec3_t newOrigin;

          if( !G_RoomForClassChange( ent, PCL_HUMAN, newOrigin ) )
          {
            G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITOFF );
            continue;
          }
          VectorCopy( newOrigin, ent->client->ps.origin );
          ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
          ent->client->pers.classSelection = PCL_HUMAN;
          ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
        }

        BG_RemoveUpgradeFromInventory( i, ent->client->ps.stats );

        if( i == UP_BATTPACK )
          G_GiveClientMaxAmmo( ent, qtrue );

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Upgrade( i )->price, qfalse );
      }
    }
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNITEM );

  //update ClientInfo
  ClientUserinfoChanged( ent->client->ps.clientNum );
}


/*
=================
Cmd_Build_f
=================
*/
void Cmd_Build_f( gentity_t *ent )
{
  char          s[ MAX_TOKEN_CHARS ];
  buildable_t   buildable;
  float         dist;
  vec3_t        origin;
  team_t        team;

  if( ent->client->pers.denyBuild )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_REVOKED );
    return;
  }

  if( ent->client->pers.teamSelection == level.surrenderTeam )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_SURRENDER );
    return;
  }

  trap_Argv( 1, s, sizeof( s ) );

  buildable = BG_BuildableByName( s )->number;

  if( G_TimeTilSuddenDeath( ) <= 0 )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_SUDDENDEATH );
    return;
  }

  team = ent->client->ps.stats[ STAT_TEAM ];

  if( buildable != BA_NONE &&
      ( ( 1 << ent->client->ps.weapon ) & BG_Buildable( buildable )->buildWeapon ) &&
      !( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING ) &&
      BG_BuildableIsAllowed( buildable ) &&
      ( ( team == TEAM_ALIENS && BG_BuildableAllowedInStage( buildable, g_alienStage.integer ) ) ||
        ( team == TEAM_HUMANS && BG_BuildableAllowedInStage( buildable, g_humanStage.integer ) ) ) )
  {
    dynMenu_t err;
    dist = BG_Class( ent->client->ps.stats[ STAT_CLASS ] )->buildDist;

    ent->client->ps.stats[ STAT_BUILDABLE ] = 0;

    //these are the errors displayed when the builder first selects something to use
    switch( G_CanBuild( ent, buildable, dist, origin ) )
    {
      // can place right away, set the blueprint and the valid togglebit
      case IBE_NONE:
      case IBE_TNODEWARN:
      case IBE_RPTNOREAC:
      case IBE_RPTPOWERHERE:
      case IBE_SPWNWARN:
        err = MN_NONE;
        // we OR-in the selected builable later
        ent->client->ps.stats[ STAT_BUILDABLE ] = SB_VALID_TOGGLEBIT;
        break;

      // can't place yet but maybe soon: start with valid togglebit off
      case IBE_NORMAL:
      case IBE_HOVELEXIT:
      case IBE_NOCREEP:
      case IBE_NOROOM:
      case IBE_NOOVERMIND:
      case IBE_NOPOWERHERE:
        err = MN_NONE;
        break;

      // more serious errors just pop a menu
      case IBE_NOALIENBP:
        err = MN_A_NOBP;
        break;

      case IBE_ONEOVERMIND:
        err = MN_A_ONEOVERMIND;
        break;

      case IBE_ONEHOVEL:
        err = MN_A_ONEHOVEL;
        break;

      case IBE_ONEREACTOR:
        err = MN_H_ONEREACTOR;
        break;

      case IBE_NOHUMANBP:
        err = MN_H_NOBP;
        break;

      case IBE_NODCC:
        err = MN_H_NODCC;
        break;

      case IBE_PERMISSION:
        err = MN_B_CANNOT;
        break;

      case IBE_LASTSPAWN:
        err = MN_B_LASTSPAWN;
        break;

      default:
        err = -1; // stop uninitialised warning
        break;
    }

    if( err == MN_NONE || ent->client->pers.disableBlueprintErrors )
      ent->client->ps.stats[ STAT_BUILDABLE ] |= buildable;
    else
      G_TriggerMenu( ent->client->ps.clientNum, err );
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_CANNOT );
}

/*
=================
Cmd_Reload_f
=================
*/
void Cmd_Reload_f( gentity_t *ent )
{
  playerState_t *ps = &ent->client->ps;
  int ammo;

  // weapon doesn't ever need reloading
  if( BG_Weapon( ps->weapon )->infiniteAmmo )
    return;

  if( ps->clips <= 0 )
    return;

  if( BG_Weapon( ps->weapon )->usesEnergy &&
      BG_InventoryContainsUpgrade( UP_BATTPACK, ps->stats ) )
    ammo = BG_Weapon( ps->weapon )->maxAmmo * BATTPACK_MODIFIER;
  else
    ammo = BG_Weapon( ps->weapon )->maxAmmo;

  // don't reload when full
  if( ps->ammo >= ammo )
    return;

  // the animation, ammo refilling etc. is handled by PM_Weapon
  if( ent->client->ps.weaponstate != WEAPON_RELOADING )
    ent->client->ps.pm_flags |= PMF_WEAPON_RELOAD;
}

/*
=================
G_StopFromFollowing

stops any other clients from following this one
called when a player leaves a team or dies
=================
*/
void G_StopFromFollowing( gentity_t *ent )
{
  int i;

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].sess.spectatorState == SPECTATOR_FOLLOW &&
        level.clients[ i ].sess.spectatorClient == ent->client->ps.clientNum )
    {
      if( !G_FollowNewClient( &g_entities[ i ], 1 ) )
        G_StopFollowing( &g_entities[ i ] );
    }
  }
}

/*
=================
G_StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void G_StopFollowing( gentity_t *ent )
{
  ent->client->ps.stats[ STAT_TEAM ] = ent->client->pers.teamSelection;

  if( ent->client->pers.teamSelection == TEAM_NONE )
  {
    ent->client->sess.spectatorState = 
      ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_FREE;
  }
  else
  {
    vec3_t spawn_origin, spawn_angles;

    ent->client->sess.spectatorState =
      ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_LOCKED;

    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      G_SelectAlienLockSpawnPoint( spawn_origin, spawn_angles );
    else if( ent->client->pers.teamSelection == TEAM_HUMANS )
      G_SelectHumanLockSpawnPoint( spawn_origin, spawn_angles );

    G_SetOrigin( ent, spawn_origin );
    VectorCopy( spawn_origin, ent->client->ps.origin );
    G_SetClientViewAngle( ent, spawn_angles );
  }
  ent->client->sess.spectatorClient = -1;
  ent->client->ps.pm_flags &= ~PMF_FOLLOW;
  ent->client->ps.stats[ STAT_STATE ] &= ~SS_WALLCLIMBING;
  ent->client->ps.stats[ STAT_VIEWLOCK ] = 0;
  ent->client->ps.eFlags &= ~( EF_WALLCLIMB | EF_WALLCLIMBCEILING );
  ent->client->ps.viewangles[ PITCH ] = 0.0f;
  ent->client->ps.clientNum = ent - g_entities;
  ent->client->ps.persistant[ PERS_CREDIT ] = ent->client->pers.credit;

  CalculateRanks( );
}

/*
=================
G_FollowLockView

Client is still following a player, but that player has gone to spectator
mode and cannot be followed for the moment
=================
*/
void G_FollowLockView( gentity_t *ent )
{
  vec3_t spawn_origin, spawn_angles;
  int clientNum;
  
  clientNum = ent->client->sess.spectatorClient;
  ent->client->sess.spectatorState =
    ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_FOLLOW;
  ent->client->ps.clientNum = clientNum;
  ent->client->ps.pm_flags &= ~PMF_FOLLOW;
  ent->client->ps.stats[ STAT_TEAM ] = ent->client->pers.teamSelection;
  ent->client->ps.stats[ STAT_STATE ] &= ~SS_WALLCLIMBING;
  ent->client->ps.stats[ STAT_VIEWLOCK ] = 0;
  ent->client->ps.eFlags &= ~( EF_WALLCLIMB | EF_WALLCLIMBCEILING );
  ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
  ent->client->ps.viewangles[ PITCH ] = 0.0f;

  // Put the view at the team spectator lock position
  if( level.clients[ clientNum ].pers.teamSelection == TEAM_ALIENS )
    G_SelectAlienLockSpawnPoint( spawn_origin, spawn_angles );
  else if( level.clients[ clientNum ].pers.teamSelection == TEAM_HUMANS )
    G_SelectHumanLockSpawnPoint( spawn_origin, spawn_angles );

  G_SetOrigin( ent, spawn_origin );
  VectorCopy( spawn_origin, ent->client->ps.origin );
  G_SetClientViewAngle( ent, spawn_angles );
}

/*
=================
G_FollowNewClient

This was a really nice, elegant function. Then I fucked it up.
=================
*/
qboolean G_FollowNewClient( gentity_t *ent, int dir )
{
  int       clientnum = ent->client->sess.spectatorClient;
  int       original = clientnum;
  qboolean  selectAny = qfalse;

  if( dir > 1 )
    dir = 1;
  else if( dir < -1 )
    dir = -1;
  else if( dir == 0 )
    return qtrue;

  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
    return qfalse;

  // select any if no target exists
  if( clientnum < 0 || clientnum >= level.maxclients )
  {
    clientnum = original = 0;
    selectAny = qtrue;
  }

  do
  {
    clientnum += dir;

    if( clientnum >= level.maxclients )
      clientnum = 0;

    if( clientnum < 0 )
      clientnum = level.maxclients - 1;

    // can't follow self
    if( &g_entities[ clientnum ] == ent )
      continue;

    // avoid selecting existing follow target
    if( clientnum == original && !selectAny )
      continue; //effectively break;

    // can only follow connected clients
    if( level.clients[ clientnum ].pers.connected != CON_CONNECTED )
      continue;

    // can't follow a spectator
    if( level.clients[ clientnum ].pers.teamSelection == TEAM_NONE )
      continue;
    
    // if stickyspec is disabled, can't follow someone in queue either
    if( !ent->client->pers.stickySpec &&
        level.clients[ clientnum ].sess.spectatorState != SPECTATOR_NOT )
      continue;
    
    // can only follow teammates when dead and on a team
    if( ent->client->pers.teamSelection != TEAM_NONE && 
        ( level.clients[ clientnum ].pers.teamSelection != 
          ent->client->pers.teamSelection ) )
      continue;
    
    // this is good, we can use it
    ent->client->sess.spectatorClient = clientnum;
    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    
    // if this client is in the spawn queue, we need to do something special
    if( level.clients[ clientnum ].sess.spectatorState != SPECTATOR_NOT )
      G_FollowLockView( ent );
    
    return qtrue;

  } while( clientnum != original );

  return qfalse;
}

/*
=================
G_ToggleFollow
=================
*/
void G_ToggleFollow( gentity_t *ent )
{
  if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    G_StopFollowing( ent );
  else
    G_FollowNewClient( ent, 1 );
}

/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent )
{
  int   i;
  int   pids[ MAX_CLIENTS ];
  char  arg[ MAX_NAME_LENGTH ];

  // won't work unless spectating
  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
    return;

  if( trap_Argc( ) != 2 )
  {
    G_ToggleFollow( ent );
  }
  else 
  {
    trap_Argv( 1, arg, sizeof( arg ) );
    if( G_ClientNumbersFromString( arg, pids, MAX_CLIENTS ) == 1 )
    {
      i = pids[ 0 ];
    }
    else
    {
      i = G_ClientNumberFromString( arg );

      if( i == -1 )
      {
        trap_SendServerCommand( ent - g_entities,
          "print \"follow: invalid player\n\"" );
        return;
      }
    }

    // can't follow self
    if( &level.clients[ i ] == ent->client )
      return;

    // can't follow another spectator if sticky spec is off
    if( !ent->client->pers.stickySpec &&
        level.clients[ i ].sess.spectatorState != SPECTATOR_NOT )
      return;

    // if not on team spectator, you can only follow teammates
    if( ent->client->pers.teamSelection != TEAM_NONE && 
        ( level.clients[ i ].pers.teamSelection != 
          ent->client->pers.teamSelection ) )
      return;

    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    ent->client->sess.spectatorClient = i;
  }
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent )
{
  char args[ 11 ];
  int  dir = 1;

  trap_Argv( 0, args, sizeof( args ) );
  if( Q_stricmp( args, "followprev" ) == 0 )
    dir = -1;

  // won't work unless spectating
  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
    return;

  G_FollowNewClient( ent, dir );
}

/*
=================
Cmd_PTRCVerify_f

Check a PTR code is valid
=================
*/
void Cmd_PTRCVerify_f( gentity_t *ent )
{
  connectionRecord_t  *connection;
  char                s[ MAX_TOKEN_CHARS ] = { 0 };
  int                 code;

  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
    return;

  code = atoi( s );

  if( G_VerifyPTRC( code ) )
  {
    connection = G_FindConnectionForCode( code );

    // valid code
    if( connection->clientTeam != TEAM_NONE )
      trap_SendServerCommand( ent->client->ps.clientNum, "ptrcconfirm" );

    // restore mapping
    ent->client->pers.connection = connection;
  }
  else
  {
    // invalid code -- generate a new one
    connection = G_GenerateNewConnection( ent->client );

    if( connection )
    {
      trap_SendServerCommand( ent->client->ps.clientNum,
        va( "ptrcissue %d", connection->ptrCode ) );
    }
  }
}

/*
=================
Cmd_PTRCRestore_f

Restore against a PTR code
=================
*/
void Cmd_PTRCRestore_f( gentity_t *ent )
{
  char                s[ MAX_TOKEN_CHARS ] = { 0 };
  int                 code;
  connectionRecord_t  *connection;

  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
    return;

  code = atoi( s );

  if( G_VerifyPTRC( code ) )
  {
    if( ent->client->pers.joinedATeam )
    {
      trap_SendServerCommand( ent - g_entities,
        "print \"You cannot use a PTR code after joining a team\n\"" );
    }
    else
    {
      // valid code
      connection = G_FindConnectionForCode( code );

      if( connection )
      {
        // set the correct team
        G_ChangeTeam( ent, connection->clientTeam );

        // set the correct credit
        ent->client->pers.credit = 0;
        G_AddCreditToClient( ent->client, connection->clientCredit, qtrue );
      }
    }
  }
  else
  {
    trap_SendServerCommand( ent - g_entities,
      va( "print \"'%d' is not a valid PTR code\n\"", code ) );
  }
}

static void Cmd_Ignore_f( gentity_t *ent )
{
  int pids[ MAX_CLIENTS ];
  char name[ MAX_NAME_LENGTH ];
  char cmd[ 9 ];
  int matches = 0;
  int i;
  qboolean ignore = qfalse;

  trap_Argv( 0, cmd, sizeof( cmd ) );
  if( Q_stricmp( cmd, "ignore" ) == 0 )
    ignore = qtrue;

  if( trap_Argc() < 2 )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
      "usage: %s [clientNum | partial name match]\n\"", cmd ) );
    return;
  }

  Q_strncpyz( name, ConcatArgs( 1 ), sizeof( name ) );
  matches = G_ClientNumbersFromString( name, pids, MAX_CLIENTS );
  if( matches < 1 )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
      "%s: no clients match the name '%s'\n\"", cmd, name ) );
    return;
  }

  for( i = 0; i < matches; i++ )
  {
    if( ignore )
    {
      if( !Com_ClientListContains( &ent->client->sess.ignoreList, pids[ i ] ) )
      {
        Com_ClientListAdd( &ent->client->sess.ignoreList, pids[ i ] );
        ClientUserinfoChanged( ent->client->ps.clientNum );
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "ignore: added %s^7 to your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
      else
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "ignore: %s^7 is already on your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
    }
    else
    {
      if( Com_ClientListContains( &ent->client->sess.ignoreList, pids[ i ] ) )
      {
        Com_ClientListRemove( &ent->client->sess.ignoreList, pids[ i ] );
        ClientUserinfoChanged( ent->client->ps.clientNum );
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "unignore: removed %s^7 from your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
      else
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "unignore: %s^7 is not on your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
    }
  }
}

/*
=================
Cmd_Test_f
=================
*/
void Cmd_Test_f( gentity_t *humanPlayer )
{
}

/*
=================
Cmd_Damage_f

Deals damage to you (for testing), arguments: [damage] [dx] [dy] [dz]
The dx/dy arguments describe the damage point's offset from the entity origin
=================
*/
void Cmd_Damage_f( gentity_t *ent )
{
  vec3_t point;
  char arg[ 16 ];
  float dx = 0.0f, dy = 0.0f, dz = 100.0f;
  int damage = 100;
  qboolean nonloc = qtrue;

  if( trap_Argc() > 1 )
  {
    trap_Argv( 1, arg, sizeof( arg ) );
    damage = atoi( arg );
  }
  if( trap_Argc() > 4 )
  {
    trap_Argv( 2, arg, sizeof( arg ) );
    dx = atof( arg );
    trap_Argv( 3, arg, sizeof( arg ) );
    dy = atof( arg );
    trap_Argv( 4, arg, sizeof( arg ) );
    dz = atof( arg );
    nonloc = qfalse;
  }
  VectorCopy( ent->s.origin, point );
  point[ 0 ] += dx;
  point[ 1 ] += dy;
  point[ 2 ] += dz;
  G_Damage( ent, NULL, NULL, NULL, point, damage,
            ( nonloc ? DAMAGE_NO_LOCDAMAGE : 0 ), MOD_TARGET_LASER );
}

/*
==================
G_FloodLimited

Determine whether a user is flood limited, and adjust their flood demerits
Print them a warning message if they are over the limit
Return is time in msec until the user can speak again
==================
*/
int G_FloodLimited( gentity_t *ent )
{
  int deltatime, ms;

  if( g_floodMinTime.integer <= 0 )
    return 0;

  // handles !ent
  if( G_admin_permission( ent, ADMF_NOCENSORFLOOD ) )
    return 0;

  deltatime = level.time - ent->client->pers.floodTime;

  ent->client->pers.floodDemerits += g_floodMinTime.integer - deltatime;
  if( ent->client->pers.floodDemerits < 0 )
    ent->client->pers.floodDemerits = 0;
  ent->client->pers.floodTime = level.time;

  ms = ent->client->pers.floodDemerits - g_floodMaxDemerits.integer;
  if( ms <= 0 )
    return 0;
  trap_SendServerCommand( ent - g_entities, va( "print \"You are flooding: "
                          "please wait %d second%s before trying again\n",
                          ( ms + 999 ) / 1000, ( ms > 1000 ) ? "s" : "" ) );
  return ms;
}

commands_t cmds[ ] = {
  { "a", CMD_MESSAGE|CMD_INTERMISSION, Cmd_AdminMessage_f },
  { "build", CMD_TEAM|CMD_LIVING, Cmd_Build_f },
  { "buy", CMD_HUMAN|CMD_LIVING, Cmd_Buy_f },
  { "callteamvote", CMD_MESSAGE|CMD_TEAM, Cmd_CallVote_f },
  { "callvote", CMD_MESSAGE, Cmd_CallVote_f },
  { "class", CMD_TEAM, Cmd_Class_f },
  { "damage", CMD_CHEAT|CMD_LIVING, Cmd_Damage_f },
  { "deconstruct", CMD_TEAM|CMD_LIVING, Cmd_Destroy_f },
  { "destroy", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Destroy_f },
  { "follow", CMD_SPEC, Cmd_Follow_f },
  { "follownext", CMD_SPEC, Cmd_FollowCycle_f },
  { "followprev", CMD_SPEC, Cmd_FollowCycle_f },
  { "give", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Give_f },
  { "god", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_God_f },
  { "ignore", 0, Cmd_Ignore_f },
  { "itemact", CMD_HUMAN|CMD_LIVING, Cmd_ActivateItem_f },
  { "itemdeact", CMD_HUMAN|CMD_LIVING, Cmd_DeActivateItem_f },
  { "itemtoggle", CMD_HUMAN|CMD_LIVING, Cmd_ToggleItem_f },
  { "kill", CMD_TEAM|CMD_LIVING, Cmd_Kill_f },
  { "levelshot", CMD_CHEAT, Cmd_LevelShot_f },
  { "m", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
  { "me", CMD_MESSAGE|CMD_INTERMISSION, Cmd_ActionMessage_f },
  { "mt", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
  { "noclip", CMD_CHEAT_TEAM, Cmd_Noclip_f },
  { "notarget", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Notarget_f },
  { "ptrcrestore", CMD_SPEC, Cmd_PTRCRestore_f },
  { "ptrcverify", CMD_SPEC, Cmd_PTRCVerify_f },
  { "reload", CMD_HUMAN|CMD_LIVING, Cmd_Reload_f },
  { "say", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
  { "say_area", CMD_MESSAGE|CMD_TEAM|CMD_LIVING, Cmd_SayArea_f },
  { "say_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
  { "score", CMD_INTERMISSION, ScoreboardMessage },
  { "sell", CMD_HUMAN|CMD_LIVING, Cmd_Sell_f },
  { "setviewpos", CMD_CHEAT_TEAM, Cmd_SetViewpos_f },
  { "team", 0, Cmd_Team_f },
  { "teamvote", CMD_TEAM, Cmd_Vote_f },
  { "test", CMD_CHEAT, Cmd_Test_f },
  { "unignore", 0, Cmd_Ignore_f },
  { "vote", 0, Cmd_Vote_f },
  { "vsay", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "vsay_local", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "vsay_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "where", 0, Cmd_Where_f }
};
static size_t numCmds = sizeof( cmds ) / sizeof( cmds[ 0 ] );

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum )
{
  gentity_t  *ent;
  char       cmd[ MAX_TOKEN_CHARS ];
  commands_t *command;

  ent = g_entities + clientNum;
  if( !ent->client )
    return;   // not fully in game yet

  trap_Argv( 0, cmd, sizeof( cmd ) );

  command = bsearch( cmd, cmds, numCmds, sizeof( cmds[ 0 ] ), cmdcmp );

  if( !command )
  {
    if( !G_admin_cmd_check( ent, qfalse ) )
      trap_SendServerCommand( clientNum,
        va( "print \"Unknown command %s\n\"", cmd ) );
    return;
  }

  // do tests here to reduce the amount of repeated code

  if( !( command->cmdFlags & CMD_INTERMISSION ) && level.intermissiontime )
    return;

  if( command->cmdFlags & CMD_CHEAT && !g_cheats.integer )
  {
    G_TriggerMenu( clientNum, MN_CMD_CHEAT );
    return;
  }

  if( command->cmdFlags & CMD_MESSAGE && ( ent->client->pers.muted ||
      G_FloodLimited( ent ) ) )
    return;

  if( ( command->cmdFlags & CMD_TEAM ||
      ( command->cmdFlags & CMD_CHEAT_TEAM && !g_cheats.integer ) ) &&
      ent->client->pers.teamSelection == TEAM_NONE )
  {
    G_TriggerMenu( clientNum, MN_CMD_TEAM );
    return;
  }

  if( command->cmdFlags & CMD_CHEAT_TEAM && !g_cheats.integer &&
      ent->client->pers.teamSelection != TEAM_NONE )
  {
    G_TriggerMenu( clientNum, MN_CMD_CHEAT_TEAM );
    return;
  }

  if( command->cmdFlags & CMD_SPEC &&
      ent->client->sess.spectatorState == SPECTATOR_NOT )
  {
    G_TriggerMenu( clientNum, MN_CMD_SPEC );
    return;
  }

  if( command->cmdFlags & CMD_ALIEN &&
      ent->client->pers.teamSelection != TEAM_ALIENS )
  {
    G_TriggerMenu( clientNum, MN_CMD_ALIEN );
    return;
  }

  if( command->cmdFlags & CMD_HUMAN &&
      ent->client->pers.teamSelection != TEAM_HUMANS )
  {
    G_TriggerMenu( clientNum, MN_CMD_HUMAN );
    return;
  }

  if( command->cmdFlags & CMD_LIVING &&
    ( ent->client->ps.stats[ STAT_HEALTH ] <= 0 ||
      ent->client->sess.spectatorState != SPECTATOR_NOT ) )
  {
    G_TriggerMenu( clientNum, MN_CMD_LIVING );
    return;
  }

  command->cmdHandler( ent );
}

/*
=================
G_SayArgc
G_SayArgv
G_SayConcatArgs

trap_Argc, trap_Argv, and ConcatArgs consider say text as a single argument
These functions assemble the text and re-parse it on word boundaries
=================
*/
int G_SayArgc( void )
{
  int c = 0;
  char *s;

  s = ConcatArgs( 0 );
  while( 1 )
  {
    while( *s == ' ' )
      s++;
    if( !*s )
      break;
    c++;
    while( *s && *s != ' ' )
      s++;
  }
  return c;
}

qboolean G_SayArgv( int n, char *buffer, int bufferLength )
{
  char *s;

  if( bufferLength < 1 )
    return qfalse;
  if( n < 0 )
    return qfalse;
  s = G_SayConcatArgs( n );
  if( !*s )
    return qfalse;
  while( *s && *s != ' ' && bufferLength > 1 )
  {
    *buffer++ = *s++;
    bufferLength--;
  }
  *buffer = 0;
  return qtrue;
}

char *G_SayConcatArgs( int start )
{
  char *s;

  s = ConcatArgs( 0 );
  while( 1 )
  {
    while( *s == ' ' )
      s++;
    if( !*s || start == 0 )
      break;
    start--;
    while( *s && *s != ' ' )
      s++;
  }
  return s;
}

void G_DecolorString( char *in, char *out, int len )
{
  len--;

  while( *in && len > 0 ) {
    if( Q_IsColorString( in ) ) {
      in += 2;
      continue;
    }
    *out++ = *in++;
    len--;
  }
  *out = '\0';
}

void Cmd_PrivateMessage_f( gentity_t *ent )
{
  int pids[ MAX_CLIENTS ];
  char name[ MAX_NAME_LENGTH ];
  char cmd[ 12 ];
  char str[ MAX_STRING_CHARS ];
  char *msg;
  char color;
  int i, pcount;
  int skipargs = 0;
  int count = 0;
  qboolean teamonly = qfalse;

  if( !g_privateMessages.integer && ent )
  {
    ADMP( "Sorry, but private messages have been disabled\n" );
    return;
  }

  // parse arguments
  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skipargs = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 3+skipargs )
  {
    ADMP( va( "usage: %s [name|slot#] [message]\n", cmd ) );
    return;
  }

  if( !Q_stricmp( cmd, "mt" ) || !Q_stricmp( cmd, "/mt" ) )
    teamonly = qtrue;

  // figure out the name matches
  G_SayArgv( 1+skipargs, name, sizeof( name ) );
  msg = G_SayConcatArgs( 2+skipargs );
  pcount = G_ClientNumbersFromString( name, pids, MAX_CLIENTS );

  // send the message
  for( i = 0; i < pcount; i++ )
    if( G_SayTo( ent, &g_entities[ pids[ i ] ], 
        teamonly ? SAY_TPRIVMSG : SAY_PRIVMSG, msg ) )
      count++;

  // report the results
  color = teamonly ? COLOR_CYAN : COLOR_YELLOW;

  Com_sprintf( str, sizeof( str ), "^%csent to %i player%s", color, count,
    ( count == 1 ) ? "" : "s" );

  if( !count )
    ADMP( va( "^3No player matching ^7\'%s^7\' ^3to send message to.\n",
      name ) );
  else
  {
    ADMP( va( "^%cPrivate message: ^7%s\n", color, msg ) );
    ADMP( va( "%s\n", str ) );

    G_LogPrintf( "%s: %d \"%s" S_COLOR_WHITE "\" \"%s\": ^%c%s\n",
      ( teamonly ) ? "TPrivMsg" : "PrivMsg",
      ( ent ) ? ent - g_entities : -1,
      ( ent ) ? ent->client->pers.netname : "console",
      name, color, msg );
  }
}

/*
=================
Cmd_AdminMessage_f

Send a message to all active admins
=================
*/
void Cmd_AdminMessage_f( gentity_t *ent )
{
  char cmd[ sizeof( "say_team" ) ];
  int skiparg = 0;

  // Check permissions and add the appropriate user [prefix]
  if( !G_admin_permission( ent, ADMF_ADMINCHAT ) )
  {
    if( !g_publicAdminMessages.integer )
    {
      ADMP( "Sorry, but use of /a by non-admins has been disabled.\n" );
      return;
    }
    else
    {
      ADMP( "Your message has been sent to any available admins "
            "and to the server logs.\n" );
    }
  }

  // Skip say/say_team if this was used from one of those
  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skiparg = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( va( "usage: %s [message]\n", cmd ) );
    return;
  }

  G_AdminMessage( ent, G_SayConcatArgs( 1 + skiparg ) );
}

