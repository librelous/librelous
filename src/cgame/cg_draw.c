// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

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

// used for scoreboard
extern    displayContextDef_t cgDC;
menuDef_t *menuScoreboard = NULL;

int drawTeamOverlayModificationCount = -1;

int sortedTeamPlayers[TEAM_MAXOVERLAY];
int numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

//TA UI
int CG_Text_Width( const char *text, float scale, int limit )
{
  int         count,len;
  float       out;
  glyphInfo_t *glyph;
  float       useScale;
// FIXME: see ui_main.c, same problem
//  const unsigned char *s = text;
  const char  *s = text;
  fontInfo_t  *font = &cgDC.Assets.textFont;
  
  if( scale <= cg_smallFont.value )
    font = &cgDC.Assets.smallFont;
  else if( scale > cg_bigFont.value )
    font = &cgDC.Assets.bigFont;
  
  useScale = scale * font->glyphScale;
  out = 0;
  
  if( text )
  {
    len = strlen( text );
    if( limit > 0 && len > limit )
      len = limit;
    
    count = 0;
    while( s && *s && count < len )
    {
      if( Q_IsColorString( s ) )
      {
        s += 2;
        continue;
      }
      else
      {
        glyph = &font->glyphs[ (int)*s ];
        //TTimo: FIXME: getting nasty warnings without the cast,
        //hopefully this doesn't break the VM build
        out += glyph->xSkip;
        s++;
        count++;
      }
    }
  }
  
  return out * useScale;
}

int CG_Text_Height( const char *text, float scale, int limit )
{
  int         len, count;
  float       max;
  glyphInfo_t *glyph;
  float       useScale;
// TTimo: FIXME
//  const unsigned char *s = text;
  const char  *s = text;
  fontInfo_t  *font = &cgDC.Assets.textFont;
  
  if( scale <= cg_smallFont.value )
    font = &cgDC.Assets.smallFont;
  else if( scale > cg_bigFont.value )
    font = &cgDC.Assets.bigFont;
  
  useScale = scale * font->glyphScale;
  max = 0;
  
  if( text )
  {
    len = strlen( text );
    if( limit > 0 && len > limit )
      len = limit;
    
    count = 0;
    while( s && *s && count < len )
    {
      if( Q_IsColorString( s ) )
      {
        s += 2;
        continue;
      }
      else
      {
        glyph = &font->glyphs[ (int)*s ];
        //TTimo: FIXME: getting nasty warnings without the cast,
        //hopefully this doesn't break the VM build
        if( max < glyph->height )
          max = glyph->height;
        
        s++;
        count++;
      }
    }
  }
  
  return max * useScale;
}

void CG_Text_PaintChar( float x, float y, float width, float height, float scale,
                        float s, float t, float s2, float t2, qhandle_t hShader )
{
  float w, h;
  w = width * scale;
  h = height * scale;
  CG_AdjustFrom640( &x, &y, &w, &h );
  trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint( float x, float y, float scale, vec4_t color, const char *text,
                    float adjust, int limit, int style )
{
  int         len, count;
  vec4_t      newColor;
  glyphInfo_t *glyph;
  float       useScale;
  fontInfo_t  *font = &cgDC.Assets.textFont;
  
  if( scale <= cg_smallFont.value )
    font = &cgDC.Assets.smallFont;
  else if( scale > cg_bigFont.value )
    font = &cgDC.Assets.bigFont;
  
  useScale = scale * font->glyphScale;
  if( text )
  {
// TTimo: FIXME
//    const unsigned char *s = text;
    const char *s = text;
    
    trap_R_SetColor( color );
    memcpy( &newColor[ 0 ], &color[ 0 ], sizeof( vec4_t ) );
    len = strlen( text );
    
    if( limit > 0 && len > limit )
      len = limit;
    
    count = 0;
    while( s && *s && count < len )
    {
      glyph = &font->glyphs[ (int)*s ];
      //TTimo: FIXME: getting nasty warnings without the cast,
      //hopefully this doesn't break the VM build
      
      if( Q_IsColorString( s ) )
      {
        memcpy( newColor, g_color_table[ ColorIndex( *( s + 1 ) ) ], sizeof( newColor ) );
        newColor[ 3 ] = color[ 3 ];
        trap_R_SetColor( newColor );
        s += 2;
        continue;
      }
      else
      {
        float yadj = useScale * glyph->top;
        if( style == ITEM_TEXTSTYLE_SHADOWED ||
            style == ITEM_TEXTSTYLE_SHADOWEDMORE )
        {
          int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
          colorBlack[ 3 ] = newColor[ 3 ];
          trap_R_SetColor( colorBlack );
          CG_Text_PaintChar( x + ofs, y - yadj + ofs, 
                             glyph->imageWidth,
                             glyph->imageHeight,
                             useScale, 
                             glyph->s,
                             glyph->t,
                             glyph->s2,
                             glyph->t2,
                             glyph->glyph );
          
          colorBlack[ 3 ] = 1.0;
          trap_R_SetColor( newColor );
        }
        else if( style == ITEM_TEXTSTYLE_NEON )
        {
          vec4_t glow, outer, inner, white;

          glow[ 0 ] = newColor[ 0 ] * 0.5;
          glow[ 1 ] = newColor[ 1 ] * 0.5;
          glow[ 2 ] = newColor[ 2 ] * 0.5;
          glow[ 3 ] = newColor[ 3 ] * 0.2;
          
          outer[ 0 ] = newColor[ 0 ];
          outer[ 1 ] = newColor[ 1 ];
          outer[ 2 ] = newColor[ 2 ];
          outer[ 3 ] = newColor[ 3 ];
          
          inner[ 0 ] = newColor[ 0 ] * 1.5 > 1.0f ? 1.0f : newColor[ 0 ] * 1.5;
          inner[ 1 ] = newColor[ 1 ] * 1.5 > 1.0f ? 1.0f : newColor[ 1 ] * 1.5;
          inner[ 2 ] = newColor[ 2 ] * 1.5 > 1.0f ? 1.0f : newColor[ 2 ] * 1.5;
          inner[ 3 ] = newColor[ 3 ];

          white[ 0 ] = white[ 1 ] = white[ 2 ] = white[ 3 ] = 1.0f;
          
          trap_R_SetColor( glow );
          CG_Text_PaintChar(  x - 3, y - yadj - 3,
                              glyph->imageWidth + 6,
                              glyph->imageHeight + 6,
                              useScale,
                              glyph->s,
                              glyph->t,
                              glyph->s2,
                              glyph->t2,
                              glyph->glyph );
          
          trap_R_SetColor( outer );
          CG_Text_PaintChar(  x - 1, y - yadj - 1,
                              glyph->imageWidth + 2,
                              glyph->imageHeight + 2,
                              useScale,
                              glyph->s,
                              glyph->t,
                              glyph->s2,
                              glyph->t2,
                              glyph->glyph );
          
          trap_R_SetColor( inner );
          CG_Text_PaintChar(  x - 0.5, y - yadj - 0.5,
                              glyph->imageWidth + 1,
                              glyph->imageHeight + 1,
                              useScale,
                              glyph->s,
                              glyph->t,
                              glyph->s2,
                              glyph->t2,
                              glyph->glyph );
          
          trap_R_SetColor( white );
        }
        
        
        CG_Text_PaintChar( x, y - yadj, 
                           glyph->imageWidth,
                           glyph->imageHeight,
                           useScale, 
                           glyph->s,
                           glyph->t,
                           glyph->s2,
                           glyph->t2,
                           glyph->glyph );
        
        x += ( glyph->xSkip * useScale ) + adjust;
        s++;
        count++;
      }
    }
    
    trap_R_SetColor( NULL );
  }
}

/*
==============
CG_DrawField

Draws large numbers for status bar and powerups
==============
*/
static void CG_DrawField( int x, int y, int width, int cw, int ch, int value )
{
  char  num[ 16 ], *ptr;
  int   l;
  int   frame;
  int   charWidth, charHeight;

  if( !( charWidth = cw ) )
    charWidth = CHAR_WIDTH;

  if( !( charHeight = ch ) )
    charWidth = CHAR_HEIGHT;

  if( width < 1 )
    return;

  // draw number string
  if( width > 4 )
    width = 4;

  switch( width )
  {
    case 1:
      value = value > 9 ? 9 : value;
      value = value < 0 ? 0 : value;
      break;
    case 2:
      value = value > 99 ? 99 : value;
      value = value < -9 ? -9 : value;
      break;
    case 3:
      value = value > 999 ? 999 : value;
      value = value < -99 ? -99 : value;
      break;
    case 4:
      value = value > 9999 ? 9999 : value;
      value = value < -999 ? -999 : value;
      break;
  }

  Com_sprintf( num, sizeof( num ), "%d", value );
  l = strlen( num );
  
  if( l > width )
    l = width;
  
  x += 2 + charWidth * ( width - l );

  ptr = num;
  while( *ptr && l )
  {
    if( *ptr == '-' )
      frame = STAT_MINUS;
    else
      frame = *ptr -'0';

    CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[ frame ] );
    x += charWidth;
    ptr++;
    l--;
  }
}

