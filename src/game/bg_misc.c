// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_misc.c -- both games misc functions, all completely stateless

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
                  
#include "q_shared.h"
#include "bg_public.h"

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"  override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

gitem_t bg_itemlist[] =
{
  {
    NULL,
    NULL,
    { NULL,
    NULL,
    0, 0} ,
/* icon */    NULL,
/* pickup */  NULL,
    0,
    0,
    0,
/* precache */ "",
/* sounds */ ""
  },  // leave index 0 alone

  //
  // ARMOR
  //

/*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_armor_shard",
    "sound/misc/ar1_pkup.wav",
    { "models/powerups/armor/shard.md3",
    "models/powerups/armor/shard_sphere.md3",
    0, 0} ,
    "icons/iconr_shard",
    "Armor Shard",
    5,
    IT_ARMOR,
    0,
    "",
    ""
  },*/

/*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_armor_combat",
    "sound/misc/ar2_pkup.wav",
    { "models/powerups/armor/armor_yel.md3",
    0, 0, 0},
    "icons/iconr_yellow",
    "Armor",
    50,
    IT_ARMOR,
    0,
    "",
    ""
  },*/

/*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_armor_body",
    "sound/misc/ar3_pkup.wav",
    { "models/powerups/armor/armor_red.md3",
    0, 0, 0},
    "icons/iconr_red",
    "Heavy Armor",
    100,
    IT_ARMOR,
    0,
    "",
    ""
  },*/

  //
  // health
  //
/*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_health_small",
    "sound/items/s_health.wav",
    { "models/powerups/health/small_cross.md3",
    "models/powerups/health/small_sphere.md3",
    0, 0 },
    "icons/iconh_green",
    "5 Health",
    5,
    IT_HEALTH,
    0,
    "",
    ""
  },*/

/*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_health",
    "sound/items/n_health.wav",
    { "models/powerups/health/medium_cross.md3",
    "models/powerups/health/medium_sphere.md3",
    0, 0 },
    "icons/iconh_yellow",
    "25 Health",
    25,
    IT_HEALTH,
    0,
    "",
    ""
  },*/

/*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_health_large",
    "sound/items/l_health.wav",
    { "models/powerups/health/large_cross.md3",
    "models/powerups/health/large_sphere.md3",
    0, 0 },
    "icons/iconh_red",
    "50 Health",
    50,
    IT_HEALTH,
    0,
    "",
    ""
  },*/

/*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_health_mega",
    "sound/items/m_health.wav",
    { "models/powerups/health/mega_cross.md3",
    "models/powerups/health/mega_sphere.md3",
    0, 0 },
    "icons/iconh_mega",
    "Mega Health",
    100,
    IT_HEALTH,
    0,
    "",
    ""
  },*/


  //
  // WEAPONS
  //

/*QUAKED weapon_gauntlet (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/

//TA:FIXME: must keep gauntlet and machinegun for now or bots have a fit and prevent game working

  {
    "weapon_gauntlet",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Gauntlet",
    0,
    IT_WEAPON,
    WP_GAUNTLET,
    "",
    ""
  },

/*QUAKED weapon_venom (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_venom",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Venom",
    0,
    IT_WEAPON,
    WP_VENOM,
    "",
    ""
  },

/*QUAKED weapon_grabandcsaw (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_grabandcsaw",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Circular Saw",
    0,
    IT_WEAPON,
    WP_GRABANDCSAW,
    "",
    ""
  },

/*QUAKED weapon_pounce (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_pounce",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Claw and Pounce",
    0,
    IT_WEAPON,
    WP_POUNCE,
    "",
    ""
  },

/*QUAKED weapon_dbuild (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_dbuild",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Droid Build",
    0,
    IT_WEAPON,
    WP_DBUILD,
    "",
    ""
  },
  
/*QUAKED weapon_dbuild2 (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_dbuild2",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Droid Build 2",
    0,
    IT_WEAPON,
    WP_DBUILD2,
    "",
    ""
  },
  
/*QUAKED weapon_hbuild (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_hbuild",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "HBuild",
    0,
    IT_WEAPON,
    WP_HBUILD,
    "",
    ""
  },
  
/*QUAKED weapon_hbuild2 (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_hbuild2",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Human Build 2",
    0,
    IT_WEAPON,
    WP_HBUILD2,
    "",
    ""
  },
  
/*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "weapon_shotgun",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/shotgun/shotgun.md3",
    0, 0, 0},
    "icons/iconw_shotgun",
    "Shotgun",
    10,
    IT_WEAPON,
    WP_SHOTGUN,
    "",
    ""
  },*/

/*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_machinegun",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/machinegun/machinegun.md3",
    0, 0, 0},
    "icons/iconw_machinegun",
    "Machinegun",
    40,
    IT_WEAPON,
    WP_MACHINEGUN,
    "",
    ""
  },

/*QUAKED weapon_chaingun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_chaingun",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/machinegun/machinegun.md3",
    0, 0, 0},
    "icons/iconw_machinegun",
    "Chaingun",
    40,
    IT_WEAPON,
    WP_CHAINGUN,
    "",
    ""
  },
  
/*QUAKED weapon_massdriver (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_massdriver",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/bfg/bfg.md3",
    0, 0, 0},
    "icons/iconw_bfg",
    "Mass Driver",
    40,
    IT_WEAPON,
    WP_MASS_DRIVER,
    "",
    ""
  },
  
/*QUAKED weapon_pulserifle (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_pulserifle",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/plasma/plasma.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Pulse Rifle",
    40,
    IT_WEAPON,
    WP_PULSE_RIFLE,
    "",
    ""
  },
  
/*QUAKED weapon_ggrenade (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_ggrenade",
    "sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
    0, 0, 0},
    "icons/iconw_gauntlet",
    "Gas Grenade",
    0,
    IT_WEAPON,
    WP_GGRENADE,
    "",
    ""
  },

/*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "weapon_grenadelauncher",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/grenadel/grenadel.md3",
    0, 0, 0},
    "icons/iconw_grenade",
    "Grenade Launcher",
    10,
    IT_WEAPON,
    WP_GRENADE_LAUNCHER,
    "",
    "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"
  },*/

/*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "weapon_rocketlauncher",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/rocketl/rocketl.md3",
    0, 0, 0},
    "icons/iconw_rocket",
    "Rocket Launcher",
    10,
    IT_WEAPON,
    WP_ROCKET_LAUNCHER,
    "",
    ""
  },*/

/*QUAKED weapon_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "weapon_lightning",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/lightning/lightning.md3",
    0, 0, 0},
    "icons/iconw_lightning",
    "Lightning Gun",
    100,
    IT_WEAPON,
    WP_LIGHTNING,
    "",
    ""
  },*/

/*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_railgun",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/railgun/railgun.md3",
    0, 0, 0},
    "icons/iconw_railgun",
    "Railgun",
    10,
    IT_WEAPON,
    WP_RAILGUN,
    "",
    ""
  },

/*QUAKED weapon_sawbladelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_lockbloblauncher",
    "sound/misc/w_pkup.wav",
    { 0, 0, 0, 0},
    "icons/iconw_rocket",
    "Lockblob Launcher",
    10,
    IT_WEAPON,
    WP_LOCKBLOB_LAUNCHER,
    "",
    ""
  },

/*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_plasmagun",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/plasma/plasma.md3",
    0, 0, 0},
    "icons/iconw_plasma",
    "Plasma Gun",
    50,
    IT_WEAPON,
    WP_PLASMAGUN,
    "",
    ""
  },
  
/*QUAKED weapon_flamer (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_flamer",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/plasma/plasma.md3",
    0, 0, 0},
    "icons/iconw_plasma",
    "Flame Thrower",
    50,
    IT_WEAPON,
    WP_FLAMER,
    "",
    ""
  },

/*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  {
    "weapon_bfg",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/bfg/bfg.md3",
    0, 0, 0},
    "icons/iconw_bfg",
    "Dual BFG",
    0,
    IT_WEAPON,
    WP_BFG,
    "",
    ""
  },

/*QUAKED weapon_grapplinghook (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "weapon_grapplinghook",
    "sound/misc/w_pkup.wav",
    { "models/weapons2/grapple/grapple.md3",
    0, 0, 0},
    "Grappling Hook",
    0,
    IT_WEAPON,
    WP_GRAPPLING_HOOK,
    "",
    ""
  },*/

  //
  // AMMO ITEMS
  //

/*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_shells",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/shotgunam.md3",
    0, 0, 0},
    "icons/icona_shotgun",
    "Shells",
    10,
    IT_AMMO,
    WP_SHOTGUN,
    "",
    ""
  },*/

/*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_bullets",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/machinegunam.md3",
    0, 0, 0},
    "icons/icona_machinegun",
    "Bullets",
    50,
    IT_AMMO,
    WP_MACHINEGUN,
    "",
    ""
  },*/

/*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_grenades",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/grenadeam.md3",
    0, 0, 0},
    "icons/icona_grenade",
    "Grenades",
    5,
    IT_AMMO,
    WP_GRENADE_LAUNCHER,
    "",
    ""
  },*/

/*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_cells",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/plasmaam.md3",
    0, 0, 0},
    "icons/icona_plasma",
    "Cells",
    30,
    IT_AMMO,
    WP_PLASMAGUN,
    "",
    ""
  },*/

/*QUAKED ammo_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_lightning",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/lightningam.md3",
    0, 0, 0},
    "icons/icona_lightning",
    "Lightning",
    60,
    IT_AMMO,
    WP_LIGHTNING,
    "",
    ""
  },*/

/*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_rockets",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/rocketam.md3",
    0, 0, 0},
    "icons/icona_rocket",
    "Rockets",
    5,
    IT_AMMO,
    WP_ROCKET_LAUNCHER,
    "",
    ""
  },*/

/*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_slugs",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/railgunam.md3",
    0, 0, 0},
    "icons/icona_railgun",
    "Slugs",
    10,
    IT_AMMO,
    WP_RAILGUN,
    "",
    ""
  },*/

/*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "ammo_bfg",
    "sound/misc/am_pkup.wav",
    { "models/powerups/ammo/bfgam.md3",
    0, 0, 0},
    "icons/icona_bfg",
    "Bfg Ammo",
    15,
    IT_AMMO,
    WP_BFG,
    "",
    ""
  },*/

  //
  // HOLDABLE ITEMS
  //
/*QUAKED holdable_teleporter (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "holdable_teleporter",
    "sound/items/holdable.wav",
    { "models/powerups/holdable/teleporter.md3",
    0, 0, 0},
    "icons/teleporter",
    "Personal Teleporter",
    60,
    IT_HOLDABLE,
    HI_TELEPORTER,
    "",
    ""
  },*/

/*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "holdable_medkit",
    "sound/items/holdable.wav",
    { "models/powerups/holdable/medkit.md3",
    "models/powerups/holdable/medkit_sphere.md3",
    0, 0},
    "icons/medkit",
    "Medkit",
    60,
    IT_HOLDABLE,
    HI_MEDKIT,
    "",
    "sound/items/use_medkit.wav"
  },*/

  //
  // POWERUP ITEMS
  //
/*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_quad",
    "sound/items/quaddamage.wav",
    { "models/powerups/instant/quad.md3",
    "models/powerups/instant/quad_ring.md3",
    0, 0 },
    "icons/quad",
    "Quad Damage",
    30,
    IT_POWERUP,
    PW_QUAD,
    "",
    "sound/items/damage2.wav sound/items/damage3.wav"
  },*/

/*QUAKED item_enviro (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_enviro",
    "sound/items/protect.wav",
    { "models/powerups/instant/enviro.md3",
    "models/powerups/instant/enviro_ring.md3",
    0, 0 },
    "icons/envirosuit",
    "Battle Suit",
    30,
    IT_POWERUP,
    PW_BATTLESUIT,
    "",
    "sound/items/airout.wav sound/items/protect3.wav"
  },*/

/*QUAKED item_haste (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_haste",
    "sound/items/haste.wav",
    { "models/powerups/instant/haste.md3",
    "models/powerups/instant/haste_ring.md3",
    0, 0 },
    "icons/haste",
    "Speed",
    30,
    IT_POWERUP,
    PW_HASTE,
    "",
    ""
  },*/

/*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_invis",
    "sound/items/invisibility.wav",
    { "models/powerups/instant/invis.md3",
    "models/powerups/instant/invis_ring.md3",
    0, 0 },
    "icons/invis",
    "Invisibility",
    30,
    IT_POWERUP,
    PW_INVIS,
    "",
    ""
  },*/

/*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_regen",
    "sound/items/regeneration.wav",
    { "models/powerups/instant/regen.md3",
    "models/powerups/instant/regen_ring.md3",
    0, 0 },
    "icons/regen",
    "Regeneration",
    30,
    IT_POWERUP,
    PW_REGEN,
    "",
    "sound/items/regen.wav"
  },*/

/*QUAKED item_flight (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
  /*{
    "item_flight",
    "sound/items/flight.wav",
    { "models/powerups/instant/flight.md3",
    "models/powerups/instant/flight_ring.md3",
    0, 0 },
    "icons/flight",
    "Flight",
    60,
    IT_POWERUP,
    PW_FLIGHT,
    "",
    "sound/items/flight.wav"
  },*/

/*QUAKED team_droid_spawn (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid spawn item
*/
  {
    "team_droid_spawn",
    "sound/items/holdable.wav",
    { "models/buildables/bioegg/bioegg.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Droid Spawn",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_SPAWN,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_droid_barricade (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid defense item
*/
  {
    "team_droid_def1",
    "sound/items/holdable.wav",
    { "models/buildables/barricade/barricade.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Barricade",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_BARRICADE,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_droid_acid_tube (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid defense item
*/
  {
    "team_droid_acid_tube",
    "sound/items/holdable.wav",
    { "models/buildables/acid_tube/acid_tube.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Acid Tube",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_ACIDTUBE,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_droid_trapper (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid defense item
*/
  {
    "team_droid_trapper",
    "sound/items/holdable.wav",
    { "models/buildables/trapper/trapper.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Trapper",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_TRAPPER,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_droid_trapper (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid defense item
*/
  {
    "team_droid_booster",
    "sound/items/holdable.wav",
    { "models/buildables/booster/booster.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Booster",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_BOOSTER,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_droid_hivemind (0 0 1) (-16 -16 -16) (16 16 16)
TA: droid build limitation item
*/
  {
    "team_droid_hivemind",
    "sound/items/holdable.wav",
    { "models/buildables/hivemind/hivemind.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Droid Hivemind",      //pickup
    0,
    IT_BUILDABLE,
    BA_D_HIVEMIND,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_spawn (0 0 1) (-16 -16 -16) (16 16 16)
TA: human spawn item
*/
  {
    "team_human_spawn",
    "sound/items/holdable.wav",
    { "models/buildables/replicator/replicator.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Spawn",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_SPAWN,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_human_def1 (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_def1",
    "sound/items/holdable.wav",
    { "models/buildables/plasmaturret/pturret_base.md3", "models/buildables/plasmaturret/pturret_top.md3", 0, 0 },
    "icons/teleporter", //icon
    "Human Defense",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_DEF1,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_human_def2 (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_def2",
    "sound/items/holdable.wav",
    { "models/buildables/plasmaturret/pturret_base.md3", "models/weapons2/machinegun/machinegun.md3", 0, 0 },
    "icons/teleporter", //icon
    "Human Defense2",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_DEF2,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_human_def3 (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_def3",
    "sound/items/holdable.wav",
    { "models/buildables/plasmaturret/pturret_base.md3", "models/weapons2/railgun/railgun.md3", 0, 0 },
    "icons/teleporter", //icon
    "Human Defense3",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_DEF3,
    "",                 //precache
    ""                  //sounds
  },

/*QUAKED team_human_mcu (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_mcu",
    "sound/items/holdable.wav",
    { "models/buildables/mcu/mcu.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human MCU",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_MCU,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_dcc (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_dcc",
    "sound/items/holdable.wav",
    { "models/buildables/dcc/dcc.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human DCC",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_DCC,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_medistat (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_medistat",
    "sound/items/holdable.wav",
    { "models/buildables/medistat/medistat.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Medistation",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_MEDISTAT,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_bank (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_bank",
    "sound/items/holdable.wav",
    { "models/buildables/bank/bank.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Bank",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_BANK,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_reactor (0 0 1) (-16 -16 -16) (16 16 16)
TA: human power item
*/
  {
    "team_human_reactor",
    "sound/items/holdable.wav",
    { "models/buildables/reactor/reactor.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Reactor",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_REACTOR,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_repeater (0 0 1) (-16 -16 -16) (16 16 16)
TA: human power item
*/
  {
    "team_human_repeater",
    "sound/items/holdable.wav",
    { "models/buildables/repeater/repeater.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Repeater",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_REPEATER,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_human_floatmine (0 0 1) (-16 -16 -16) (16 16 16)
TA: human defense item
*/
  {
    "team_human_floatmine",
    "sound/items/holdable.wav",
    { "models/buildables/floatmine/floatmine.md3", 0, 0, 0 },
    "icons/teleporter", //icon
    "Human Floatmine",      //pickup
    0,
    IT_BUILDABLE,
    BA_H_FLOATMINE,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_torch (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_torch",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "Torch",      //pickup
    0,
    IT_UPGRADE,
    UP_TORCH,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_nvg (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_nvg",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "NVG",      //pickup
    0,
    IT_UPGRADE,
    UP_NVG,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_carmour (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_carmour",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "Chest Armour",      //pickup
    0,
    IT_UPGRADE,
    UP_CHESTARMOUR,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_larmour (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_larmour",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "Limb Armour",      //pickup
    0,
    IT_UPGRADE,
    UP_LIMBARMOUR,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_helmet (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_helmet",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "Helmet",      //pickup
    0,
    IT_UPGRADE,
    UP_HELMET,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED upgrade_bsuit (0 0 1) (-16 -16 -16) (16 16 16)
*/
  {
    "upgrade_bsuit",
    "sound/items/holdable.wav",
    { 0, 0, 0, 0 },
    "icons/teleporter", //icon
    "Battle Suit",      //pickup
    0,
    IT_UPGRADE,
    UP_BATTLESUIT,
    "",                 //precache
    ""                  //sounds
  },
  
/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
  /*{
    "team_CTF_redflag",
    "sound/teamplay/flagtk_red.wav",
    { "models/flags/r_flag.md3",
    0, 0, 0 },
    "icons/iconf_red1",
    "Red Flag",
    0,
    IT_TEAM,
    PW_REDFLAG,
    "",
    "sound/teamplay/flagcap_red.wav sound/teamplay/flagtk_red.wav sound/teamplay/flagret_red.wav"
  },*/

/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
  /*{
    "team_CTF_blueflag",
    "sound/teamplay/flagtk_blu.wav",
    { "models/flags/b_flag.md3",
    0, 0, 0 },
    "icons/iconf_blu1",
    "Blue Flag",
    0,
    IT_TEAM,
    PW_BLUEFLAG,
    "",
    "sound/teamplay/flagcap_blu.wav sound/teamplay/flagtk_blu.wav sound/teamplay/flagret_blu.wav"
  },*/

  // end of list marker
  {NULL}
};

int   bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;

////////////////////////////////////////////////////////////////////////////////

buildableAttributes_t bg_buildableList[ ] =
{
  {
    BA_D_SPAWN,            //int       buildNum;     
    "bioegg",              //char      *buildName;
    "team_droid_spawn",    //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    100,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    200,                   //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.5f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    120,                   //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_D_BARRICADE,        //int       buildNum;
    "barricade",           //char      *buildName;
    "team_droid_barricade",//char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    120,                   //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_D_BOOSTER,          //int       buildNum;
    "booster",             //char      *buildName;
    "team_droid_booster",  //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    120,                   //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_D_ACIDTUBE,         //int       buildNum;
    "acid_tube",           //char      *buildName;
    "team_droid_acid_tube",//char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    400,                   //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    500,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    120,                   //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_D_TRAPPER,          //int       buildNum;
    "trapper",             //char      *buildName;
    "team_droid_trapper",  //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    1000,                  //int       nextthink;
    0,                     //int       turretFireSpeed;
    400,                   //int       turretRange;
    WP_LOCKBLOB_LAUNCHER,  //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    30,                    //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_D_HIVEMIND,         //int       buildNum;
    "hivemind",            //char      *buildName;
    "team_droid_hivemind", //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    0,                     //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_DSPAWN,            //int       meansOfDeath;
    BIT_DROIDS,            //int       team;
    ( 1 << WP_DBUILD )|( 1 << WP_DBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    -1,                    //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    120,                   //int       creepSize;
    qtrue                  //qboolean  reactorTest;
  },
  {
    BA_H_SPAWN,            //int       buildNum;
    "replicator",          //char      *buildName;
    "team_human_spawn",    //char      *entityName;
    { -40, -40, -4 },      //vec3_t    mins;
    { 40, 40, 4 },         //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    100,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_MEDISTAT,         //int       buildNum;
    "medistat",            //char      *buildName;
    "team_human_medistat", //char      *entityName;
    { -40, -40, -4 },      //vec3_t    mins;
    { 40, 40, 4 },         //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    100,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_DEF1,             //int       buildNum;
    "plasmaturret",        //char      *buildName;
    "team_human_def1",     //char      *entityName;
    { -24, -24, -11 },     //vec3_t    mins;
    { 24, 24, 11 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    50,                    //int       nextthink;
    500,                   //int       turretFireSpeed;
    500,                   //int       turretRange;
    WP_PLASMAGUN,          //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_DEF2,             //int       buildNum;
    "mgturret",            //char      *buildName;
    "team_human_def2",     //char      *entityName;
    { -24, -24, -11 },     //vec3_t    mins;
    { 24, 24, 11 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    50,                    //int       nextthink;
    50,                    //int       turretFireSpeed;
    300,                   //int       turretRange;
    WP_MACHINEGUN,         //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_DEF3,             //int       buildNum;
    "railturret",          //char      *buildName;
    "team_human_def3",     //char      *entityName;
    { -24, -24, -11 },     //vec3_t    mins;
    { 24, 24, 11 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    80,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    20,                    //int       splashDamage;
    50,                    //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    150,                   //int       nextthink;
    4000,                  //int       turretFireSpeed;
    1500,                  //int       turretRange;
    WP_RAILGUN,            //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_DCC,              //int       buildNum;
    "dcc",                 //char      *buildName;
    "team_human_dcc",      //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    200,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_BANK,             //int       buildNum;
    "bank",                //char      *buildName;
    "team_human_bank",     //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    200,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_MCU,              //int       buildNum;
    "mcu",                 //char      *buildName;
    "team_human_mcu",      //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    200,                   //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_REACTOR,          //int       buildNum;
    "reactor",             //char      *buildName;
    "team_human_reactor",  //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    0,                     //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    -1,                    //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qtrue                  //qboolean  reactorTest;
  },
  {
    BA_H_REPEATER,         //int       buildNum;
    "repeater",            //char      *buildName;
    "team_human_repeater", //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_GRAVITY,            //trType_t traj;
    0.0,                   //float        bounce;
    10,                    //int       buildPoints;
    1000,                  //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  },
  {
    BA_H_FLOATMINE,        //int       buildNum;
    "floatmine",           //char      *buildName;
    "team_human_floatmine",//char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    TR_BUOYANCY,           //trType_t traj;
    0.2,                   //float        bounce;
    10,                    //int       buildPoints;
    100,                   //int       health;
    50,                    //int       damage;
    50,                    //int       splashDamage;
    150,                   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    BIT_HUMANS,            //int       team;
    ( 1 << WP_HBUILD )|( 1 << WP_HBUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    0,                     //int       turretFireSpeed;
    0,                     //int       turretRange;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse                 //qboolean  reactorTest;
  }
};

int   bg_numBuildables = sizeof( bg_buildableList ) / sizeof( bg_buildableList[ 0 ] );

/*
==============
BG_FindBuildNumForName
==============
*/
int BG_FindBuildNumForName( char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].buildName, name ) )
      return bg_buildableList[ i ].buildNum;
  }

  //wimp out
  return BA_NONE;
}

/*
==============
BG_FindBuildNumForEntityName
==============
*/
int BG_FindBuildNumForEntityName( char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].entityName, name ) )
      return bg_buildableList[ i ].buildNum;
  }

  //wimp out
  return BA_NONE;
}

/*
==============
BG_FindNameForBuildNum
==============
*/
char *BG_FindNameForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
      return bg_buildableList[ i ].buildName;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindEntityNameForBuildNum
==============
*/
char *BG_FindEntityNameForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
      return bg_buildableList[ i ].entityName;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindBBoxForBuildable
==============
*/
void BG_FindBBoxForBuildable( int bclass, vec3_t mins, vec3_t maxs )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      if( mins != NULL )
        VectorCopy( bg_buildableList[ i ].mins, mins );
        
      if( maxs != NULL )
        VectorCopy( bg_buildableList[ i ].maxs, maxs );
        
      return;
    }
  }
  
  if( mins != NULL )
    VectorCopy( bg_buildableList[ 0 ].mins, mins );
    
  if( maxs != NULL )
    VectorCopy( bg_buildableList[ 0 ].maxs, maxs );
}

/*
==============
BG_FindTrajectoryForBuildable
==============
*/
trType_t BG_FindTrajectoryForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].traj;
    }
  }
  
  return TR_GRAVITY;
}

/*
==============
BG_FindBounceForBuildable
==============
*/
float BG_FindBounceForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].bounce;
    }
  }
  
  return 0.0;
}

/*
==============
BG_FindBuildPointsForBuildable
==============
*/
int BG_FindBuildPointsForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].buildPoints;
    }
  }
  
  return 1000;
}

/*
==============
BG_FindHealthForBuildable
==============
*/
int BG_FindHealthForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].health;
    }
  }
  
  return 1000;
}

/*
==============
BG_FindDamageForBuildable
==============
*/
int BG_FindDamageForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].damage;
    }
  }
  
  return 50;
}

/*
==============
BG_FindSplashDamageForBuildable
==============
*/
int BG_FindSplashDamageForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].splashDamage;
    }
  }
  
  return 50;
}

/*
==============
BG_FindSplashRadiusForBuildable
==============
*/
int BG_FindSplashRadiusForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].splashRadius;
    }
  }
  
  return 200;
}

