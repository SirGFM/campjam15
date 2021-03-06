/**
 * @file include/campjam15/gameCtx.h
 * 
 * Defines this game's context
 */
#ifndef __GAMECTX_H__
#define __GAMECTX_H__

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSpriteset.h>
#include <GFraMe/gfmText.h>
#include <GFraMe/gfmTilemap.h>
#include <GFraMe/gfmTypes.h>

/** Define custom types */
#define COLLIDEABLE gfmType_reserved_2
#define BUTTON      gfmType_reserved_3
#define PLAYER      gfmType_reserved_4
#define DOC         gfmType_reserved_5
#define BULLET      gfmType_reserved_6
/** 'Export' all types */
extern char *pDictNames[];
extern int pDictTypes[];
extern int dictLen;

/** Enumeration of states */
enum enGameState {
    state_intro = 0,
    state_reset,
    state_max
};
typedef enum enGameState gameState;

/** Keep all common game objects in a single struct */
struct stGameCommon {
    /** The game's quadtree */
    gfmQuadtreeRoot *pQt;
    /** Global text */
    gfmText *pText;
    /** The game's default background */
    gfmTilemap *pTMap;
};
typedef struct stGameCommon gameCommon;

/** The game's context */
struct stGameCtx {
    /** Stuff common to all states */
    gameCommon common;
    /** Store the current state */
    gameState state;
    /** The library context */
    gfmCtx *pCtx;
    /** 8x8 spriteset */
    gfmSpriteset *pSset8x8;
    /** 16x16 spriteset */
    gfmSpriteset *pSset16x16;
    /** 32x32 spriteset */
    gfmSpriteset *pSset32x32;
    /** Flag to, programatically, force the game to quit; Differs from the
     * library's flag, since that one only says when the 'exit' button was
     * pressed */
    int isRunning;
    /** Handle of the texture managed by the library */
    int iTex;
    /** Handle for the 'action' key */
    int p1ActionHnd;
    /** Handle for the 'jump' key */
    int p1JumpHnd;
    /** Handle for the 'left' key */
    int p1LeftHnd;
    /** Handle for the 'right' key */
    int p1RightHnd;
    /** Handle for the 'action' key */
    int p2ActionHnd;
    /** Handle for the 'jump' key */
    int p2JumpHnd;
    /** Handle for the 'left' key */
    int p2LeftHnd;
    /** Handle for the 'right' key */
    int p2RightHnd;
    int resetHnd;
    /** Audio handles */
    int song;
    int barrier;
    int death;
    int jump2;
    int jump1;
    int reflect;
    int shoot;
    int hit;
    /** Current state's context; It's set to NULL before changing the state */
    void *pState;
};
typedef struct stGameCtx gameCtx;

#endif /* __GAMECTX_H__*/