//=============== TA: was cg_newdraw.c

void CG_InitTeamChat( )
{
  memset( teamChat1,  0, sizeof( teamChat1 ) );
  memset( teamChat2,  0, sizeof( teamChat2 ) );
  memset( systemChat, 0, sizeof( systemChat ) );
}

void CG_SetPrintString( int type, const char *p )
{
  if( type == SYSTEM_PRINT )
  {
    strcpy( systemChat, p );
  }
  else
  {
    strcpy( teamChat2, teamChat1 );
    strcpy( teamChat1, p );
  }
}

static void CG_DrawPlayerCreditsValue( rectDef_t *rect, vec4_t color )
{
  int           value;
  playerState_t *ps;

  ps = &cg.snap->ps;

  value = ps->persistant[ PERS_CREDIT ];
  if( value > -1 )
  {
    trap_R_SetColor( color );
    CG_DrawField( rect->x, rect->y, 4, rect->w / 4, rect->h, value );
    trap_R_SetColor( NULL );
  }
}

static void CG_DrawPlayerBankValue( rectDef_t *rect, vec4_t color )
{
  int           value;
  playerState_t *ps;

  ps = &cg.snap->ps;

  value = ps->persistant[ PERS_BANK ];
  if( value > -1 )
  {
    trap_R_SetColor( color );
    CG_DrawField( rect->x, rect->y, 4, rect->w / 4, rect->h, value );
    trap_R_SetColor( NULL );
  }
}

static void CG_DrawPlayerStamina( rectDef_t *rect )
{
  playerState_t *ps = &cg.snap->ps;
  int stamina = ps->stats[ STAT_STAMINA ];
  int height = (int)( (float)stamina / ( MAX_STAMINA / ( rect->h / 2 ) ) );
  vec4_t bcolor = { 0.5f, 0.5f, 0.5f, 0.5f };
  vec4_t pos =    { 0.0f, 0.5f, 0.0f, 0.5f };
  vec4_t neg =    { 0.5f, 0.0f, 0.0f, 0.5f };

  trap_R_SetColor( bcolor );   // white
  CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.whiteShader );

  if( stamina > 0 )
  {
    trap_R_SetColor( pos ); // green
    CG_DrawPic( rect->x, rect->y + ( rect->h / 2 - height ),
                rect->w, height, cgs.media.whiteShader );
  }
  
  if( stamina < 0 )
  {
    trap_R_SetColor( neg ); // red
    CG_DrawPic( rect->x, rect->y + rect->h / 2, rect->w, -height, cgs.media.whiteShader );
  }

  trap_R_SetColor( NULL );
}

static void CG_DrawPlayerAmmoValue( rectDef_t *rect, vec4_t color )
{
  int           value;
  centity_t     *cent;
  playerState_t *ps;

  cent = &cg_entities[ cg.snap->ps.clientNum ];
  ps = &cg.snap->ps;

  if( cent->currentState.weapon )
  {
    switch( cent->currentState.weapon )
    {
      case WP_ABUILD:
      case WP_ABUILD2:
      case WP_HBUILD:
      case WP_HBUILD2:
        //percentage of power remaining
        value = (int)( ( (float)cgs.humanBuildPoints / (float)cgs.humanBuildPointsTotal ) * 100.0f );
        break;

      default:
        BG_unpackAmmoArray( cent->currentState.weapon, ps->ammo, ps->powerups, &value, NULL, NULL );
        break;
    }
    
    if( value > -1 )
    {
      trap_R_SetColor( color );
      CG_DrawField( rect->x, rect->y, 4, rect->w / 4, rect->h, value );
      trap_R_SetColor( NULL );
    }
  }
}

#define BUILD_DELAY_TIME  2000

static void CG_DrawPlayerClipsValue( rectDef_t *rect, vec4_t color )
{
  int           value;
  centity_t     *cent;
  playerState_t *ps;

  cent = &cg_entities[ cg.snap->ps.clientNum ];
  ps = &cg.snap->ps;

  if( cent->currentState.weapon )
  {
    switch( cent->currentState.weapon )
    {
      case WP_ABUILD:
      case WP_ABUILD2:
      case WP_HBUILD:
      case WP_HBUILD2:
        //remaining build delay
        value = (int)( ceil( (float)ps->stats[ STAT_MISC ] / 1000.0f ) );

        if( cg.time - cg.lastBuildAttempt <= BUILD_DELAY_TIME )
        {
          if( ( cg.time / 300 ) % 2 )
          {
            color[ 0 ] = 1.0f;
            color[ 1 ] = color[ 2 ] = 0.0f;
            color[ 3 ] = 1.0f;
          }
        }
        break;

      default:
        BG_unpackAmmoArray( cent->currentState.weapon, ps->ammo, ps->powerups, NULL, &value, NULL );
        break;
    }
    
    if( value > -1 )
    {
      trap_R_SetColor( color );
      CG_DrawField( rect->x, rect->y, 4, rect->w / 4, rect->h, value );
      trap_R_SetColor( NULL );
    }
  }
}

static void CG_DrawPlayerHealth( rectDef_t *rect, vec4_t color )
{
  playerState_t *ps;
  int value;

  ps = &cg.snap->ps;

  value = ps->stats[ STAT_HEALTH ];

  trap_R_SetColor( color );
  CG_DrawField( rect->x, rect->y, 4, rect->w / 4, rect->h, value );
  trap_R_SetColor( NULL );
}