/*
==============
BG_FindMODForBuildable
==============
*/
int BG_FindMODForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].meansOfDeath;
    }
  }
  
  return MOD_UNKNOWN;
}

/*
==============
BG_FindTeamForBuildable
==============
*/
int BG_FindTeamForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].team;
    }
  }
  
  return BIT_NONE;
}

/*
==============
BG_FindBuildWeaponForBuildable
==============
*/
weapon_t BG_FindBuildWeaponForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].buildWeapon;
    }
  }
  
  return BA_NONE;
}

/*
==============
BG_FindAnimForBuildable
==============
*/
int BG_FindAnimForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].idleAnim;
    }
  }
  
  return BANIM_IDLE1;
}

/*
==============
BG_FindNextThinkForBuildable
==============
*/
int BG_FindNextThinkForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].nextthink;
    }
  }
  
  return 100;
}

/*
==============
BG_FindFireSpeedForBuildable
==============
*/
int BG_FindFireSpeedForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].turretFireSpeed;
    }
  }
  
  return 1000;
}

/*
==============
BG_FindRangeForBuildable
==============
*/
int BG_FindRangeForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].turretRange;
    }
  }
  
  return 1000;
}

/*
==============
BG_FindProjTypeForBuildable
==============
*/
weapon_t BG_FindProjTypeForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].turretProjType;
    }
  }
  
  return WP_NONE;
}

