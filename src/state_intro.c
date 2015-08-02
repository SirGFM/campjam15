/**
 * @file src/state_intro.c
 * 
 * Intro state; Plays a little animation and then start the battle againts the
 * doc
 */
#include <campjam15/collision.h>
#include <campjam15/doc.h>
#include <campjam15/gameCtx.h>
#include <campjam15/player.h>

#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTilemap.h>

#include <string.h>

/** Current state of the intro animation */
enum enIntroState {
    intro_begin = 0,
    intro_flash,
    intro_end,
    intro_game,
    intro_gameover,
    intro_max
};
typedef enum enIntroState introState;

/** All the texts that are shown before the actual game... (or whatever) */
/**/
#define NUM_TEXTS 1
char *pTexts[] = {
    "A",
/**
#define NUM_TEXTS 5
char *pTexts[] = {
    "MWAHAHAHA",
    "THE ULTIMATE LIFE FORM IS ALMOST COMPLETE",
    "THIS CYBER T-REX WILL BE ABLE TO TRAVEL BACK IN TIME AND WILL ALLOW ME TO "
        "CONTROL ALL DINOSSAURS",
    "AND WITH THEIR COMBINED POWERS, THE WORLD WILL FINALLY BE MINE!",
    "CYBER T-REX... ACTIVATE!"
/**/
};

static int pBulletAnimData[] = {
/* len,fps,loop,frames...*/
    2 , 16, 0  ,64,65,
    2 , 16, 0  ,64,66,
    0
};
static int bulletAnimDataLen = sizeof(pBulletAnimData) / sizeof(int) - 1;

/** Array with the tilemap animation */
static int pMachineFxAnim[] = {
/* len,fps,loop,frames...*/
    4 , 16,  1 ,128,129,130,131,
    0
};
/** Array with a flash animation, done with tilemaps */
static int pFlashFxAnim[] = {
/* len,fps,loop,frames...*/
    18 , 30,  0 ,172,171,170,169,168,167,166,165,164,163,162,161,160,192,193,
                 194,195,196,
    2  , 8 ,  0 ,196,197,
    5  , 30,  0 ,197,198,199,200,201,
    0
};