static void CG_DrawProgressBar( rectDef_t *rect, vec4_t color,
                                float scale, int align, int textStyle, float progress )
{
  int     rimWidth = (int)( rect->h / 20.0f );
  float   doneWidth = ( rect->w - 2 * rimWidth ) * progress;
  float   leftWidth = ( rect->w - 2 * rimWidth ) - doneWidth;
  float   tx, ty, tw, th;
  char    textBuffer[ 8 ];
  
  if( progress < 0.0f )
    progress = 0.0f;
  else if( progress > 1.0f )
    progress = 1.0f;
  
  trap_R_SetColor( color );
  
  //draw rim and bar
  CG_DrawPic( rect->x, rect->y, rimWidth + doneWidth, rect->h, cgs.media.whiteShader );
  CG_DrawPic( rimWidth + rect->x + doneWidth, rect->y,
    leftWidth, rimWidth, cgs.media.whiteShader );
  CG_DrawPic( rimWidth + rect->x + doneWidth, rect->y + rect->h - rimWidth,
    leftWidth, rimWidth, cgs.media.whiteShader );
  CG_DrawPic( rect->x + rect->w - rimWidth, rect->y, rimWidth, rect->h, cgs.media.whiteShader );

  trap_R_SetColor( NULL );
  
  //draw text
  Com_sprintf( textBuffer, sizeof( textBuffer ), "%d%%", (int)( progress * 100 ) );
  tw = CG_Text_Width( textBuffer, scale, 0 );
  th = scale * 40.0f;
  
  switch( align )
  {
    case ITEM_ALIGN_LEFT:
      tx = rect->x + ( rect->w / 10.0f );
      ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
      break;

    case ITEM_ALIGN_RIGHT:
      tx = rect->x + rect->w - ( rect->w / 10.0f ) - tw;
      ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
      break;

    case ITEM_ALIGN_CENTER:
      tx = rect->x + ( rect->w / 2.0f ) - ( tw / 2.0f );
      ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
      break;

    default:
      tx = ty = 0.0f;
  }
  
  CG_Text_Paint( tx, ty, scale, color, textBuffer, 0, 0, textStyle );
}

static void CG_DrawProgressLabel( rectDef_t *rect, float text_x, float text_y, vec4_t color,
                                  float scale, int align, const char *s, float fraction )
{
  vec4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
  float tx, tw = CG_Text_Width( s, scale, 0 );

  switch( align )
  {
    case ITEM_ALIGN_LEFT:
      tx = 0.0f;
      break;

    case ITEM_ALIGN_RIGHT:
      tx = rect->w - tw;
      break;

    case ITEM_ALIGN_CENTER:
      tx = ( rect->w / 2.0f ) - ( tw / 2.0f );
      break;

    default:
      tx = 0.0f;
  }
  
  if( fraction < 1.0f )
    CG_Text_Paint( rect->x + text_x + tx, rect->y + text_y, scale, white,
      s, 0, 0, ITEM_TEXTSTYLE_NORMAL );
  else
    CG_Text_Paint( rect->x + text_x + tx, rect->y + text_y, scale, color,
      s, 0, 0, ITEM_TEXTSTYLE_NEON );
}

static void CG_DrawMediaProgress( rectDef_t *rect, vec4_t color, float scale, int align, int textStyle )
{
  CG_DrawProgressBar( rect, color, scale, align, textStyle, cg.mediaFraction );
}

static void CG_DrawMediaProgressLabel( rectDef_t *rect, float text_x, float text_y,
                                       vec4_t color, float scale, int align )
{
  CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Map and Textures", cg.mediaFraction );
}

static void CG_DrawBuildablesProgress( rectDef_t *rect, vec4_t color, float scale, int align, int textStyle )
{
  CG_DrawProgressBar( rect, color, scale, align, textStyle, cg.buildablesFraction );
}

static void CG_DrawBuildablesProgressLabel( rectDef_t *rect, float text_x, float text_y,
                                            vec4_t color, float scale, int align )
{
  CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Buildable Models", cg.buildablesFraction );
}

static void CG_DrawCharModelProgress( rectDef_t *rect, vec4_t color, float scale, int align, int textStyle )
{
  CG_DrawProgressBar( rect, color, scale, align, textStyle, cg.charModelFraction );
}

static void CG_DrawCharModelProgressLabel( rectDef_t *rect, float text_x, float text_y,
                                           vec4_t color, float scale, int align )
{
  CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Character Models", cg.charModelFraction );
}

static void CG_DrawOverallProgress( rectDef_t *rect, vec4_t color, float scale, int align, int textStyle )
{
  float total;

  total = ( cg.charModelFraction + cg.buildablesFraction + cg.mediaFraction ) / 3.0f;
  CG_DrawProgressBar( rect, color, scale, align, textStyle, total );
}

static void CG_DrawLevelShot( rectDef_t *rect )
{
  const char  *s;
  const char  *info;
  qhandle_t   levelshot;
  qhandle_t   detail;
  
  info = CG_ConfigString( CS_SERVERINFO );
  s = Info_ValueForKey( info, "mapname" );
  levelshot = trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", s ) );
  
  if( !levelshot )
    levelshot = trap_R_RegisterShaderNoMip( "gfx/2d/load_screen" );
  
  trap_R_SetColor( NULL );
  CG_DrawPic( rect->x, rect->y, rect->w, rect->h, levelshot );
  
  // blend a detail texture over it
  detail = trap_R_RegisterShader( "levelShotDetail" );
  CG_DrawPic( rect->x, rect->y, rect->w, rect->h, detail );
}

static void CG_DrawLoadingString( rectDef_t *rect, float text_x, float text_y, vec4_t color,
                                  float scale, int align, int textStyle, const char *s )
{
  float tw, th, tx;
  int   pos, i;
  char  buffer[ 1024 ];
  char  *end;
  
  if( !s[ 0 ] )
    return;
  
  strcpy( buffer, s );
  tw = CG_Text_Width( s, scale, 0 );
  th = scale * 40.0f;

  pos = i = 0;
 
  while( pos < strlen( s ) )
  {
    strcpy( buffer, &s[ pos ] );
    tw = CG_Text_Width( buffer, scale, 0 );

    while( tw > rect->w )
    {
      end = strrchr( buffer, ' ' );
      
      if( end == NULL )
        break;
      
      *end = '\0';
      tw = CG_Text_Width( buffer, scale, 0 );
    }

    switch( align )
    {
      case ITEM_ALIGN_LEFT:
        tx = rect->x;
        break;

      case ITEM_ALIGN_RIGHT:
        tx = rect->x + rect->w - tw;
        break;

      case ITEM_ALIGN_CENTER:
        tx = rect->x + ( rect->w / 2.0f ) - ( tw / 2.0f );
        break;

      default:
        tx = 0.0f;
    }
    
    CG_Text_Paint( tx + text_x, rect->y + text_y + i * ( th + 3 ), scale, color,
      buffer, 0, 0, textStyle );
    
    pos += strlen( buffer ) + 1;
    i++;
  }
}

static void CG_DrawLevelName( rectDef_t *rect, float text_x, float text_y,
                              vec4_t color, float scale, int align, int textStyle )
{
  const char  *s;
  
  s = CG_ConfigString( CS_MESSAGE );

  CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, s );
}

static void CG_DrawMOTD( rectDef_t *rect, float text_x, float text_y,
                         vec4_t color, float scale, int align, int textStyle )
{
  const char  *s;
  
  s = CG_ConfigString( CS_MOTD );

  CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, s );
}

static void CG_DrawHostname( rectDef_t *rect, float text_x, float text_y,
                             vec4_t color, float scale, int align, int textStyle )
{
  char buffer[ 1024 ];
  const char  *info;

  info = CG_ConfigString( CS_SERVERINFO );
  
  Q_strncpyz( buffer, Info_ValueForKey( info, "sv_hostname" ), 1024 );
  Q_CleanStr( buffer );

  CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, buffer );
}

/*
======================
CG_UpdateMediaFraction

======================
*/
void CG_UpdateMediaFraction( float newFract )
{
  cg.mediaFraction = newFract;

  trap_UpdateScreen( );
}

/*
====================
CG_DrawLoadingScreen

Draw all the status / pacifier stuff during level loading
====================
*/
void CG_DrawLoadingScreen( void )
{
  Menu_Paint( Menus_FindByName( "Loading" ), qtrue );
}