/*
==============
BG_FindMinNormalForBuildable
==============
*/
float BG_FindMinNormalForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].minNormal;
    }
  }
  
  return 0.707f;
}

/*
==============
BG_FindInvertNormalForBuildable
==============
*/
qboolean BG_FindInvertNormalForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].invertNormal;
    }
  }
  
  return qfalse;
}

/*
==============
BG_FindCreepTestForBuildable
==============
*/
int BG_FindCreepTestForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].creepTest;
    }
  }
  
  return qfalse;
}

/*
==============
BG_FindCreepSizeForBuildable
==============
*/
int BG_FindCreepSizeForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].creepSize;
    }
  }
  
  return CREEP_BASESIZE;
}

/*
==============
BG_FindUniqueTestForBuildable
==============
*/
int BG_FindUniqueTestForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].buildNum == bclass )
    {
      return bg_buildableList[ i ].reactorTest;
    }
  }
  
  return qfalse;
}

////////////////////////////////////////////////////////////////////////////////

classAttributes_t bg_classList[ ] =
{
  { 
    PCL_D_B_BASE,                                   //int     classnum;
    "Builder",                                      //char    *classname;
    "lucy",                                         //char    *modelname;
    "default",                                      //char    *skinname;
    { -15, -15, -20 },                              //vec3_t  mins;
    { 15, 15, 20 },                                 //vec3_t  maxs;
    { 15, 15, 20 },                                 //vec3_t  crouchmaxs;
    { -15, -15, -4 },                               //vec3_t  deadmins;
    { 15, 15, 4 },                                  //vec3_t  deadmaxs;
    12, 12,                                         //int     viewheight, crouchviewheight;
    50,                                             //int     health;
    0,                                              //int     armor;
    SCA_TAKESFALLDAMAGE|SCA_FOVWARPS,               //int     abilities;
    50.0f,                                          //float   buildDist;
    1000.0f,                                        //float   launchSpeed;
    80,                                             //int     fov;
    0.015f,                                         //float   bob;
    350,                                            //int     steptime;
    0.5f,                                           //float   speed;
    1.0f,                                           //float   sticky;
    { PCL_D_B_LEV1, PCL_NONE, PCL_NONE },           //int     children[ 3 ];
    2000,                                           //int     timetoevolve;
    100                                             //int     value;
  },
  { 
    PCL_D_B_LEV1,                                   //int     classnum;
    "BuilderLevel1",                                //char    *classname;
    "lucy",                                         //char    *modelname;
    "angel",                                        //char    *skinname;
    { -15, -15, -20 },                              //vec3_t  mins;
    { 15, 15, 20 },                                 //vec3_t  maxs;
    { 15, 15, 20 },                                 //vec3_t  crouchmaxs;
    { -15, -15, -4 },                               //vec3_t  deadmins;
    { 15, 15, 4 },                                  //vec3_t  deadmaxs;
    12, 12,                                         //int     viewheight, crouchviewheight;
    75,                                             //int     health;
    10,                                             //int     armor;
    SCA_CANJUMP|SCA_FOVWARPS,                       //int     abilities;
    50.0f,                                          //float   buildDist;
    0.0f,                                           //float   launchSpeed;
    110,                                            //int     fov;
    0.005f,                                         //float   bob;
    200,                                            //int     steptime;
    1.0f,                                           //float   speed;
    1.0f,                                           //float   sticky;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    2000,                                           //int     timetoevolve;
    100                                             //int     value;
  },
  {
    PCL_D_O_BASE,                                               //int     classnum;
    "Offensive",                                                //char    *classname;
    "klesk",                                                    //char    *modelname;
    "default",                                                  //char    *skinname;
    { -15, -15, -15 },                                          //vec3_t  mins;
    { 15, 15, 15 },                                             //vec3_t  maxs;
    { 15, 15, 15 },                                             //vec3_t  crouchmaxs;
    { -15, -15, -4 },                                           //vec3_t  deadmins;
    { 15, 15, 4 },                                              //vec3_t  deadmaxs;
    0, 0,                                                       //int     viewheight, crouchviewheight;
    25,                                                         //int     health;
    0,                                                          //int     armor;
    SCA_WALLCLIMBER|SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS, //int     abilities;
    0.0f,                                                       //float   buildDist;
    0.0f,                                                       //float   launchSpeed;
    140,                                                        //int     fov;
    0.0f,                                                       //float   bob;
    25,                                                         //int     steptime;
    1.8f,                                                       //float   speed;
    5.0f,                                                       //float   sticky;
    { PCL_D_O_LEV1, PCL_NONE, PCL_NONE },                       //int     children[ 3 ];
    3000,                                                       //int     timetoevolve;
    100                                                         //int     value;
  },
  {
    PCL_D_O_LEV1,                                 //int     classnum;
    "OffensiveLevel1",                            //char    *classname;
    "anarki",                                     //char    *modelname;
    "default",                                    //char    *skinname;
    { -24, -24, -24 },                            //vec3_t  mins;
    { 24, 24, 24 },                               //vec3_t  maxs;
    { 24, 24, 24 },                               //vec3_t  crouchmaxs;
    { -24, -24, -4 },                             //vec3_t  deadmins;
    { 24, 24, 4 },                                //vec3_t  deadmaxs;
    6, 6,                                         //int     viewheight, crouchviewheight;
    50,                                           //int     health;
    5,                                            //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    120,                                          //int     fov;
    0.001f,                                       //float   bob;
    25,                                           //int     steptime;
    1.6f,                                         //float   speed;
    4.0f,                                         //float   sticky;
    { PCL_D_O_LEV2, PCL_D_O_LEV1_UPG, PCL_NONE }, //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV1_UPG,                             //int     classnum;
    "OffensiveLevel1-Upgrade",                    //char    *classname;
    "anarki",                                     //char    *modelname;
    "default",                                    //char    *skinname;
    { -24, -24, -24 },                            //vec3_t  mins;
    { 24, 24, 24 },                               //vec3_t  maxs;
    { 24, 24, 24 },                               //vec3_t  crouchmaxs;
    { -24, -24, -4 },                             //vec3_t  deadmins;
    { 24, 24, 4 },                                //vec3_t  deadmaxs;
    6, 6,                                         //int     viewheight, crouchviewheight;
    50,                                           //int     health;
    5,                                            //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    120,                                          //int     fov;
    0.001f,                                       //float   bob;
    25,                                           //int     steptime;
    1.6f,                                         //float   speed;
    4.0f,                                         //float   sticky;
    { PCL_D_O_LEV2, PCL_NONE, PCL_NONE },         //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV2,                                 //int     classnum;
    "OffensiveLevel2",                            //char    *classname;
    "bones",                                      //char    *modelname;
    "default",                                    //char    *skinname;
    { -24, -24, -24 },                            //vec3_t  mins;
    { 24, 24, 24 },                               //vec3_t  maxs;
    { 24, 24, 24 },                               //vec3_t  crouchmaxs;
    { -24, -24, -4 },                             //vec3_t  deadmins;
    { 24, 24, 4 },                                //vec3_t  deadmaxs;
    6, 6,                                         //int     viewheight, crouchviewheight;
    50,                                           //int     health;
    10,                                           //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    120,                                          //int     fov;
    0.0005f,                                      //float   bob;
    25,                                           //int     steptime;
    1.5f,                                         //float   speed;
    3.0f,                                         //float   sticky;
    { PCL_D_O_LEV3, PCL_D_O_LEV2_UPG, PCL_NONE }, //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV2_UPG,                             //int     classnum;
    "OffensiveLevel2-Upgrade",                    //char    *classname;
    "bones",                                      //char    *modelname;
    "default",                                    //char    *skinname;
    { -24, -24, -24 },                            //vec3_t  mins;
    { 24, 24, 24 },                               //vec3_t  maxs;
    { 24, 24, 24 },                               //vec3_t  crouchmaxs;
    { -24, -24, -4 },                             //vec3_t  deadmins;
    { 24, 24, 4 },                                //vec3_t  deadmaxs;
    6, 6,                                         //int     viewheight, crouchviewheight;
    50,                                           //int     health;
    10,                                           //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    120,                                          //int     fov;
    0.0005f,                                      //float   bob;
    25,                                           //int     steptime;
    1.5f,                                         //float   speed;
    3.0f,                                         //float   sticky;
    { PCL_D_O_LEV3, PCL_NONE, PCL_NONE },         //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV3,                                 //int     classnum;
    "OffensiveLevel3",                            //char    *classname;
    "orbb",                                       //char    *modelname;
    "default",                                    //char    *skinname;
    { -15, -15, -15 },                            //vec3_t  mins;
    { 15, 15, 15 },                               //vec3_t  maxs;
    { 15, 15, 15 },                               //vec3_t  crouchmaxs;
    { -15, -15, -4 },                             //vec3_t  deadmins;
    { 15, 15, 4 },                                //vec3_t  deadmaxs;
    4, 4,                                         //int     viewheight, crouchviewheight;
    100,                                          //int     health;
    20,                                           //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    130,                                          //int     fov;
    0.0f,                                         //float   bob;
    60,                                           //int     steptime;
    1.3f,                                         //float   speed;
    5.0f,                                         //float   sticky;
    { PCL_D_O_LEV4, PCL_D_O_LEV3_UPG, PCL_NONE }, //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV3_UPG,                             //int     classnum;
    "OffensiveLevel3-Upgrade",                    //char    *classname;
    "orbb",                                       //char    *modelname;
    "default",                                    //char    *skinname;
    { -15, -15, -15 },                            //vec3_t  mins;
    { 15, 15, 15 },                               //vec3_t  maxs;
    { 15, 15, 15 },                               //vec3_t  crouchmaxs;
    { -15, -15, -4 },                             //vec3_t  deadmins;
    { 15, 15, 4 },                                //vec3_t  deadmaxs;
    4, 4,                                         //int     viewheight, crouchviewheight;
    100,                                          //int     health;
    20,                                           //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    130,                                          //int     fov;
    0.0f,                                         //float   bob;
    60,                                           //int     steptime;
    1.3f,                                         //float   speed;
    5.0f,                                         //float   sticky;
    { PCL_D_O_LEV4, PCL_NONE, PCL_NONE },         //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_D_O_LEV4,                                 //int     classnum;
    "OffensiveLevel4",                            //char    *classname;
    "xaero",                                      //char    *modelname;
    "default",                                    //char    *skinname;
    { -15, -15, -15 },                            //vec3_t  mins;
    { 15, 15, 15 },                               //vec3_t  maxs;
    { 15, 15, 15 },                               //vec3_t  crouchmaxs;
    { -15, -15, -4 },                             //vec3_t  deadmins;
    { 15, 15, 4 },                                //vec3_t  deadmaxs;
    4, 4,                                         //int     viewheight, crouchviewheight;
    100,                                          //int     health;
    30,                                           //int     armor;
    SCA_CANJUMP|SCA_NOWEAPONDRIFT|SCA_FOVWARPS,   //int     abilities;
    0.0f,                                         //float   buildDist;
    0.0f,                                         //float   launchSpeed;
    130,                                          //int     fov;
    0.0f,                                         //float   bob;
    60,                                           //int     steptime;
    1.3f,                                         //float   speed;
    5.0f,                                         //float   sticky;
    { PCL_NONE, PCL_NONE, PCL_NONE },             //int     children[ 3 ];
    3000,                                         //int     timetoevolve;
    100                                           //int     value;
  },
  {
    PCL_H_BASE,                                 //int     classnum;
    "Human",                                    //char    *classname;
    "sarge",                                    //char    *modelname;
    "default",                                  //char    *skinname;
    { -15, -15, -24 },                          //vec3_t  mins;
    { 15, 15, 32 },                             //vec3_t  maxs;
    { 15, 15, 16 },                             //vec3_t  crouchmaxs;
    { -15, -15, -4 },                           //vec3_t  deadmins;
    { 15, 15, 4 },                              //vec3_t  deadmaxs;
    26, 12,                                     //int     viewheight, crouchviewheight;
    100,                                        //int     health;
    0,                                          //int     armor;
    SCA_TAKESFALLDAMAGE|SCA_CANJUMP,            //int     abilities;
    80.0f,                                      //float   buildDist;
    0.0f,                                       //float   launchSpeed;
    90,                                         //int     fov;
    0.002f,                                     //float   bob;
    200,                                        //int     steptime;
    1.0f,                                       //float   speed;
    1.0f,                                       //float   sticky;
    { PCL_NONE, PCL_NONE, PCL_NONE },           //int     children[ 3 ];
    0,                                          //int     timetoevolve;
    0                                           //int     value;
  }
};

