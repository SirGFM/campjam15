/**
 * @file src/player.c
 * 
 */
#include <campjam15/collision.h>
#include <campjam15/gameCtx.h>
#include <campjam15/player.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <stdlib.h>
#include <string.h>

int pPlAnimData[] = {
/*              len,fps,loop,frames...*/
/* SLEEP      */ 1 , 0 ,  0 , 48,
/* DOWN       */ 1 , 0 ,  0 , 49,
/* NORM_STAND */ 1 , 0 ,  0 , 50,
/* NORM_HAPPY */ 3 , 12,  1 , 50,50,51,
/* STAND      */ 1 , 0 ,  0 , 52,
/* WALK       */ 2 , 12,  1 , 53,54,
/* SHOOT      */ 1 , 0 ,  0 , 55,
/* HIT        */ 6 , 12,  0 , 56,57,56,57,
                 0
};
int plAnimDataLen = sizeof(pPlAnimData) / sizeof(int) - 1;

struct stPlayer {
    gfmSprite *pSpr;
};

/**
 * Initialize the player and all of its animations
 * 
 * @param  pPl   The player
 * @param  pGame The game's context
 * @param  x     The player's horizontal position
 * @param  y     The player's vertical position
 * @return       ...
 */
gfmRV player_init(player **ppPl, gameCtx *pGame, int x, int y) {
    gfmRV rv;
    player *pPl;
    
    // Set default value
    pPl = 0;
    
    // Sanitize arguments
    ASSERT(ppPl, GFMRV_ARGUMENTS_BAD);
    ASSERT(!(*ppPl), GFMRV_ARGUMENTS_BAD);
    
    // Alloc the player
    pPl = (player*)malloc(sizeof(player));
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    
    // Make sure everything is clean
    memset(pPl, 0x0, sizeof(player));
    
    // Alloc the player's sprite
    rv = gfmSprite_getNew(&(pPl->pSpr));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_init(pPl->pSpr, x, y, 14/*width*/, 26/*height*/,
            pGame->pSset32x32, -8/*offX*/, -5/*offY*/, 0/*child*/, PLAYER);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Add every animation
    rv = gfmSprite_addAnimations(pPl->pSpr, pPlAnimData, plAnimDataLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Play the sleep animation
    rv = gfmSprite_playAnimation(pPl->pSpr, PL_SLEEP);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Set the acceleration
    rv = gfmSprite_setVerticalAcceleration(pPl->pSpr, -500);
    ASSERT_NR(rv == GFMRV_OK);
    
    *ppPl = pPl;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pPl)
        free(pPl);
    
    return rv;
}

/**
 * Release the player
 * 
 * @param  pPl   The player
 * @return       ...
 */
gfmRV player_free(player *pPl) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    
    gfmSprite_free(&(pPl->pSpr));
    free(pPl);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Make the player play an animation
 * 
 * @param  pPl        The player
 * @param  playerAnim The animation
 * @return            ...
 */
gfmRV player_play(player *pPl, playerAnim anim) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    ASSERT(anim < PL_MAX, GFMRV_ARGUMENTS_BAD);
    
    rv = gfmSprite_playAnimation(pPl->pSpr, anim);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Collide the player against everything
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_collide(player *pPl, gameCtx *pGame) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    
    // Actually collide the player
    rv = gfmQuadtree_collideSprite(pGame->common.pQt, pPl->pSpr);
    ASSERT_NR(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE);
    
    // If a collision was detected, handle it and continue the operation
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collide(pGame->common.pQt);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Makes the player wave vertically
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_updateWave(player *pPl, gameCtx *pGame) {
    gfmRV rv;
    double vy;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    
    // Retrieve the vertical velocity and check if it should switch directions
    rv = gfmSprite_getVerticalVelocity(&vy, pPl->pSpr);
    ASSERT_NR(rv == GFMRV_OK);
    
    if (vy > 15.0) {
        rv = gfmSprite_setVerticalAcceleration(pPl->pSpr, -30);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if (vy < -15.0) {
        rv = gfmSprite_setVerticalAcceleration(pPl->pSpr, 30);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    // Actually update the sprite
    rv = gfmSprite_update(pPl->pSpr, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Updates the player, allowing the player to control it
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_update(player *pPl, gameCtx *pGame) {
    gfmCollision dir;
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    
    // Get the current status of the player (if it's touching anything)
    rv = gfmSprite_getCollision(&dir, pPl->pSpr);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Make it fall
    rv = gfmSprite_setVerticalAcceleration(pPl->pSpr, 500);
    ASSERT_NR(rv == GFMRV_OK);
    // Make sure it doesn't clip through the floor
    if ((dir & gfmCollision_down) == gfmCollision_down) {
        rv = gfmSprite_setVerticalVelocity(pPl->pSpr, 32);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    // Actually update the sprite
    rv = gfmSprite_update(pPl->pSpr, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

/**
 * Draw the player
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_draw(player *pPl, gameCtx *pGame) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pPl, GFMRV_ARGUMENTS_BAD);
    
    rv = gfmSprite_draw(pPl->pSpr, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