float CG_GetValue( int ownerDraw )
{
  centity_t *cent;
  clientInfo_t *ci;
  playerState_t *ps;

  cent = &cg_entities[ cg.snap->ps.clientNum ];
  ps = &cg.snap->ps;

  switch( ownerDraw )
  {
    case CG_PLAYER_AMMO_VALUE:
      if( cent->currentState.weapon )
      {
        int value;

        BG_unpackAmmoArray( cent->currentState.weapon, ps->ammo, ps->powerups,
           &value, NULL, NULL );
        
        return value;
      }
      break;
    case CG_PLAYER_CLIPS_VALUE:
      if( cent->currentState.weapon )
      {
        int value;

        BG_unpackAmmoArray( cent->currentState.weapon, ps->ammo, ps->powerups,
           NULL, &value, NULL );
        
        return value;
      }
      break;
    case CG_PLAYER_HEALTH:
      return ps->stats[ STAT_HEALTH ];
      break;
    default:
      break;
  }
  
  return -1;
}

static void CG_DrawAreaSystemChat( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader )
{
  CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, systemChat, 0, 0, 0 );
}

static void CG_DrawAreaTeamChat( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader )
{
  CG_Text_Paint( rect->x, rect->y + rect->h, scale, color,teamChat1, 0, 0, 0 );
}

static void CG_DrawAreaChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, teamChat2, 0, 0, 0);
}

const char *CG_GetKillerText( )
{
  const char *s = "";
  if( cg.killerName[ 0 ] )
    s = va( "Fragged by %s", cg.killerName );

  return s;
}


static void CG_DrawKiller( rectDef_t *rect, float scale, vec4_t color,
                           qhandle_t shader, int textStyle )
{
  // fragged by ... line
 if( cg.killerName[ 0 ] )
 {
   int x = rect->x + rect->w / 2;
   CG_Text_Paint( x - CG_Text_Width( CG_GetKillerText( ), scale, 0 ) / 2,
     rect->y + rect->h, scale, color, CG_GetKillerText( ), 0, 0, textStyle );
  }
}


static void CG_Text_Paint_Limit( float *maxX, float x, float y, float scale,
                                 vec4_t color, const char* text, float adjust, int limit )
{
  int len, count;
  vec4_t newColor;
  glyphInfo_t *glyph;

  if( text )
  {
// TTimo: FIXME
//    const unsigned char *s = text; // bk001206 - unsigned
    const char *s = text;
    float max = *maxX;
    float useScale;
    fontInfo_t *font = &cgDC.Assets.textFont;
    
    if( scale <= cg_smallFont.value )
      font = &cgDC.Assets.smallFont;
    else if( scale > cg_bigFont.value )
      font = &cgDC.Assets.bigFont;
    
    useScale = scale * font->glyphScale;
    trap_R_SetColor( color );
    len = strlen( text );          
    
    if( limit > 0 && len > limit )
      len = limit;
    
    count = 0;

    while( s && *s && count < len )
    {
      glyph = &font->glyphs[ (int)*s ];
      //TTimo: FIXME: getting nasty warnings without the cast,
      //hopefully this doesn't break the VM build
      
      if( Q_IsColorString( s ) )
      {
        memcpy( newColor, g_color_table[ ColorIndex( *(s+1) ) ], sizeof( newColor ) );
        newColor[ 3 ] = color[ 3 ];
        trap_R_SetColor( newColor );
        s += 2;
        continue;
      }
      else
      {
        float yadj = useScale * glyph->top;
        
        if( CG_Text_Width( s, useScale, 1 ) + x > max )
        {
          *maxX = 0;
          break;
        }
        
        CG_Text_PaintChar( x, y - yadj, 
                           glyph->imageWidth,
                           glyph->imageHeight,
                           useScale, 
                           glyph->s,
                           glyph->t,
                           glyph->s2,
                           glyph->t2,
                           glyph->glyph );
        x += ( glyph->xSkip * useScale ) + adjust;
        *maxX = x;
        count++;
        s++;
      }
    }
    
    trap_R_SetColor( NULL );
  }
}

static void CG_DrawTeamSpectators( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader )
{
  if( cg.spectatorLen )
  {
    float maxX;

    if( cg.spectatorWidth == -1 )
    {
      cg.spectatorWidth = 0;
      cg.spectatorPaintX = rect->x + 1;
      cg.spectatorPaintX2 = -1;
    }

    if( cg.spectatorOffset > cg.spectatorLen )
    {
      cg.spectatorOffset = 0;
      cg.spectatorPaintX = rect->x + 1;
      cg.spectatorPaintX2 = -1;
    }

    if( cg.time > cg.spectatorTime )
    {
      cg.spectatorTime = cg.time + 10;
      
      if( cg.spectatorPaintX <= rect->x + 2 )
      {
        if( cg.spectatorOffset < cg.spectatorLen )
        {
          cg.spectatorPaintX += CG_Text_Width( &cg.spectatorList[ cg.spectatorOffset ], scale, 1 ) - 1;
          cg.spectatorOffset++;
        }
        else
        {
          cg.spectatorOffset = 0;
          
          if( cg.spectatorPaintX2 >= 0 )
            cg.spectatorPaintX = cg.spectatorPaintX2;
          else
            cg.spectatorPaintX = rect->x + rect->w - 2;
          
          cg.spectatorPaintX2 = -1;
        }
      }
      else
      {
        cg.spectatorPaintX--;
        
        if( cg.spectatorPaintX2 >= 0 )
          cg.spectatorPaintX2--;
      }
    }

    maxX = rect->x + rect->w - 2;
    
    CG_Text_Paint_Limit( &maxX, cg.spectatorPaintX, rect->y + rect->h - 3, scale, color,
                         &cg.spectatorList[ cg.spectatorOffset ], 0, 0 ); 
    
    if( cg.spectatorPaintX2 >= 0 )
    {
      float maxX2 = rect->x + rect->w - 2;
      CG_Text_Paint_Limit( &maxX2, cg.spectatorPaintX2, rect->y + rect->h - 3, scale,
                           color, cg.spectatorList, 0, cg.spectatorOffset ); 
    }
    
    if( cg.spectatorOffset && maxX > 0 )
    {
      // if we have an offset ( we are skipping the first part of the string ) and we fit the string
      if( cg.spectatorPaintX2 == -1 )
        cg.spectatorPaintX2 = rect->x + rect->w - 2;
    }
    else
      cg.spectatorPaintX2 = -1;
  }
}
void CG_OwnerDraw( float x, float y, float w, float h, float text_x,
                   float text_y, int ownerDraw, int ownerDrawFlags,
                   int align, float special, float scale, vec4_t color,
                   qhandle_t shader, int textStyle )
{
  rectDef_t rect;

  if( cg_drawStatus.integer == 0 )
    return;

  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  switch( ownerDraw )
  {
    case CG_PLAYER_CREDITS_VALUE:
      CG_DrawPlayerCreditsValue( &rect, color );
      break;
    case CG_PLAYER_BANK_VALUE:
      CG_DrawPlayerBankValue( &rect, color );
      break;
    case CG_PLAYER_STAMINA:
      CG_DrawPlayerStamina( &rect );
      break;
    case CG_PLAYER_AMMO_VALUE:
      CG_DrawPlayerAmmoValue( &rect, color );
      break;
    case CG_PLAYER_CLIPS_VALUE:
      CG_DrawPlayerClipsValue( &rect, color );
      break;
    case CG_PLAYER_HEALTH:
      CG_DrawPlayerHealth( &rect, color );
      break;
    case CG_AREA_SYSTEMCHAT:
      CG_DrawAreaSystemChat( &rect, scale, color, shader );
      break;
    case CG_AREA_TEAMCHAT:
      CG_DrawAreaTeamChat( &rect, scale, color, shader );
      break;
    case CG_AREA_CHAT:
      CG_DrawAreaChat( &rect, scale, color, shader );
      break;
    case CG_KILLER:
      CG_DrawKiller( &rect, scale, color, shader, textStyle );
      break;
    case CG_PLAYER_SELECT:
      CG_DrawWeaponSelect( &rect );
      break;
    case CG_PLAYER_SELECTTEXT:
      CG_DrawWeaponSelectText( &rect, scale, textStyle );
      break;
    case CG_SPECTATORS:
      CG_DrawTeamSpectators( &rect, scale, color, shader );
      break;
      
    //loading screen
    case CG_LOAD_LEVELSHOT:
      CG_DrawLevelShot( &rect );
      break;
    case CG_LOAD_MEDIA:
      CG_DrawMediaProgress( &rect, color, scale, align, textStyle );
      break;
    case CG_LOAD_MEDIA_LABEL:
      CG_DrawMediaProgressLabel( &rect, text_x, text_y, color, scale, align );
      break;
    case CG_LOAD_BUILDABLES:
      CG_DrawBuildablesProgress( &rect, color, scale, align, textStyle );
      break;
    case CG_LOAD_BUILDABLES_LABEL:
      CG_DrawBuildablesProgressLabel( &rect, text_x, text_y, color, scale, align );
      break;
    case CG_LOAD_CHARMODEL:
      CG_DrawCharModelProgress( &rect, color, scale, align, textStyle );
      break;
    case CG_LOAD_CHARMODEL_LABEL:
      CG_DrawCharModelProgressLabel( &rect, text_x, text_y, color, scale, align );
      break;
    case CG_LOAD_OVERALL:
      CG_DrawOverallProgress( &rect, color, scale, align, textStyle );
      break;
    case CG_LOAD_LEVELNAME:
      CG_DrawLevelName( &rect, text_x, text_y, color, scale, align, textStyle );
      break;
    case CG_LOAD_MOTD:
      CG_DrawMOTD( &rect, text_x, text_y, color, scale, align, textStyle );
      break;
    case CG_LOAD_HOSTNAME:
      CG_DrawHostname( &rect, text_x, text_y, color, scale, align, textStyle );
      break;
    default:
      break;
  }
}