int   bg_numPclasses = sizeof( bg_classList ) / sizeof( bg_classList[ 0 ] );

/*
==============
BG_FindClassNumForName
==============
*/
int BG_FindClassNumForName( char *name )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( !Q_stricmp( bg_classList[ i ].className, name ) )
      return bg_classList[ i ].classNum;
  }

  //wimp out
  return PCL_NONE;
}

/*
==============
BG_FindNameForClassNum
==============
*/
char *BG_FindNameForClassNum( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
      return bg_classList[ i ].className;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindModelNameForClass
==============
*/
char *BG_FindModelNameForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
      return bg_classList[ i ].modelName;
  }

  //note: must return a valid modelName!
  return bg_classList[ 0 ].modelName;
}

/*
==============
BG_FindBBoxForClass
==============
*/
void BG_FindBBoxForClass( int pclass, vec3_t mins, vec3_t maxs, vec3_t cmaxs, vec3_t dmins, vec3_t dmaxs )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      if( mins != NULL )
        VectorCopy( bg_classList[ i ].mins,        mins );
        
      if( maxs != NULL )
        VectorCopy( bg_classList[ i ].maxs,        maxs );
        
      if( cmaxs != NULL )
        VectorCopy( bg_classList[ i ].crouchMaxs,  cmaxs );
        
      if( dmins != NULL )
        VectorCopy( bg_classList[ i ].deadMins,    dmins );
        
      if( dmaxs != NULL )
        VectorCopy( bg_classList[ i ].deadMaxs,    dmaxs );

      return;
    }
  }
  
  if( mins != NULL )
    VectorCopy( bg_classList[ 0 ].mins,        mins );
    
  if( maxs != NULL )
    VectorCopy( bg_classList[ 0 ].maxs,        maxs );
    
  if( cmaxs != NULL )
    VectorCopy( bg_classList[ 0 ].crouchMaxs,  cmaxs );
    
  if( dmins != NULL )
    VectorCopy( bg_classList[ 0 ].deadMins,    dmins );
    
  if( dmaxs != NULL )
    VectorCopy( bg_classList[ 0 ].deadMaxs,    dmaxs );
}

