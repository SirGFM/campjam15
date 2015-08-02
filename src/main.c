/**
 * @file src/main.c
 * 
 * The game's entry point; Loads all assets and then manage switching state and
 * stuff
 */
#include <campjam15/gameCtx.h>
#include <campjam15/state_intro.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmInput.h>
#include <GFraMe/gfmSpriteset.h>

#include <string.h>

/** The game virtual window's width */
#define VWIDTH     320
/** The game virtual window's height */
#define VHEIGHT    240
/** The game default window's width */
#define WND_WIDTH  640
/** The game default window's height */
#define WND_HEIGHT 480
/** The game's main (and only) texture */
#define TEXTURE    "atlas.bmp"
/** Transparent color */
#define COLOR_KEY  0xff00ff
/** Base FPS */
#define FPS        60

/** Keep all tile types in a single place */
char *pDictNames[] = {
    "collideable",
    "button"
};
int pDictTypes[] = {
    COLLIDEABLE,
    BUTTON
};
int dictLen = sizeof(pDictTypes) / sizeof(int);

/**
 * Loads all game's assets
 * 
 * @param  pGame The game context
 * @return       GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
static gfmRV loadAssets(gameCtx *pGame) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    
    // Load the texture
    rv = gfm_loadTextureStatic(&(pGame->iTex), pGame->pCtx, TEXTURE, COLOR_KEY);
    ASSERT_NR(rv == GFMRV_OK);
    // Set it as the default
    rv = gfm_setDefaultTexture(pGame->pCtx, pGame->iTex);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Create the spritesets
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, pGame->iTex,
            8/*tw*/, 8/*th*/);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_createSpritesetCached(&(pGame->pSset16x16), pGame->pCtx,
            pGame->iTex, 16/*tw*/, 16/*th*/);
    rv = gfm_createSpritesetCached(&(pGame->pSset32x32), pGame->pCtx,
            pGame->iTex, 32/*tw*/, 32/*th*/);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Load the song (only in RELEASE)
#ifndef DEBUG
    rv = gfm_loadAudio(&(pGame->song), pGame->pCtx, "song.wav", 8);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_setRepeat(pGame->pCtx, pGame->song, 0);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = gfm_loadAudio(&(pGame->jump2), pGame->pCtx, "jump2.wav", 9);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->barrier), pGame->pCtx, "barrier.wav", 11);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->death), pGame->pCtx, "death.wav", 9);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->jump1), pGame->pCtx, "jump1.wav", 9);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->reflect), pGame->pCtx, "reflect.wav", 11);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->shoot), pGame->pCtx, "shoot.wav", 9);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_loadAudio(&(pGame->hit), pGame->pCtx, "hit.wav", 7);
    ASSERT_NR(rv == GFMRV_OK);
#endif
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

int main(int argc, char *argv[]) {
    gameCtx game;
    gfmRV rv;
    
    // Clean the game's context
    memset(&game, 0x0, sizeof(gameCtx));
    
    // Initialize the library
    rv = gfm_getNew(&(game.pCtx));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_init(game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Set the game's title
    rv = gfm_setTitleStatic(game.pCtx, "com.gfmgamecorner", "DinoPanic");
    ASSERT_NR(rv == GFMRV_OK);
    
    // Initialize the window (last param is whether the user can resize the wnd)
    rv = gfm_initGameWindow(game.pCtx, VWIDTH, VHEIGHT, WND_WIDTH, WND_HEIGHT,
            0/*userCantResize*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Set the background color
    rv = gfm_setBackground(game.pCtx, 0xff222034);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Initialize the audio sub-system
#ifndef DEBUG
    rv = gfm_initAudio(game.pCtx, gfmAudio_defQuality);
    ASSERT_NR(rv == GFMRV_OK);
#endif
    
    if (argc > 1 && strcmp(argv[1], "full") == 0) {
        rv = gfm_setFullscreen(game.pCtx);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    // Load all assets
    // TODO push this into another thread and play an animation
    rv = loadAssets(&game);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Add all virtual keys
    rv = gfm_addVirtualKey(&(game.p1ActionHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p1JumpHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p1LeftHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p1RightHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p2ActionHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p2JumpHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p2LeftHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_addVirtualKey(&(game.p2RightHnd), game.pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Bind all inputs
    rv = gfm_bindInput(game.pCtx, game.p1ActionHnd, gfmKey_space);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p1JumpHnd, gfmKey_w);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p1LeftHnd, gfmKey_a);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p1RightHnd, gfmKey_d);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p2ActionHnd, gfmKey_n0);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p2JumpHnd, gfmKey_up);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p2LeftHnd, gfmKey_left);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_bindInput(game.pCtx, game.p2RightHnd, gfmKey_right);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Alloc common stuff
    rv = gfmQuadtree_getNew(&(game.common.pQt));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_getNew(&(game.common.pTMap));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmText_getNew(&(game.common.pText));
    ASSERT_NR(rv == GFMRV_OK);
    
#ifndef DEBUG
    rv = gfm_playAudio(0, game.pCtx, game.song, 0.8);
    ASSERT_NR(rv == GFMRV_OK);
#endif
    
    // Initalize the FPS counter (will only work on DEBUG mode)
    rv = gfm_initFPSCounter(game.pCtx, game.pSset8x8, 0/*firstTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Set the main loop framerate
    rv = gfm_setStateFrameRate(game.pCtx, FPS, FPS);
    ASSERT_NR(rv == GFMRV_OK);
    // Initialize the timer
    rv = gfm_setFPS(game.pCtx, FPS);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Set the game as running and go
    game.isRunning = 1;
    game.state = state_intro;
    while (game.isRunning && gfm_didGetQuitFlag(game.pCtx) == GFMRV_FALSE) {
        // Make sure this is cleaned when we enter the next state
        game.pState = 0;
        switch (game.state) {
            case state_intro: rv = intro(&game); break;
            default: rv = GFMRV_INTERNAL_ERROR;
        }
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    rv = GFMRV_OK;
__ret:
    // Release all common stuff (they are guaranteed to be NULL if not
    // initialized)
    gfmQuadtree_free(&(game.common.pQt));
    gfmTilemap_free(&(game.common.pTMap));
    gfmText_free(&(game.common.pText));
    // Make sure everything is cleaned, even on error
    gfm_free(&(game.pCtx));
    
    return rv;
}