void CG_MouseEvent( int x, int y ) 
{
  int n;

  if( ( cg.predictedPlayerState.pm_type == PM_NORMAL ||
        cg.predictedPlayerState.pm_type == PM_SPECTATOR ) &&
        cg.showScores == qfalse ) 
  {
    trap_Key_SetCatcher( 0 );
    return;
  }

  cgs.cursorX += x;
  if( cgs.cursorX < 0 )
    cgs.cursorX = 0;
  else if( cgs.cursorX > 640 )
    cgs.cursorX = 640;

  cgs.cursorY += y;
  if( cgs.cursorY < 0 )
    cgs.cursorY = 0;
  else if( cgs.cursorY > 480 )
    cgs.cursorY = 480;

  n = Display_CursorType( cgs.cursorX, cgs.cursorY );
  cgs.activeCursor = 0;
  if( n == CURSOR_ARROW )
    cgs.activeCursor = cgs.media.selectCursor;
  else if( n == CURSOR_SIZER )
    cgs.activeCursor = cgs.media.sizeCursor;

  if( cgs.capturedItem )
    Display_MouseMove( cgs.capturedItem, x, y );
  else
    Display_MouseMove( NULL, cgs.cursorX, cgs.cursorY );
}

/*
==================
CG_HideTeamMenus
==================

*/
void CG_HideTeamMenu( )
{
  Menus_CloseByName( "teamMenu" );
  Menus_CloseByName( "getMenu" );
}

/*
==================
CG_ShowTeamMenus
==================

*/
void CG_ShowTeamMenu( )
{
  Menus_OpenByName("teamMenu");
}

/*
==================
CG_EventHandling
==================
 type 0 - no event handling
      1 - team menu
      2 - hud editor

*/
void CG_EventHandling( int type )
{
  cgs.eventHandling = type;
  
  if( type == CGAME_EVENT_NONE )
  {
    CG_HideTeamMenu( );
  }
  else if( type == CGAME_EVENT_TEAMMENU )
  {
    //CG_ShowTeamMenu();
  }
  else if( type == CGAME_EVENT_SCOREBOARD )
  {
    
  }
}



void CG_KeyEvent( int key, qboolean down )
{
  if( !down)
    return;

  if( cg.predictedPlayerState.pm_type == PM_NORMAL ||
      ( cg.predictedPlayerState.pm_type == PM_SPECTATOR &&
        cg.showScores == qfalse ) )
  {
    CG_EventHandling( CGAME_EVENT_NONE );
    trap_Key_SetCatcher( 0 );
    return;
  }

  Display_HandleKey( key, down, cgs.cursorX, cgs.cursorY );

  if( cgs.capturedItem )
  {
    cgs.capturedItem = NULL;
  }
  else
  {
    if( key == K_MOUSE2 && down )
      cgs.capturedItem = Display_CaptureItem( cgs.cursorX, cgs.cursorY );
  }
}

int CG_ClientNumFromName( const char *p )
{
  int i;
  
  for( i = 0; i < cgs.maxclients; i++ )
  {
    if( cgs.clientinfo[ i ].infoValid &&
        Q_stricmp( cgs.clientinfo[ i ].name, p ) == 0 )
      return i;
  }
  
  return -1;
}

void CG_ShowResponseHead( )
{
  Menus_OpenByName( "voiceMenu" );
  trap_Cvar_Set( "cl_conXOffset", "72" );
  cg.voiceTime = cg.time;
}

void CG_RunMenuScript( char **args )
{
}


void CG_GetTeamColor(vec4_t *color)
{
  (*color)[0] = (*color)[2] = 0.0f;
  (*color)[1] = 0.17f;
  (*color)[3] = 0.25f;
}
//END TA UI


/*
================
CG_DrawLighting

================
*/
static void CG_DrawLighting( void )
{
  centity_t   *cent;
  
  cent = &cg_entities[cg.snap->ps.clientNum];

  if( cg.snap->ps.stats[ STAT_PCLASS ] == PCL_H_BASE )
  {
    if( BG_activated( UP_NVG, cg.snap->ps.stats ) )
      CG_DrawPic( 0, 0, 640, 480, cgs.media.humanNV );
  }

  //fade to black if stamina is low
  if( ( cg.snap->ps.stats[ STAT_STAMINA ] < -800  ) &&
      ( cg.snap->ps.stats[ STAT_PTEAM ] == PTE_HUMANS ) )
  {
    vec4_t black = { 0, 0, 0, 0 };
    black[ 3 ] = 1.0 - ( (float)( cg.snap->ps.stats[ STAT_STAMINA ] + 1000 ) / 200.0f );
    trap_R_SetColor( black );
    CG_DrawPic( 0, 0, 640, 480, cgs.media.whiteShader );
    trap_R_SetColor( NULL );
  }
}

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y )
{
  char    *s;
  int     w;

  s = va( "time:%d snap:%d cmd:%d", cg.snap->serverTime,
    cg.latestSnapshotNum, cgs.serverCommandSequence );
  w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

  CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

  return y + BIGCHAR_HEIGHT + 4;
}