/*
==============
BG_FindViewheightForClass
==============
*/
void BG_FindViewheightForClass( int pclass, int *viewheight, int *cViewheight )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      if( viewheight != NULL )
        *viewheight = bg_classList[ i ].viewheight;
        
      if( cViewheight != NULL )
        *cViewheight = bg_classList[ i ].crouchViewheight;

      return;
    }
  }
  
  if( viewheight != NULL )
    *viewheight = bg_classList[ 0 ].viewheight;
    
  if( cViewheight != NULL )
    *cViewheight = bg_classList[ 0 ].crouchViewheight;
}

/*
==============
BG_FindHealthForClass
==============
*/
int BG_FindHealthForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].health;
    }
  }
  
  return 100;
}

/*
==============
BG_FindArmorForClass
==============
*/
int BG_FindArmorForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].armor;
    }
  }
  
  return 0;
}

/*
==============
BG_FindFovForClass
==============
*/
int BG_FindFovForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].fov;
    }
  }
  
  return 90;
}

/*
==============
BG_FindBobForClass
==============
*/
float BG_FindBobForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].bob;
    }
  }
  
  return 0.002;
}

/*
==============
BG_FindSpeedForClass
==============
*/
float BG_FindSpeedForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].speed;
    }
  }
  
  return 1.0;
}

/*
==============
BG_FindStickyForClass
==============
*/
float BG_FindStickyForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].sticky;
    }
  }
  
  return 1.0;
}

/*
==============
BG_FindSteptimeForClass
==============
*/
int BG_FindSteptimeForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].steptime;
    }
  }
  
  return 200;
}

/*
==============
BG_ClassHasAbility
==============
*/
qboolean BG_ClassHasAbility( int pclass, int ability )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return ( bg_classList[ i ].abilities & ability );
    }
  }

  //hack to get CANJUMP when a spectator
  if( ability == SCA_CANJUMP )
    return qtrue;
  else
    return qfalse;
}

/*
==============
BG_FindBuildDistForClass
==============
*/
float BG_FindBuildDistForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].buildDist;
    }
  }
  
  return 0.0f;
}

/*
==============
BG_FindLaunchSpeedForClass
==============
*/
float BG_FindLaunchSpeedForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].launchSpeed;
    }
  }
  
  return 0.0f;
}

/*
==============
BG_ClassCanEvolveFromTo
==============
*/
qboolean BG_ClassCanEvolveFromTo( int fclass, int tclass )
{
  int i, j;

  if( tclass == PCL_NONE )
    return qfalse;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == fclass )
    {
      for( j = 0; j <= 3; j++ )
        if( bg_classList[ i ].children[ j ] == tclass ) return qtrue;

      return qfalse; //may as well return by this point
    }
  }

  return qfalse;
}

/*
==============
BG_FindEvolveTimeForClass
==============
*/
int BG_FindEvolveTimeForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].timeToEvolve;
    }
  }
  
  return 5000;
}

/*
==============
BG_FindValueOfClass
==============
*/
int BG_FindValueOfClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numPclasses; i++ )
  {
    if( bg_classList[ i ].classNum == pclass )
    {
      return bg_classList[ i ].value;
    }
  }
  
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