/** This state's context */
struct stIntroCtx {
    /** The evil doc sprite */
    doc *pDoc;
    /** The player's single bullet */
    gfmSprite *pBullet1;
    gfmSprite *pBullet2;
    /** Effect on top of the machine */
    gfmTilemap *pMachineFx;
    /** Effect that 'flashes' the screen */
    gfmTilemap *pFlashFx;
    /** Current text being displayed */
    int currentText;
    /** For how long the text should be shown after completion */
    int delayOnComplete;
    /** For how long the 'flashing' animation has run */
    int flashAnimTime;
    /** Which state should be animated and drawn */
    introState state;
    /** The player sprite */
    player *pPl;
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
    int height, width;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // Initialize the background
    rv = gfmTilemap_init(pGame->common.pTMap, pGame->pSset8x8, 1/*width*/,
        1/*height*/, -1/*defTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Load the background from a file
    rv = gfmTilemap_loadf(pGame->common.pTMap, pGame->pCtx, "lab.gfm",
            7/*filenameLen*/, pDictNames, pDictTypes, dictLen);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Alloc the fx tilemap
    rv = gfmTilemap_getNew(&(pIntro->pMachineFx));
    ASSERT_NR(rv == GFMRV_OK);
    // Initialize and load the fx
    rv = gfmTilemap_init(pIntro->pMachineFx, pGame->pSset8x8, 5/*width*/,
        9/*height*/, -1/*defTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_loadf(pIntro->pMachineFx, pGame->pCtx, "machine-fx.gfm",
            14/*filenameLen*/, pDictNames, pDictTypes, dictLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Set its position
    rv = gfmTilemap_setPosition(pIntro->pMachineFx, 24/*x*/, 120/*y*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Add the tilemap animations
    rv = gfmTilemap_addAnimations(pIntro->pMachineFx, pMachineFxAnim,
            sizeof(pMachineFxAnim) / sizeof(int) - 1);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_recacheAnimations(pIntro->pMachineFx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Alloc the fx tilemap
    rv = gfmTilemap_getNew(&(pIntro->pFlashFx));
    ASSERT_NR(rv == GFMRV_OK);
    // Initialize and load the flash fx
    rv = gfmTilemap_init(pIntro->pFlashFx, pGame->pSset8x8, 40/*width*/,
        30/*height*/, -1/*defTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_loadf(pIntro->pFlashFx, pGame->pCtx, "flash-fx.gfm",
            14/*filenameLen*/, pDictNames, pDictTypes, dictLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Add the tilemap animations
    rv = gfmTilemap_addAnimations(pIntro->pFlashFx, pFlashFxAnim,
            sizeof(pFlashFxAnim) / sizeof(int) - 1);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmTilemap_recacheAnimations(pIntro->pFlashFx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Get the camera's dimensions (to limit the text's dimensions)
    rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    // Prepare the text
    rv = gfmText_init(pGame->common.pText, 0/*x*/, 24/*y*/, width / 8,
            4/*maxLines*/, 100/*delay*/, 0/*dontBindToWorld*/, pGame->pSset8x8,
            0/*firstTile*/);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Initialize the player
    rv = player_init(&(pIntro->pPl), pGame, 36/*x*/, 150/*y*/);
    ASSERT_NR(rv == GFMRV_OK);
    // Initialize the doc
    rv = doc_init(&(pIntro->pDoc), pGame, 145/*x*/, 186/*y*/);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Alloc and initialize the bullet
    rv = gfmSprite_getNew(&(pIntro->pBullet1));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_init(pIntro->pBullet1, 1000, -100, 14/*width*/, 8/*height*/,
            pGame->pSset16x16, -1/*offX*/, -4/*offY*/, 0/*child*/, BULLET);
    ASSERT_NR(rv == GFMRV_OK);
    // Stupid trick, add the animation twice so we can reset it
    rv = gfmSprite_addAnimations(pIntro->pBullet1, pBulletAnimData, bulletAnimDataLen);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_getNew(&(pIntro->pBullet2));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_init(pIntro->pBullet2, 1000, -100, 14/*width*/, 8/*height*/,
            pGame->pSset16x16, -1/*offX*/, -4/*offY*/, 0/*child*/, BULLET);
    ASSERT_NR(rv == GFMRV_OK);
    // Stupid trick, add the animation twice so we can reset it
    rv = gfmSprite_addAnimations(pIntro->pBullet2, pBulletAnimData, bulletAnimDataLen);
    ASSERT_NR(rv == GFMRV_OK);
    
    pIntro->state = intro_begin;
    
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
gfmRV intro_update_game(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    gfmInputState reset;
    int nreset;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    rv = gfm_getKeyState(&reset, &nreset, pGame->pCtx, pGame->resetHnd);
    ASSERT_NR(rv == GFMRV_OK);
    if ((reset & gfmInput_justPressed) == gfmInput_justPressed) {
        pGame->state = state_reset;
        return GFMRV_OK;
    }
    
    // Update the player
    rv = player_update(pIntro->pPl, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    // Update the doc
    rv = doc_update(pIntro->pDoc, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    // Update the bullet
    rv = gfmSprite_update(pIntro->pBullet1, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_update(pIntro->pBullet2, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Collide everything
    do {
        int height, width;
        
        // Initialize the quadtree, making it a little bigger than the screen
        rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfmQuadtree_initRoot(pGame->common.pQt, -2/*x*/, -2/*y*/,
                width + 4, height + 4, 4/*maxDepth*/, 6/*maxNodes*/);
        ASSERT_NR(rv == GFMRV_OK);
        
        // Populate the quadtree with the tilemap's hitboxes
        rv = gfmQuadtree_populateTilemap(pGame->common.pQt,
                pGame->common.pTMap);
        ASSERT_NR(rv == GFMRV_OK);
        
        // Collides the player against the world
        rv =  player_collide(pIntro->pPl, pGame);
        ASSERT_NR(rv == GFMRV_OK);
        // Collides the doc against the world
        rv = doc_collide(pIntro->pDoc, pGame);
        ASSERT_NR(rv == GFMRV_OK);
        // Collide the bullet against the world
        rv = gfmQuadtree_collideSprite(pGame->common.pQt, pIntro->pBullet1);
        ASSERT_NR(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE);
        // If a collision was detected, handle it and continue the operation
        if (rv == GFMRV_QUADTREE_OVERLAPED) {
            rv = collide(pGame->common.pQt);
            ASSERT_NR(rv == GFMRV_OK);
        }
        rv = gfmQuadtree_collideSprite(pGame->common.pQt, pIntro->pBullet2);
        ASSERT_NR(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE);
        // If a collision was detected, handle it and continue the operation
        if (rv == GFMRV_QUADTREE_OVERLAPED) {
            rv = collide(pGame->common.pQt);
            ASSERT_NR(rv == GFMRV_OK);
        }
    } while (0);
    
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
gfmRV intro_update_flash(gameCtx *pGame) {
    gfmRV rv;
    int elapsed;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    rv = gfmTilemap_update(pIntro->pFlashFx, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Update this animation's time so we can move to the next one
    rv = gfm_getElapsedTime(&elapsed, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    pIntro->flashAnimTime += elapsed;
    // Change the state after 2 seconds
    if (pIntro->flashAnimTime > 2000) {
        // TODO Change to the last part of the animation
        pIntro->state = intro_game;
    }
    
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
gfmRV intro_update_begin(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // TODO Update everything
    
    // If the previous text finished playing, start the next
    rv = gfmText_didFinish(pGame->common.pText);
    ASSERT_NR(rv == GFMRV_TRUE || rv == GFMRV_FALSE);
    if (rv == GFMRV_TRUE) {
        // If the timer is out, update the text
        if (pIntro->delayOnComplete <= 0) {
            char *pText;
            
            if (pIntro->currentText >= NUM_TEXTS) {
                // Go to the next state
                pIntro->state++;
                // Make the t-rex a cyber t-rex
                rv = player_play(pIntro->pPl, PL_STAND);
                ASSERT_NR(rv == GFMRV_OK);
            }
            else {
                // Get the current text
                pText = pTexts[pIntro->currentText];
                rv = gfmText_setText(pGame->common.pText, pText, strlen(pText),
                        1/*doCopy*/);
                ASSERT_NR(rv == GFMRV_OK);
                // Update the timer
                pIntro->delayOnComplete = 1500;

                // Go to the next text
                pIntro->currentText++;
                
                // Awake the player during the doc's monologue
                if (pIntro->currentText == 3) {
                    rv = player_play(pIntro->pPl, PL_NORM_STAND);
                    ASSERT_NR(rv == GFMRV_OK);
                }
            }
        }
        else {
            int elapsed;
            
            // Decrement the timer if it still hasn't finished
            rv = gfm_getElapsedTime(&elapsed, pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
            
            pIntro->delayOnComplete -= elapsed;
        }
    }
    else {
        rv = gfmText_update(pGame->common.pText, pGame->pCtx);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    rv = gfmTilemap_update(pIntro->pMachineFx, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = player_updateWave(pIntro->pPl, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    
    // TODO Collide everything
#if 0
    do {
        int height, width;

        rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfmQuadtree_initRoot(pGame->common.pQt, -2/*x*/, -2/*y*/, width + 4,
                height + 4, 4/*maxDepth*/, 6/*maxNodes*/);
        ASSERT_NR(rv == GFMRV_OK);

        rv = gfmQuadtree_populateTilemap(pGame->common.pQt, pGame->common.pTMap);
        ASSERT_NR(rv == GFMRV_OK);

        rv =  player_collide(pIntro->pPl, pGame);
        ASSERT_NR(rv == GFMRV_OK);
        rv =  doc_collide(pIntro->pDoc, pGame);
        ASSERT_NR(rv == GFMRV_OK);
    } while (0);
#endif
    
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
gfmRV intro_draw_game(gameCtx *pGame) {
    gfmRV rv;
    introCtx *pIntro;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    rv = gfmTilemap_draw(pGame->common.pTMap, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = player_draw(pIntro->pPl, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    rv = doc_draw(pIntro->pDoc, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_draw(pIntro->pBullet1, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_draw(pIntro->pBullet2, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    // TODO Draw the bullets and the enemies...
    
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
gfmRV intro_draw_flash(gameCtx *pGame) {
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
    
    rv = player_draw(pIntro->pPl, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    rv = doc_draw(pIntro->pDoc, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = gfmTilemap_draw(pIntro->pFlashFx, pGame->pCtx);
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
gfmRV intro_draw_begin(gameCtx *pGame) {
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
    
    rv = player_draw(pIntro->pPl, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    rv = doc_draw(pIntro->pDoc, pGame);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = gfmTilemap_draw(pIntro->pMachineFx, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = gfmText_draw(pGame->common.pText, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
#if 0
    rv = gfmQuadtree_drawBounds(pGame->common.pQt, pGame->pCtx, 0/*colors*/);
    ASSERT_NR(rv == GFMRV_OK);
#endif
    
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
    gfmSprite_free(&(pIntro->pBullet1));
    gfmSprite_free(&(pIntro->pBullet2));
    gfmTilemap_free(&(pIntro->pFlashFx));
    gfmTilemap_free(&(pIntro->pMachineFx));
    player_free(pIntro->pPl);
    doc_free(pIntro->pDoc);
    
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
            switch (_intro.state) {
                case intro_begin: {
                    rv = intro_update_begin(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                case intro_flash: {
                    rv = intro_update_flash(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                case intro_game: {
                    rv = intro_update_game(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                default: break;
            }
            
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
        }
        // Draw stuff
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
            
            // Draw everything
            switch (_intro.state) {
                case intro_begin: {
                    rv = intro_draw_begin(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                case intro_flash: {
                    rv = intro_draw_flash(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                case intro_game: {
                    rv = intro_draw_game(pGame);
                    ASSERT_NR(rv == GFMRV_OK);
                } break;
                default: break;
            }
            
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT_NR(rv == GFMRV_OK);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    intro_clean(pGame);
    
    return rv;
}

/**
 * @return GFMRV_TRUE (if did shoot), GFMRV_FALSE (otherwise), ...
 */
gfmRV shoot_bullet(gameCtx *pGame, int x, int y) {
    gfmRV rv;
    gfmSprite *pSpr;
    int bX, bY, height, width;
    introCtx *pIntro;
    
    // Set default values
    pSpr = 0;
    
    // Sanitize arguments
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame->pState, GFMRV_ARGUMENTS_BAD);
    // Check that the current state is the intro state
    ASSERT(pGame->state == state_intro, GFMRV_FUNCTION_FAILED);
    
    // Get the current state
    pIntro = (introCtx*)(pGame->pState);
    
    // Check that the bullet isn't on screen
    rv = gfmSprite_getPosition(&bX, &bY, pIntro->pBullet1);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    if (bX < 0 || bX > width) {
        pSpr = pIntro->pBullet1;
    }
    else {
        rv = gfmSprite_getPosition(&bX, &bY, pIntro->pBullet2);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfm_getCameraDimensions(&width, &height, pGame->pCtx);
        ASSERT_NR(rv == GFMRV_OK);
        if (bX < 0 || bX > width) {
            pSpr = pIntro->pBullet2;
        }
    }
    
    ASSERT(pSpr, GFMRV_FALSE);
    
    // Set the bullet's position
    rv = gfmSprite_setPosition(pSpr, x, y);
    ASSERT_NR(rv == GFMRV_OK);
    // Set it's velocity
    rv = gfmSprite_setHorizontalVelocity(pSpr, 200);
    ASSERT_NR(rv == GFMRV_OK);
    // Restart the animation
    rv = gfmSprite_playAnimation(pSpr, 1);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_playAnimation(pSpr, 0);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_TRUE;
__ret:
    return rv;
}