/*
==================
CG_DrawFPS
==================
*/
//TA: personally i think this should be longer - it should really be a cvar
#define FPS_FRAMES  40
static float CG_DrawFPS( float y )
{
  char        *s;
  int         w;
  static int  previousTimes[ FPS_FRAMES ];
  static int  index;
  int         i, total;
  int         fps;
  static int  previous;
  int         t, frameTime;

  // don't use serverTime, because that will be drifting to
  // correct for internet lag changes, timescales, timedemos, etc
  t = trap_Milliseconds( );
  frameTime = t - previous;
  previous = t;

  previousTimes[ index % FPS_FRAMES ] = frameTime;
  index++;
  
  if( index > FPS_FRAMES )
  {
    // average multiple frames together to smooth changes out a bit
    total = 0;
    
    for( i = 0 ; i < FPS_FRAMES ; i++ )
      total += previousTimes[ i ];
    
    if( !total )
      total = 1;
    
    fps = 1000 * FPS_FRAMES / total;

    s = va( "%dfps", fps );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

    CG_DrawBigString( 635 - w, y + 2, s, 1.0F );
  }

  return y + BIGCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawTimer
=================
*/
static float CG_DrawTimer( float y )
{
  char    *s;
  int     w;
  int     mins, seconds, tens;
  int     msec;

  msec = cg.time - cgs.levelStartTime;

  seconds = msec / 1000;
  mins = seconds / 60;
  seconds -= mins * 60;
  tens = seconds / 10;
  seconds -= tens * 10;

  s = va( "%d:%d%d", mins, tens, seconds );
  w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

  CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

  return y + BIGCHAR_HEIGHT + 4;
}


/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void )
{
  float y;

  y = 0;

  if( cg_drawSnapshot.integer )
    y = CG_DrawSnapshot( y );
  
  if( cg_drawFPS.integer )
    y = CG_DrawFPS( y );
  
  if( cg_drawTimer.integer )
    y = CG_DrawTimer( y );
}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/


/*
=================
CG_DrawBuildPoints

Draw the small two score display
=================
*/
/*static float CG_DrawBuildPoints( float y )
{
  const char  *s;
  int     points, totalpoints, buildpoints;
  int     team;
  int     x, w;
  float   y1;
  qboolean  spectator;

  y -= BIGCHAR_HEIGHT + 8;

  y1 = y;
 

  x = 640;
  points = cg.snap->ps.persistant[PERS_POINTS];
  totalpoints = cg.snap->ps.persistant[PERS_TOTALPOINTS];

  team = cg.snap->ps.stats[ STAT_PTEAM ];
  
  if( team == PTE_ALIENS )
    buildpoints = cgs.alienBuildPoints;
  else if( team == PTE_HUMANS )
    buildpoints = cgs.humanBuildPoints;

  spectator = ( team == PTE_NONE );

  if( !spectator )
  {
    s = va( "%2i", points );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH + 8;
    x -= w;

    CG_DrawBigString( x + 2, y, s, 1.0F );

    s = va( "%2i", totalpoints );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH + 8;
    x -= w;

    CG_DrawBigString( x + 2, y, s, 1.0F );

    s = va( "%2i", buildpoints );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH + 8;
    x -= w;

    CG_DrawBigString( x + 2, y, s, 1.0F );
  }

  return y1 - 8;
}*/

//===========================================================================================


/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define LAG_SAMPLES   128

typedef struct
{
  int   frameSamples[ LAG_SAMPLES ];
  int   frameCount;
  int   snapshotFlags[ LAG_SAMPLES ];
  int   snapshotSamples[ LAG_SAMPLES ];
  int   snapshotCount;
} lagometer_t;

lagometer_t   lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void )
{
  int     offset;

  offset = cg.time - cg.latestSnapshotTime;
  lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1 ) ] = offset;
  lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap )
{
  // dropped packet
  if( !snap )
  {
    lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = -1;
    lagometer.snapshotCount++;
    return;
  }

  // add this snapshot's info
  lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->ping;
  lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->snapFlags;
  lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void )
{
  float       x, y;
  int         cmdNum;
  usercmd_t   cmd;
  const char  *s;
  int         w;

  // draw the phone jack if we are completely past our buffers
  cmdNum = trap_GetCurrentCmdNumber( ) - CMD_BACKUP + 1;
  trap_GetUserCmd( cmdNum, &cmd );
  
  // special check for map_restart
  if( cmd.serverTime <= cg.snap->ps.commandTime || cmd.serverTime > cg.time )
    return;

  // also add text in center of screen
  s = "Connection Interrupted";
  w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
  CG_DrawBigString( 320 - w / 2, 100, s, 1.0F);

  // blink the icon
  if( ( cg.time >> 9 ) & 1 )
    return;

  x = 640 - 48;
  y = 480 - 48;

  CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader( "gfx/2d/net.tga" ) );
}