weaponAttributes_t bg_weapons[ ] =
{
  {
    WP_MACHINEGUN,        //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "rifle",              //char      *weaponName;
    "Rifle",              //char      *weaponHumanName;
    30,                   //int       quan;
    3,                    //int       clips;
    3,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_FLAMER,            //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "flamer",             //char      *weaponName;
    "Flame Thrower",      //char      *weaponHumanName;
    400,                  //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_CHAINGUN,          //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "chaingun",           //char      *weaponName;
    "Chaingun",           //char      *weaponHumanName;
    300,                  //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_MASS_DRIVER,       //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "mdriver",            //char      *weaponName;
    "Mass Driver",        //char      *weaponHumanName;
    5,                    //int       quan;
    2,                    //int       clips;
    3,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_PULSE_RIFLE,       //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "prifle",             //char      *weaponName;
    "Pulse Rifle",        //char      *weaponHumanName;
    50,                   //int       quan;
    3,                    //int       clips;
    3,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_HBUILD,            //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "ckit",               //char      *weaponName;
    "Construction Kit",   //char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_HBUILD2,           //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "ackit",              //char      *weaponName;
    "Adv Construction Kit",//char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_DBUILD,            //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "dbuild",             //char      *weaponName;
    "Droid build weapon", //char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_DBUILD2,           //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "dbuild2",            //char      *weaponName;
    "Droid build weapon2",//char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_VENOM,             //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "venom",              //char      *weaponName;
    "Venom",              //char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  },
  {
    WP_GRABANDCSAW,       //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "grabandcsaw",        //char      *weaponName;
    "Circular Saw",       //char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qtrue,                //qboolean  hasAltMode;
    qtrue                 //qboolean  synced;
  },
  {
    WP_POUNCE,            //int       weaponNum;
    100,                  //int       price;
    SLOT_WEAPON,          //int       slots;
    "pounce",             //char      *weaponName;
    "Claw and pounce",    //char      *weaponHumanName;
    0,                    //int       quan;
    0,                    //int       clips;
    0,                    //int       maxClips;
    qfalse,               //qboolean  hasAltMode;
    qfalse                //qboolean  synced;
  }
};

int   bg_numWeapons = sizeof( bg_weapons ) / sizeof( bg_weapons[ 0 ] );

/*
==============
BG_FindPriceForWeapon
==============
*/
int BG_FindPriceForWeapon( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
    {
      return bg_weapons[ i ].price;
    }
  }
  
  return 100;
}

/*
==============
BG_FindSlotsForWeapon
==============
*/
int BG_FindSlotsForWeapon( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
    {
      return bg_weapons[ i ].slots;
    }
  }
  
  return SLOT_WEAPON;
}

/*
==============
BG_FindNameForWeapon
==============
*/
char *BG_FindNameForWeapon( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
      return bg_weapons[ i ].weaponName;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindWeaponNumForName
==============
*/
int BG_FindWeaponNumForName( char *name )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( !Q_stricmp( bg_weapons[ i ].weaponName, name ) )
      return bg_weapons[ i ].weaponNum;
  }

  //wimp out
  return WP_NONE;
}

/*
==============
BG_FindHumanNameForWeapon
==============
*/
char *BG_FindHumanNameForWeapon( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
      return bg_weapons[ i ].weaponHumanName;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindAmmoForWeapon
==============
*/
void BG_FindAmmoForWeapon( int weapon, int *quan, int *clips, int *maxClips )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
    {
      if( quan != NULL )
        *quan = bg_weapons[ i ].quan;
      if( clips != NULL )
        *clips = bg_weapons[ i ].clips;
      if( maxClips != NULL )
        *maxClips = bg_weapons[ i ].maxClips;

      //no need to keep going
      break;
    }
  }
}

/*
==============
BG_WeaponHasAltMode
==============
*/
qboolean BG_WeaponHasAltMode( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
    {
      return bg_weapons[ i ].hasAltMode;
    }
  }

  return qfalse;
}

/*
==============
BG_WeaponModesAreSynced
==============
*/
qboolean BG_WeaponModesAreSynced( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].weaponNum == weapon )
    {
      return bg_weapons[ i ].synced;
    }
  }

  return qfalse;
}

////////////////////////////////////////////////////////////////////////////////

upgradeAttributes_t bg_upgrades[ ] =
{
  {
    UP_TORCH,               //int   upgradeNum;
    100,                    //int   price;
    SLOT_NONE,              //int   slots;
    "torch",                //char  *upgradeName;
    "Torch"                 //char  *upgradeHumanName;
  },
  {
    UP_NVG,                 //int   upgradeNum;
    100,                    //int   price;
    SLOT_HEAD,              //int   slots;
    "nvg",                  //char  *upgradeName;
    "NVG"                   //char  *upgradeHumanName;
  },
  {
    UP_CHESTARMOUR,         //int   upgradeNum;
    100,                    //int   price;
    SLOT_TORSO,             //int   slots;
    "carmour",              //char  *upgradeName;
    "Chest Armour"          //char  *upgradeHumanName;
  },
  {
    UP_LIMBARMOUR,          //int   upgradeNum;        
    100,                    //int   price;
    SLOT_ARMS|SLOT_LEGS,    //int   slots;
    "larmour",              //char  *upgradeName;
    "Limb Armour"           //char  *upgradeHumanName;
  },
  {
    UP_HELMET,              //int   upgradeNum;
    100,                    //int   price;
    SLOT_HEAD,              //int   slots;
    "helmet",               //char  *upgradeName;
    "Helmet"                //char  *upgradeHumanName;
  },
  {
    UP_ANTITOXIN,           //int   upgradeNum;
    100,                    //int   price;
    SLOT_NONE,              //int   slots;
    "atoxin",               //char  *upgradeName;
    "Anti-toxin"            //char  *upgradeHumanName;
  },
  {
    UP_BATTPACK,            //int   upgradeNum;
    100,                    //int   price;
    SLOT_BACKPACK,          //int   slots;
    "battpack",             //char  *upgradeName;
    "Battery Pack"          //char  *upgradeHumanName;
  },
  {
    UP_JETPACK,             //int   upgradeNum;
    100,                    //int   price;
    SLOT_BACKPACK,          //int   slots;
    "jetpack",              //char  *upgradeName;
    "Jet Pack"              //char  *upgradeHumanName;
  },
  {
    UP_THREATHELMET,        //int   upgradeNum;
    100,                    //int   price;
    SLOT_HEAD,              //int   slots;
    "thelmet",              //char  *upgradeName;
    "Threat Helmet"         //char  *upgradeHumanName;
  },
  {
    UP_BATTLESUIT,                            //int   upgradeNum;
    100,                                      //int   price;
    SLOT_HEAD|SLOT_TORSO|SLOT_ARMS|SLOT_LEGS, //int   slots;
    "bsuit",                                  //char  *upgradeName;
    "Battlesuit"                              //char  *upgradeHumanName;
  },
  {
    UP_IMPANTKIT,                             //int   upgradeNum;
    100,                                      //int   price;
    SLOT_HEAD|SLOT_TORSO|SLOT_ARMS|SLOT_LEGS, //int   slots;
    "ikit",                                   //char  *upgradeName;
    "Implant Kit"                             //char  *upgradeHumanName;
  }
};

int   bg_numUpgrades = sizeof( bg_upgrades ) / sizeof( bg_upgrades[ 0 ] );

/*
==============
BG_FindPriceForUpgrade
==============
*/
int BG_FindPriceForUpgrade( int upgrade )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( bg_upgrades[ i ].upgradeNum == upgrade )
    {
      return bg_upgrades[ i ].price;
    }
  }
  
  return 100;
}

/*
==============
BG_FindSlotsForUpgrade
==============
*/
int BG_FindSlotsForUpgrade( int upgrade )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( bg_upgrades[ i ].upgradeNum == upgrade )
    {
      return bg_upgrades[ i ].slots;
    }
  }
  
  return SLOT_NONE;
}

/*
==============
BG_FindNameForUpgrade
==============
*/
char *BG_FindNameForUpgrade( int upgrade )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( bg_upgrades[ i ].upgradeNum == upgrade )
      return bg_upgrades[ i ].upgradeName;
  }

  //wimp out
  return "";
}

/*
==============
BG_FindUpgradeNumForName
==============
*/
int BG_FindUpgradeNumForName( char *name )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( !Q_stricmp( bg_upgrades[ i ].upgradeName, name ) )
      return bg_upgrades[ i ].upgradeNum;
  }

  //wimp out
  return UP_NONE;
}

/*
==============
BG_FindHumanNameForUpgrade
==============
*/
char *BG_FindHumanNameForUpgrade( int upgrade )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( bg_upgrades[ i ].upgradeNum == upgrade )
      return bg_upgrades[ i ].upgradeHumanName;
  }

  //wimp out
  return "";
}

////////////////////////////////////////////////////////////////////////////////

/*
==============
BG_FindItemForPowerup
==============
*/
gitem_t *BG_FindItemForPowerup( powerup_t pw ) {
  int   i;

  for ( i = 0 ; i < bg_numItems ; i++ ) {
    if ( (bg_itemlist[i].giType == IT_POWERUP /*||
        bg_itemlist[i].giType == IT_TEAM ||
        bg_itemlist[i].giType == IT_PERSISTANT_POWERUP*/) &&
      bg_itemlist[i].giTag == pw ) {
      return &bg_itemlist[i];
    }
  }

  return NULL;
}


/*
==============
BG_FindItemForHoldable
==============
*/
gitem_t *BG_FindItemForHoldable( holdable_t pw ) {
  int   i;

  for ( i = 0 ; i < bg_numItems ; i++ ) {
    if ( bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw ) {
      return &bg_itemlist[i];
    }
  }

  Com_Error( ERR_DROP, "HoldableItem not found" );

  return NULL;
}


/*
===============
BG_FindItemForWeapon

===============
*/
gitem_t *BG_FindItemForWeapon( weapon_t weapon ) {
  gitem_t *it;

  for ( it = bg_itemlist + 1 ; it->classname ; it++) {
    if ( it->giType == IT_WEAPON && it->giTag == weapon ) {
      return it;
    }
  }

  Com_Error( ERR_DROP, "Couldn't find item for weapon %i", weapon);
  return NULL;
}


