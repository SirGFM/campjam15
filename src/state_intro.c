/**
 * @file src/state_intro.c
 * 
 * Intro state; Plays a little animation and then start the battle againts the
 * doc
 */
#include <campjam15/gameCtx.h>

#include <GFraMe/gfmTilemap.h>

#include <string.h>

/** Array with the tilemap animation */
static int pMachineFxAnim[] = {
/* len,fps,loop,frames...*/
    4 , 16,  1 ,128,129,130,131,
    0
};

/** This state's context */
struct stIntroCtx {
    /** Effect on top of the machine */
    gfmTilemap *pMachineFx;
};
typedef struct stIntroCtx introCtx;

/**
 * Initializes (and loads) everything used by this state
 * 
 * @param  pGame The game's context
 * @return       GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro_init(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // Alloc the fx tilemap
    rv = gfmTilemap_getNew(&(pIntro->pMachineFx));
    ASSERT_NR(rv == GFMRV_OK);
    
    // Initialize the background
    rv = gfmTilemap_init(pGame->common.pTMap, pGame->pSset8x8, 1/*width*/,
        1/*height*/, -1/*defTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Load the background from a file
    rv = gfmTilemap_loadf(pGame->common.pTMap, pGame->pCtx, "lab.gfm",
            7/*filenameLen*/, pDictNames, pDictTypes, dictLen);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Initialize and load the fx
    rv = gfmTilemap_init(pIntro->pMachineFx, pGame->pSset8x8, 5/*width*/,
        4/*height*/, -1/*defTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_loadf(pIntro->pMachineFx, pGame->pCtx, "machine-fx.gfm",
            14/*filenameLen*/, pDictNames, pDictTypes, dictLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Set its position
    rv = gfmTilemap_setPosition(pIntro->pMachineFx, 24/*x*/, 160/*y*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Add the tilemap animations
    rv = gfmTilemap_addAnimations(pIntro->pMachineFx, pMachineFxAnim,
            sizeof(pMachineFxAnim) / sizeof(int) - 1);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_recacheAnimations(pIntro->pMachineFx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Update the state
 * 
 * @param  pGame The game's context
 * @return       GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro_update(gameCtx *pGame) {
    gfmRV rv;
    int height, width;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // TODO Update everything
    rv = gfmTilemap_update(pIntro->pMachineFx, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // TODO Collide everything
    rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmQuadtree_initRoot(pGame->common.pQt, -2/*x*/, -2/*y*/, width + 4,
        height + 4, 4/*maxDepth*/, 6/*maxNodes*/);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmQuadtree_populateTilemap(pGame->common.pQt, pGame->common.pTMap);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draw the state
 * 
 * @param  pGame The game's context
 * @return       GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro_draw(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // TODO Draw everything
    rv = gfmTilemap_draw(pGame->common.pTMap, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_draw(pIntro->pMachineFx, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    //rv = gfmQuadtree_drawBounds(pGame->common.pQt, pGame->pCtx, 0/*colors*/);
    //ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Clean all data
 * 
 * @param  pGame The game's context
 * @return       GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro_clean(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // Free the tilemap
    rv = gfmTilemap_free(&(pIntro->pMachineFx));
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * The game's first state
 * 
 * @param  gameCtx The game's context
 * @return         GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro(gameCtx *pGame) {
    gfmRV rv;
    introCtx _intro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    
    // Clean the state's context (the stack may have "trash")
    memset(&_intro, 0x0, sizeof(introCtx));
    // Set the current state date
    pGame->pState = &_intro;
    
    // Initialize this state
    rv = intro_init(pGame);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Run until the window is closed (or the state changes)
    while (pGame->state == state_intro && pGame->isRunning
            && gfm_didGetQuitFlag(pGame->pCtx) == GFMRV_FALSE) {
        rv = gfm_handleEvents(pGame->pCtx);
        ASSERT_NR(rv == GFMRV_OK);
        
        // Update stuff
        while (gfm_isUpdating(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_fpsCounterUpdateBegin(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
            
            // Update everything
            rv = intro_update(pGame);
            ASSERT_NR(rv == GFMRV_OK);
            
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
        }
        // Draw stuff
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
            
            // Draw everything
            rv = intro_draw(pGame);
            ASSERT_NR(rv == GFMRV_OK);
            
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    intro_clean(pGame);
    
    return rv;
}