#define MAX_LAGOMETER_PING  900
#define MAX_LAGOMETER_RANGE 300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void )
{
  int   a, x, y, i;
  float v;
  float ax, ay, aw, ah, mid, range;
  int   color;
  float vscale;

  if( !cg_lagometer.integer || cgs.localServer )
  {
    CG_DrawDisconnect( );
    return;
  }

  //
  // draw the graph
  //
  x = 640 - 48;
  y = 480 - 144;

  trap_R_SetColor( NULL );
  CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

  ax = x;
  ay = y;
  aw = 48;
  ah = 48;
  CG_AdjustFrom640( &ax, &ay, &aw, &ah );

  color = -1;
  range = ah / 3;
  mid = ay + range;

  vscale = range / MAX_LAGOMETER_RANGE;

  // draw the frame interpoalte / extrapolate graph
  for( a = 0 ; a < aw ; a++ )
  {
    i = ( lagometer.frameCount - 1 - a ) & ( LAG_SAMPLES - 1 );
    v = lagometer.frameSamples[ i ];
    v *= vscale;
    
    if( v > 0 )
    {
      if( color != 1 )
      {
        color = 1;
        trap_R_SetColor( g_color_table[ ColorIndex( COLOR_YELLOW ) ] );
      }
      
      if( v > range )
        v = range;
        
      trap_R_DrawStretchPic( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
    }
    else if( v < 0 )
    {
      if( color != 2 )
      {
        color = 2;
        trap_R_SetColor( g_color_table[ ColorIndex( COLOR_BLUE ) ] );
      }
      
      v = -v;
      if( v > range )
        v = range;
      
      trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
    }
  }

  // draw the snapshot latency / drop graph
  range = ah / 2;
  vscale = range / MAX_LAGOMETER_PING;

  for( a = 0 ; a < aw ; a++ )
  {
    i = ( lagometer.snapshotCount - 1 - a ) & ( LAG_SAMPLES - 1 );
    v = lagometer.snapshotSamples[ i ];
    
    if( v > 0 )
    {
      if( lagometer.snapshotFlags[ i ] & SNAPFLAG_RATE_DELAYED )
      {
        if( color != 5 )
        {
          color = 5;  // YELLOW for rate delay
          trap_R_SetColor( g_color_table[ ColorIndex( COLOR_YELLOW ) ] );
        }
      }
      else
      {
        if( color != 3 )
        {
          color = 3;
        
          trap_R_SetColor( g_color_table[ ColorIndex( COLOR_GREEN ) ] );
        }
      }
    
      v = v * vscale;
      
      if( v > range )
        v = range;
      
      trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
    }
    else if( v < 0 )
    {
      if( color != 4 )
      {
        color = 4;    // RED for dropped snapshots
        trap_R_SetColor( g_color_table[ ColorIndex( COLOR_RED ) ] );
      }
      
      trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
    }
  }

  trap_R_SetColor( NULL );

  if( cg_nopredict.integer || cg_synchronousClients.integer )
    CG_DrawBigString( ax, ay, "snc", 1.0 );

  CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_CenterPrint( const char *str, int y, int charWidth )
{
  char  *s;

  Q_strncpyz( cg.centerPrint, str, sizeof( cg.centerPrint ) );

  cg.centerPrintTime = cg.time;
  cg.centerPrintY = y;
  cg.centerPrintCharWidth = charWidth;

  // count the number of lines for centering
  cg.centerPrintLines = 1;
  s = cg.centerPrint;
  while( *s )
  {
    if( *s == '\n' )
      cg.centerPrintLines++;
    
    s++;
  }
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void )
{
  char  *start;
  int   l;
  int   x, y, w;
  int h;
  float *color;

  if( !cg.centerPrintTime )
    return;

  color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
  if( !color )
    return;

  trap_R_SetColor( color );

  start = cg.centerPrint;

  y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

  while( 1 )
  {
    char linebuffer[ 1024 ];

    for( l = 0; l < 50; l++ )
    {
      if( !start[ l ] || start[ l ] == '\n' )
        break;
      
      linebuffer[ l ] = start[ l ];
    }
    
    linebuffer[ l ] = 0;

    w = CG_Text_Width( linebuffer, 0.5, 0 );
    h = CG_Text_Height( linebuffer, 0.5, 0 );
    x = ( SCREEN_WIDTH - w ) / 2;
    CG_Text_Paint( x, y + h, 0.5, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
    y += h + 6;

    while( *start && ( *start != '\n' ) )
      start++;
      
    if( !*start )
      break;
    
    start++;
  }

  trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIR

================================================================================
*/


/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair( void )
{
  float     w, h;
  qhandle_t hShader;
  float     f;
  float     x, y;
  int       ca;

  if( !cg_drawCrosshair.integer )
    return;

  if( ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) ||
      ( cg.snap->ps.stats[ STAT_STATE ] & SS_INFESTING ) ||
      ( cg.snap->ps.stats[ STAT_STATE ] & SS_HOVELING ) )
    return;

  if( cg.renderingThirdPerson )
    return;

  // set color based on health
  if( cg_crosshairHealth.integer )
  {
    vec4_t    hcolor;

    CG_ColorForHealth( hcolor );
    trap_R_SetColor( hcolor );
  }
  else
    trap_R_SetColor( NULL );

  w = h = cg_crosshairSize.value;

  // pulse the size of the crosshair when picking up items
  f = cg.time - cg.itemPickupBlendTime;
  if( f > 0 && f < ITEM_BLOB_TIME )
  {
    f /= ITEM_BLOB_TIME;
    w *= ( 1 + f );
    h *= ( 1 + f );
  }

  x = cg_crosshairX.integer;
  y = cg_crosshairY.integer;
  CG_AdjustFrom640( &x, &y, &w, &h );

  ca = cg_drawCrosshair.integer;
  if( ca < 0 )
    ca = 0;
  
  hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

  trap_R_DrawStretchPic( x + cg.refdef.x + 0.5 * ( cg.refdef.width - w ),
    y + cg.refdef.y + 0.5 * ( cg.refdef.height - h ),
    w, h, 0, 0, 1, 1, hShader );
}



/*
=================
CG_ScanForCrosshairEntity
=================
*/
static void CG_ScanForCrosshairEntity( void )
{
  trace_t   trace;
  vec3_t    start, end;
  int     content;

  VectorCopy( cg.refdef.vieworg, start );
  VectorMA( start, 131072, cg.refdef.viewaxis[ 0 ], end );

  CG_Trace( &trace, start, vec3_origin, vec3_origin, end,
    cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
  
  if( trace.entityNum >= MAX_CLIENTS )
    return;

  // if the player is in fog, don't show it
  content = trap_CM_PointContents( trace.endpos, 0 );
  if( content & CONTENTS_FOG )
    return;

  // update the fade timer
  cg.crosshairClientNum = trace.entityNum;
  cg.crosshairClientTime = cg.time;
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void CG_DrawCrosshairNames( void )
{
  float   *color;
  char    *name;
  float   w;

  if( !cg_drawCrosshair.integer )
    return;
  
  if( !cg_drawCrosshairNames.integer )
    return;
  
  if( cg.renderingThirdPerson )
    return;

  // scan the known entities to see if the crosshair is sighted on one
  CG_ScanForCrosshairEntity( );

  // draw the name of the player being looked at
  color = CG_FadeColor( cg.crosshairClientTime, 1000 );
  if( !color )
  {
    trap_R_SetColor( NULL );
    return;
  }

  name = cgs.clientinfo[ cg.crosshairClientNum ].name;
  color[ 3 ] *= 0.5f;
  w = CG_Text_Width( name, 0.3f, 0 );
  CG_Text_Paint( 320 - w / 2, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED );

  trap_R_SetColor( NULL );
}



//==============================================================================

/*
=================
CG_DrawVote
=================
*/
static void CG_DrawVote( void )
{
  char  *s;
  int   sec;

  if( !cgs.voteTime )
    return;

  // play a talk beep whenever it is modified
  if( cgs.voteModified )
  {
    cgs.voteModified = qfalse;
    trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
  }

  sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
  
  if( sec < 0 )
    sec = 0;
  
  s = va( "VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo );
  CG_DrawSmallString( 0, 58, s, 1.0F );
  s = "or press ESC then click Vote";
  CG_DrawSmallString( 0, 58 + SMALLCHAR_HEIGHT + 2, s, 1.0F );
}

/*
=================
CG_DrawTeamVote
=================
*/
static void CG_DrawTeamVote( void )
{
  char  *s;
  int   sec, cs_offset;

  if( cgs.clientinfo->team == TEAM_HUMANS )
    cs_offset = 0;
  else if( cgs.clientinfo->team == TEAM_ALIENS )
    cs_offset = 1;
  else
    return;

  if( !cgs.teamVoteTime[ cs_offset ] )
    return;

  // play a talk beep whenever it is modified
  if ( cgs.teamVoteModified[ cs_offset ] )
  {
    cgs.teamVoteModified[ cs_offset ] = qfalse;
    trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
  }

  sec = ( VOTE_TIME - ( cg.time - cgs.teamVoteTime[ cs_offset ] ) ) / 1000;
  
  if( sec < 0 )
    sec = 0;
  
  s = va( "TEAMVOTE(%i):%s yes:%i no:%i", sec, cgs.teamVoteString[ cs_offset ],
              cgs.teamVoteYes[cs_offset], cgs.teamVoteNo[ cs_offset ] );
  
  CG_DrawSmallString( 0, 90, s, 1.0F );
}


static qboolean CG_DrawScoreboard( )
{
  static qboolean firstTime = qtrue;
  float fade, *fadeColor;

  if( menuScoreboard )
    menuScoreboard->window.flags &= ~WINDOW_FORCED;
  
  if( cg_paused.integer )
  {
    cg.deferredPlayerLoading = 0;
    firstTime = qtrue;
    return qfalse;
  }

  if( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
      cg.predictedPlayerState.pm_type == PM_INTERMISSION )
  {
    fade = 1.0;
    fadeColor = colorWhite;
  }
  else
  {
    cg.deferredPlayerLoading = 0;
    cg.killerName[ 0 ] = 0;
    firstTime = qtrue;
    return qfalse;
  }                                           


  if( menuScoreboard == NULL )
    menuScoreboard = Menus_FindByName( "teamscore_menu" );

  if( menuScoreboard )
  {
    if( firstTime )
    {
      CG_SetScoreSelection( menuScoreboard );
      firstTime = qfalse;
    }
    
    Menu_Paint( menuScoreboard, qtrue );
  }

  return qtrue;
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void )
{
  if( cgs.gametype == GT_SINGLE_PLAYER )
  {
    CG_DrawCenterString( );
    return;
  }

  cg.scoreFadeTime = cg.time;
  cg.scoreBoardShowing = CG_DrawScoreboard( );
}

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow( void )
{
  float       x;
  vec4_t      color;
  const char  *name;

  if( !( cg.snap->ps.pm_flags & PMF_FOLLOW ) )
    return qfalse;
  
  color[ 0 ] = 1;
  color[ 1 ] = 1;
  color[ 2 ] = 1;
  color[ 3 ] = 1;


  CG_DrawBigString( 320 - 9 * 8, 24, "following", 1.0F );

  name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

  x = 0.5 * ( 640 - GIANT_WIDTH * CG_DrawStrlen( name ) );

  CG_DrawStringExt( x, 40, name, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );

  return qtrue;
}



/*
=================
CG_DrawAmmoWarning
=================
*/
static void CG_DrawAmmoWarning( void )
{
  const char  *s;
  int     w;

  if( cg.snap->ps.weapon == WP_NONE )
    return;
    
  if ( cg_drawAmmoWarning.integer == 0 )
    return;

  if ( !cg.lowAmmoWarning )
    return;

  if ( cg.lowAmmoWarning == 2 )
    s = "OUT OF AMMO";
  else
    s = "LOW AMMO WARNING";

  w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
  CG_DrawBigString( 320 - w / 2, 64, s, 1.0F );
}

/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup( void )
{
  int     w;
  int     sec;
  int     i;
  float   scale;
  clientInfo_t  *ci1, *ci2;
  int     cw;
  const char  *s;

  sec = cg.warmup;
  if( !sec )
    return;

  if( sec < 0 )
  {
    s = "Waiting for players";
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
    CG_DrawBigString( 320 - w / 2, 24, s, 1.0F );
    cg.warmupCount = 0;
    return;
  }

  if( cgs.gametype == GT_TOURNAMENT )
  {
    // find the two active players
    ci1 = NULL;
    ci2 = NULL;
    
    for( i = 0 ; i < cgs.maxclients ; i++ )
    {
      if( cgs.clientinfo[ i ].infoValid && cgs.clientinfo[ i ].team == TEAM_FREE )
      {
        if( !ci1 )
          ci1 = &cgs.clientinfo[ i ];
        else
          ci2 = &cgs.clientinfo[ i ];
      }
    }

    if( ci1 && ci2 )
    {
      s = va( "%s vs %s", ci1->name, ci2->name );
      w = CG_DrawStrlen( s );
      if( w > 640 / GIANT_WIDTH )
        cw = 640 / w;
      else
        cw = GIANT_WIDTH;
      
      CG_DrawStringExt( 320 - w * cw / 2, 20,s, colorWhite,
          qfalse, qtrue, cw, (int)( cw * 1.5f ), 0 );
    }
  }
  else
  {
    if( cgs.gametype == GT_FFA )
      s = "Free For All";
    else if ( cgs.gametype == GT_TEAM )
      s = "Team Deathmatch";
    else if ( cgs.gametype == GT_CTF ) 
      s = "Capture the Flag";
    else 
      s = "";
    
    w = CG_DrawStrlen( s );
    if( w > 640 / GIANT_WIDTH ) 
      cw = 640 / w;
    else 
      cw = GIANT_WIDTH;
    
    CG_DrawStringExt( 320 - w * cw / 2, 25,s, colorWhite,
        qfalse, qtrue, cw, (int)( cw * 1.1f ), 0 );
  }

  sec = ( sec - cg.time ) / 1000;
  if( sec < 0 )
  {
    cg.warmup = 0;
    sec = 0;
  }
  
  s = va( "Starts in: %i", sec + 1 );
  
  if( sec != cg.warmupCount )
  {
    cg.warmupCount = sec;
    switch( sec )
    {
      case 0:
        trap_S_StartLocalSound( cgs.media.count1Sound, CHAN_ANNOUNCER );
        break;
      case 1:
        trap_S_StartLocalSound( cgs.media.count2Sound, CHAN_ANNOUNCER );
        break;
      case 2:
        trap_S_StartLocalSound( cgs.media.count3Sound, CHAN_ANNOUNCER );
        break;
      default:
        break;
    }
  }
  
  scale = 0.45f;
  
  switch( cg.warmupCount )
  {
    case 0:
      cw = 28;
      scale = 0.54f;
      break;
    case 1:
      cw = 24;
      scale = 0.51f;
      break;
    case 2:
      cw = 20;
      scale = 0.48f;
      break;
    default:
      cw = 16;
      scale = 0.45f;
      break;
  }

  w = CG_DrawStrlen( s );
  CG_DrawStringExt( 320 - w * cw / 2, 70, s, colorWhite,
      qfalse, qtrue, cw, (int)( cw * 1.5 ), 0 );
}

//==================================================================================

#define SPECTATOR_STRING "SPECTATOR"
/*
=================
CG_Draw2D
=================
*/
static void CG_Draw2D( void )
{
  vec4_t  color;
  float   w;

  color[ 0 ] = color[ 1 ] = color[ 2 ] = color[ 3 ] = 1.0f;
  
  // if we are taking a levelshot for the menu, don't draw anything
  if( cg.levelShot )
    return;

  if( cg_draw2D.integer == 0 )
    return;

  if( cg.snap->ps.pm_type == PM_INTERMISSION )
  {
    CG_DrawIntermission( );
    return;
  }

  //TA: draw the lighting effects e.g. nvg
  CG_DrawLighting( );

  if( cg.snap->ps.persistant[ PERS_TEAM ] == TEAM_SPECTATOR )
  {
    w = CG_Text_Width( SPECTATOR_STRING, 0.7f, 0 );
    CG_Text_Paint( 320 - w / 2, 440, 0.7f, color, SPECTATOR_STRING, 0, 0, ITEM_TEXTSTYLE_SHADOWED );
  }
  else if( !( cg.snap->ps.stats[ STAT_STATE ] & SS_INFESTING ) &&
           !( cg.snap->ps.stats[ STAT_STATE ] & SS_HOVELING ) )
  {
    // don't draw any status if dead or the scoreboard is being explicitly shown
    if( !cg.showScores && cg.snap->ps.stats[ STAT_HEALTH ] > 0 )
    {
      if( cg_drawStatus.integer )
      {
        Menu_Paint(
          Menus_FindByName(
            BG_FindHudNameForClass( cg.predictedPlayerState.stats[ STAT_PCLASS ] ) ), qtrue );
      }
      
      CG_DrawAmmoWarning();
      CG_DrawCrosshair();
      
      if( BG_gotItem( UP_HELMET, cg.snap->ps.stats ) )
        CG_Scanner( );

      if( BG_ClassHasAbility( cg.predictedPlayerState.stats[ STAT_PCLASS ], SCA_ALIENSENSE ) )
        CG_AlienSense( );
    }
  }

  CG_DrawVote();
  CG_DrawTeamVote();

  CG_DrawLagometer();

  CG_DrawUpperRight();

  if( !CG_DrawFollow( ) )
    CG_DrawWarmup();

  // don't draw center string if scoreboard is up
  cg.scoreBoardShowing = CG_DrawScoreboard( );
  
  if( !cg.scoreBoardShowing )
    CG_DrawCenterString( );
}


static void CG_DrawTourneyScoreboard( )
{
  CG_DrawOldTourneyScoreboard();
}


/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView )
{
  float   separation;
  vec3_t    baseOrg;

  // optionally draw the info screen instead
  if( !cg.snap )
    return;

  // optionally draw the tournement scoreboard instead
  if( cg.snap->ps.persistant[ PERS_TEAM ] == TEAM_SPECTATOR &&
      ( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) )
  {
    CG_DrawTourneyScoreboard( );
    return;
  }

  switch ( stereoView )
  {
    case STEREO_CENTER:
      separation = 0;
      break;
    case STEREO_LEFT:
      separation = -cg_stereoSeparation.value / 2;
      break;
    case STEREO_RIGHT:
      separation = cg_stereoSeparation.value / 2;
      break;
    default:
      separation = 0;
      CG_Error( "CG_DrawActive: Undefined stereoView" );
  }

  // clear around the rendered view if sized down
  CG_TileClear( );

  // offset vieworg appropriately if we're doing stereo separation
  VectorCopy( cg.refdef.vieworg, baseOrg );
  
  if( separation != 0 )
    VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[ 1 ],
              cg.refdef.vieworg );

  // draw 3D view
  trap_R_RenderScene( &cg.refdef );

  // restore original viewpoint if running stereo
  if( separation != 0 )
    VectorCopy( baseOrg, cg.refdef.vieworg );

  // draw status bar and other floating elements
  CG_Draw2D( );
}