/*
===============
BG_FindItemForBuildable

TA: new function for finding buildable items
===============
*/
gitem_t *BG_FindItemForBuildable( buildable_t buildable ) {
  gitem_t *it;

  for ( it = bg_itemlist + 1 ; it->classname ; it++) {
    if ( it->giType == IT_BUILDABLE && it->giTag == buildable ) {
      return it;
    }
  }

  Com_Error( ERR_DROP, "Couldn't find item for buildable %i", buildable);
  return NULL;
}


/*
===============
BG_FindItemForUpgrade

TA: new function for finding upgrade items
===============
*/
gitem_t *BG_FindItemForUpgrade( upgrade_t upgrade ) {
  gitem_t *it;

  for ( it = bg_itemlist + 1 ; it->classname ; it++) {
    if ( it->giType == IT_UPGRADE && it->giTag == upgrade ) {
      return it;
    }
  }

  Com_Error( ERR_DROP, "Couldn't find item for upgrade %i", upgrade);
  return NULL;
}


/*
===============
BG_FindItem

===============
*/
gitem_t *BG_FindItem( const char *pickupName ) {
  gitem_t *it;

  for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
    if ( !Q_stricmp( it->pickup_name, pickupName ) )
      return it;
  }

  return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean  BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
  vec3_t    origin;

  BG_EvaluateTrajectory( &item->pos, atTime, origin );

  // we are ignoring ducked differences here
  if ( ps->origin[0] - origin[0] > 44
    || ps->origin[0] - origin[0] < -50
    || ps->origin[1] - origin[1] > 36
    || ps->origin[1] - origin[1] < -36
    || ps->origin[2] - origin[2] > 36
    || ps->origin[2] - origin[2] < -36 ) {
    return qfalse;
  }

  return qtrue;
}

/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean  BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps ) {
  gitem_t *item;

  if ( ent->modelindex < 1 || ent->modelindex >= bg_numItems ) {
    Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
  }

  item = &bg_itemlist[ent->modelindex];

  switch( item->giType ) {
  case IT_WEAPON:
    return qtrue; // weapons are always picked up

  case IT_AMMO:
    if ( ps->ammo[ item->giTag ] >= 200 ) {
      return qfalse;    // can't hold any more
    }
    return qtrue;

  case IT_ARMOR:
    if ( ps->stats[STAT_ARMOR] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
      return qfalse;
    }
    return qtrue;

  case IT_HEALTH:
    // small and mega healths will go over the max, otherwise
    // don't pick up if already at max
    if ( item->quantity == 5 || item->quantity == 100 ) {
      if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
        return qfalse;
      }
      return qtrue;
    }

    if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] ) {
      return qfalse;
    }
    return qtrue;

  case IT_POWERUP:
    return qtrue; // powerups are always picked up

  case IT_TEAM: // team items, such as flags
    // ent->modelindex2 is non-zero on items if they are dropped
    // we need to know this because we can pick up our dropped flag (and return it)
    // but we can't pick up our flag at base
    if (ps->persistant[PERS_TEAM] == TEAM_HUMANS) {
      //TA: remove powerups
      /*if (item->giTag == PW_BLUEFLAG ||
        (item->giTag == PW_REDFLAG && ent->modelindex2) ||
        (item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]))
        return qtrue;*/
    } else if (ps->persistant[PERS_TEAM] == TEAM_DROIDS) {
      /*if (item->giTag == PW_REDFLAG ||
        (item->giTag == PW_BLUEFLAG && ent->modelindex2) ||
        (item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]))
        return qtrue;*/
    }
    return qfalse;

  //TA: not using the q3 holdable items code
  /*case IT_HOLDABLE:
    // can only hold one item at a time
    if ( ps->stats[STAT_HOLDABLE_ITEM] ) {
      return qfalse;
    }
    return qtrue;*/

  case IT_BAD:
    Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );
    
  default:
#ifndef Q3_VM
#ifndef NDEBUG // bk0001204
    Com_Printf("BG_CanItemBeGrabbed: unknown enum %d\n", item->giType );
#endif
#endif
    break;
  }

  return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result ) {
  float   deltaTime;
  float   phase;

  switch( tr->trType ) {
  case TR_STATIONARY:
  case TR_INTERPOLATE:
    VectorCopy( tr->trBase, result );
    break;
  case TR_LINEAR:
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
    break;
  case TR_SINE:
    deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
    phase = sin( deltaTime * M_PI * 2 );
    VectorMA( tr->trBase, phase, tr->trDelta, result );
    break;
  case TR_LINEAR_STOP:
    if ( atTime > tr->trTime + tr->trDuration ) {
      atTime = tr->trTime + tr->trDuration;
    }
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    if ( deltaTime < 0 ) {
      deltaTime = 0;
    }
    VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
    break;
  case TR_GRAVITY:
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
    result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
    break;
  case TR_BUOYANCY:
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
    result[2] += 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
    break;
  default:
    Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
    break;
  }
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result ) {
  float deltaTime;
  float phase;

  switch( tr->trType ) {
  case TR_STATIONARY:
  case TR_INTERPOLATE:
    VectorClear( result );
    break;
  case TR_LINEAR:
    VectorCopy( tr->trDelta, result );
    break;
  case TR_SINE:
    deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
    phase = cos( deltaTime * M_PI * 2 );  // derivative of sin = cos
    phase *= 0.5;
    VectorScale( tr->trDelta, phase, result );
    break;
  case TR_LINEAR_STOP:
    if ( atTime > tr->trTime + tr->trDuration ) {
      VectorClear( result );
      return;
    }
    VectorCopy( tr->trDelta, result );
    break;
  case TR_GRAVITY:
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    VectorCopy( tr->trDelta, result );
    result[2] -= DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
    break;
  case TR_BUOYANCY:
    deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
    VectorCopy( tr->trDelta, result );
    result[2] += DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
    break;
  default:
    Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
    break;
  }
}

char *eventnames[] = {
  "EV_NONE",

  "EV_FOOTSTEP",
  "EV_FOOTSTEP_METAL",
  "EV_FOOTSTEP_SQUELCH",
  "EV_FOOTSPLASH",
  "EV_FOOTWADE",
  "EV_SWIM",

  "EV_STEP_4",
  "EV_STEP_8",
  "EV_STEP_12",
  "EV_STEP_16",

  "EV_FALL_SHORT",
  "EV_FALL_MEDIUM",
  "EV_FALL_FAR",

  "EV_JUMP_PAD",      // boing sound at origin, jump sound on player

  "EV_JUMP",
  "EV_WATER_TOUCH", // foot touches
  "EV_WATER_LEAVE", // foot leaves
  "EV_WATER_UNDER", // head touches
  "EV_WATER_CLEAR", // head leaves

  "EV_ITEM_PICKUP",     // normal item pickups are predictable
  "EV_GLOBAL_ITEM_PICKUP",  // powerup / team sounds are broadcast to everyone

  "EV_NOAMMO",
  "EV_CHANGE_WEAPON",
  "EV_FIRE_WEAPON",
  "EV_FIRE_WEAPON2",
  "EV_FIRE_WEAPONBOTH",

  "EV_USE_ITEM0",
  "EV_USE_ITEM1",
  "EV_USE_ITEM2",
  "EV_USE_ITEM3",
  "EV_USE_ITEM4",
  "EV_USE_ITEM5",
  "EV_USE_ITEM6",
  "EV_USE_ITEM7",
  "EV_USE_ITEM8",
  "EV_USE_ITEM9",
  "EV_USE_ITEM10",
  "EV_USE_ITEM11",
  "EV_USE_ITEM12",
  "EV_USE_ITEM13",
  "EV_USE_ITEM14",
  "EV_USE_ITEM15",

  "EV_ITEM_RESPAWN",
  "EV_PLAYER_RESPAWN", //TA: for fovwarp effects
  "EV_ITEM_POP",
  "EV_PLAYER_TELEPORT_IN",
  "EV_PLAYER_TELEPORT_OUT",

  "EV_GRENADE_BOUNCE",    // eventParm will be the soundindex

  "EV_GENERAL_SOUND",
  "EV_GLOBAL_SOUND",    // no attenuation

  "EV_BULLET_HIT_FLESH",
  "EV_BULLET_HIT_WALL",

  "EV_MISSILE_HIT",
  "EV_MISSILE_MISS",
  "EV_MISSILE_MISS_METAL",
  "EV_ITEM_EXPLOSION", //TA: human item explosions
  "EV_RAILTRAIL",
  "EV_SHOTGUN",
  "EV_BULLET",        // otherEntity is the shooter

  "EV_PAIN",
  "EV_DEATH1",
  "EV_DEATH2",
  "EV_DEATH3",
  "EV_OBITUARY",

  "EV_POWERUP_QUAD",
  "EV_POWERUP_BATTLESUIT",
  "EV_POWERUP_REGEN",

  "EV_GIB_PLAYER",      // gib a previously living player
  "EV_GIB_DROID",       //TA: generic green gib for droids

  "EV_BUILD_CONSTRUCT", //TA
  "EV_BUILD_DESTROY",   //TA
  "EV_BUILD_ANIM",      //TA

  "EV_DEBUG_LINE",
  "EV_STOPLOOPINGSOUND",
  "EV_TAUNT",
  "EV_TAUNT_YES",
  "EV_TAUNT_NO",
  "EV_TAUNT_FOLLOWME",
  "EV_TAUNT_GETFLAG",
  "EV_TAUNT_GUARDBASE",
  "EV_TAUNT_PATROL",

  "EV_MENU"             //TA: menu event
};

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void  trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {
#ifdef _DEBUG
  {
    char buf[256];
    trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));
    if ( atof(buf) != 0 ) {
#ifdef QAGAME
      Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
      Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
    }
  }
#endif
  ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
  ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
  ps->eventSequence++;
}


/*
========================
BG_TouchJumpPad
========================
*/
void BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad ) {
  vec3_t  angles;
  float p;
  int effectNum;

  // spectators don't use jump pads
  if ( ps->pm_type != PM_NORMAL ) {
    return;
  }

  // flying characters don't hit bounce pads
  if ( ps->powerups[PW_FLIGHT] ) {
    return;
  }

  // if we didn't hit this same jumppad the previous frame
  // then don't play the event sound again if we are in a fat trigger
  if ( ps->jumppad_ent != jumppad->number ) {

    vectoangles( jumppad->origin2, angles);
    p = fabs( AngleNormalize180( angles[PITCH] ) );
    if( p < 45 ) {
      effectNum = 0;
    } else {
      effectNum = 1;
    }
    BG_AddPredictableEventToPlayerstate( EV_JUMP_PAD, effectNum, ps );
  }
  // remember hitting this jumppad this frame
  ps->jumppad_ent = jumppad->number;
  ps->jumppad_frame = ps->pmove_framecount;
  // give the player the velocity from the jumppad
  VectorCopy( jumppad->origin2, ps->velocity );
}


/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
  int   i;
  vec3_t  ceilingNormal = { 0, 0, -1 };

  if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE ) {
    s->eType = ET_INVISIBLE;
  } else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
    s->eType = ET_INVISIBLE;
  } else {
    s->eType = ET_PLAYER;
  }

  s->number = ps->clientNum;

  s->pos.trType = TR_INTERPOLATE;
  VectorCopy( ps->origin, s->pos.trBase );
  if ( snap ) {
    SnapVector( s->pos.trBase );
  }
  //set the trDelta for flag direction
  VectorCopy( ps->velocity, s->pos.trDelta );

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );
  if ( snap ) {
    SnapVector( s->apos.trBase );
  }

  //TA: i need for other things :)
  //s->angles2[YAW] = ps->movementDir;
  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;
  if ( ps->stats[STAT_HEALTH] <= 0 ) {
    s->eFlags |= EF_DEAD;
  } else {
    s->eFlags &= ~EF_DEAD;
  }

  if ( ps->externalEvent ) {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  } else if ( ps->entityEventSequence < ps->eventSequence ) {
    int   seq;

    if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
    }
    seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  /*s->powerups = 0;
  for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
    if ( ps->powerups[ i ] ) {
      s->powerups |= 1 << i;
    }
  }*/

  //TA: use powerups field to store team/class info:
  s->powerups = ps->stats[ STAT_PTEAM ] | ( ps->stats[ STAT_PCLASS ] << 8 );

  //TA: have to get the surfNormal thru somehow...
  if( ps->stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING )
    VectorCopy( ceilingNormal, s->angles2 );
  else
    VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;
}


/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap ) {
  int   i;
  vec3_t  ceilingNormal = { 0, 0, -1 };

  if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE ) {
    s->eType = ET_INVISIBLE;
  } else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
    s->eType = ET_INVISIBLE;
  } else {
    s->eType = ET_PLAYER;
  }

  s->number = ps->clientNum;

  s->pos.trType = TR_LINEAR_STOP;
  VectorCopy( ps->origin, s->pos.trBase );
  if ( snap ) {
    SnapVector( s->pos.trBase );
  }
  // set the trDelta for flag direction and linear prediction
  VectorCopy( ps->velocity, s->pos.trDelta );
  // set the time for linear prediction
  s->pos.trTime = time;
  // set maximum extra polation time
  s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );
  if ( snap ) {
    SnapVector( s->apos.trBase );
  }

  //TA: i need for other things :)
  //s->angles2[YAW] = ps->movementDir;
  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;
  if ( ps->stats[STAT_HEALTH] <= 0 ) {
    s->eFlags |= EF_DEAD;
  } else {
    s->eFlags &= ~EF_DEAD;
  }

  if ( ps->externalEvent ) {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  } else if ( ps->entityEventSequence < ps->eventSequence ) {
    int   seq;

    if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
    }
    seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  /*s->powerups = 0;
  for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
    if ( ps->powerups[ i ] ) {
      s->powerups |= 1 << i;
    }
  }*/

  //TA: use powerups field to store team/class info:
  s->powerups = ps->stats[ STAT_PTEAM ] | ( ps->stats[ STAT_PCLASS ] << 8 );

  //TA: have to get the surfNormal thru somehow...
  if( ps->stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING )
    VectorCopy( ceilingNormal, s->angles2 );
  else
    VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;
}


//TA: extract the ammo quantity from the array
void BG_unpackAmmoArray( int weapon, int ammo[ ], int ammo2[ ], int *quan, int *clips, int *maxclips )
{
  int   ammoarray[32];
  int   i;
  
  for( i = 0; i <= 15; i++ )
    ammoarray[ i ] = ammo[ i ];

  for( i = 16; i <= 31; i++ )
    ammoarray[ i ] = ammo2[ i - 16 ];

  if( quan != NULL )
    *quan = ammoarray[ weapon ] & 0x03FF;

  if( clips != NULL )
    *clips = ( ammoarray[ weapon ] >> 10 ) & 0x07;

  if( maxclips != NULL )
    *maxclips = ( ammoarray[ weapon ] >> 13 ) & 0x07;
}

//TA: pack the ammo quantity into the array
void BG_packAmmoArray( int weapon, int ammo[ ], int ammo2[ ], int quan, int clips, int maxclips )
{
  int   weaponvalue;

  weaponvalue = quan | ( clips << 10 ) | ( maxclips << 13 );

  if( weapon <= 15 )
    ammo[ weapon ] = weaponvalue;
  else if( weapon >= 16 )
    ammo2[ weapon - 16 ] = weaponvalue;
}

//TA: check whether infinite ammo
qboolean BG_infiniteAmmo( int weapon )
{
  switch( weapon )
  {
    case WP_VENOM:
    case WP_GRABANDCSAW:
    case WP_POUNCE:
    case WP_DBUILD:
    case WP_DBUILD2:
    case WP_HBUILD:
    case WP_HBUILD2:
      return qtrue;
      break;

    //nothing else has infinite ammo
    default:
      return qfalse;
      break;
  }
}

//TA: pack weapons into the array
void BG_packWeapon( int weapon, int stats[ ] )
{
  int  weaponList;

  weaponList = ( stats[ STAT_WEAPONS ] & 0x0000FFFF ) | ( ( stats[ STAT_WEAPONS2 ] << 16 ) & 0xFFFF0000 );

  weaponList |= ( 1 << weapon );
  
  stats[ STAT_WEAPONS ] = weaponList & 0x0000FFFF;
  stats[ STAT_WEAPONS2 ] = ( weaponList & 0xFFFF0000 ) >> 16;

  if( stats[ STAT_SLOTS ] & BG_FindSlotsForWeapon( weapon ) )
    Com_Printf( S_COLOR_YELLOW "WARNING: Held items conflict with weapon %d\n", weapon );

  stats[ STAT_SLOTS ] |= BG_FindSlotsForWeapon( weapon );
}

//TA: remove weapons from the array
void BG_removeWeapon( int weapon, int stats[ ] )
{
  int  weaponList;

  weaponList = ( stats[ STAT_WEAPONS ] & 0x0000FFFF ) | ( ( stats[ STAT_WEAPONS2 ] << 16 ) & 0xFFFF0000 );

  weaponList &= ~( 1 << weapon );
  
  stats[ STAT_WEAPONS ] = weaponList & 0x0000FFFF;
  stats[ STAT_WEAPONS2 ] = ( weaponList & 0xFFFF0000 ) >> 16;
  
  stats[ STAT_SLOTS ] &= ~BG_FindSlotsForWeapon( weapon );
}

//TA: check whether array contains weapon
qboolean BG_gotWeapon( int weapon, int stats[ ] )
{
  int  weaponList;

  weaponList = ( stats[ STAT_WEAPONS ] & 0x0000FFFF ) | ( ( stats[ STAT_WEAPONS2 ] << 16 ) & 0xFFFF0000 );

  return( weaponList & ( 1 << weapon ) );
}

//TA: pack items into array
void BG_packItem( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] |= ( 1 << item );

  if( stats[ STAT_SLOTS ] & BG_FindSlotsForUpgrade( item ) )
    Com_Printf( S_COLOR_YELLOW "WARNING: Held items conflict with upgrade %d\n", item );
    
  stats[ STAT_SLOTS ] |= BG_FindSlotsForUpgrade( item );
}

//TA: remove items from array
void BG_removeItem( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] &= ~( 1 << item );

  stats[ STAT_SLOTS ] &= ~BG_FindSlotsForUpgrade( item );
}

//TA: check if item is in array
qboolean BG_gotItem( int item, int stats[ ] )
{
  return( stats[ STAT_ITEMS ] & ( 1 << item ) );
}

//TA: set item active in array
void BG_activateItem( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] |= ( 1 << item );
}

//TA: set item deactive in array
void BG_deactivateItem( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] &= ~( 1 << item );
}

//TA: check if item active in array
qboolean BG_activated( int item, int stats[ ] )
{
  return( stats[ STAT_ACTIVEITEMS ] & ( 1 << item ) );
}

