/**
 * @file src/doc.c
 * 
 */
#include <campjam15/collision.h>
#include <campjam15/doc.h>
#include <campjam15/gameCtx.h>
#include <campjam15/state_intro.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <stdlib.h>
#include <string.h>

int pDocAnimData[] = {
/*          len,fps,loop,frames...*/
/* STAND  */ 1 , 0 ,  0 , 32,
/* WALK   */ 2 , 16,  1 , 33,34,
/* SHIELD */ 13, 16,  0 , 35,35,36,37,38,39,38,39,38,37,36,35,35,
/* HIT    */ 6 , 12,  0 , 40,41,40,41,40,41,
/* SPAWN  */ 4 , 16,  0 , 35,35,35,35,
/* SHOOT  */ 
             0
};
int docAnimDataLen = sizeof(pDocAnimData) / sizeof(int) - 1;

struct stDoc {
    gfmSprite *pSpr;
    docAnim anim;
};

/**
 * Initialize the doc and all of its animations
 * 
 * @param  ppDoc The doc
 * @param  pGame The game's context
 * @param  x     The doc's horizontal position
 * @param  y     The doc's vertical position
 * @return       ...
 */
gfmRV doc_init(doc **ppDoc, gameCtx *pGame, int x, int y) {
    gfmRV rv;
    doc *pDoc;
    
    // Set default value
    pDoc = 0;
    
    // Sanitize arguments
    ASSERT(ppDoc, GFMRV_ARGUMENTS_BAD);
    ASSERT(!(*ppDoc), GFMRV_ARGUMENTS_BAD);
    
    // Alloc the doc
    pDoc = (doc*)malloc(sizeof(doc));
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    // Make sure everything is clean
    memset(pDoc, 0x0, sizeof(doc));
    
    // Alloc the doc's sprite
    rv = gfmSprite_getNew(&(pDoc->pSpr));
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfmSprite_init(pDoc->pSpr, x, y, 10/*width*/, 22/*height*/,
            pGame->pSset32x32, -12/*offX*/, -8/*offY*/, pDoc/*child*/, DOC);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Add every animation
    rv = gfmSprite_addAnimations(pDoc->pSpr, pDocAnimData, docAnimDataLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Play the sleep animation
    rv = doc_play(pDoc, DOC_STAND);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Set the acceleration
    rv = gfmSprite_setVerticalAcceleration(pDoc->pSpr, 500);
    ASSERT_NR(rv == GFMRV_OK);
    
    *ppDoc = pDoc;
    rv = GFMRV_OK;
__ret:
    if (rv != GFMRV_OK && pDoc)
        free(pDoc);
    
    return rv;
}

/**
 * Release the doc
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_free(doc *pDoc) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    gfmSprite_free(&(pDoc->pSpr));
    free(pDoc);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Collide the doc against everything
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_collide(doc *pDoc, gameCtx *pGame) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    // Actually collide the doc
    rv = gfmQuadtree_collideSprite(pGame->common.pQt, pDoc->pSpr);
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
 * Updates the doc's AI
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_update(doc *pDoc, gameCtx *pGame) {
    double vx, vy;
    gfmCollision dir;
    gfmInputState action, jump, left, right;
    gfmRV rv;
    int nAction, nJump, nLeft, nRight;
    int justShielded;
    
    // Set default values
    justShielded = 0;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    ASSERT(pGame, GFMRV_ARGUMENTS_BAD);
    
    // Get all the key states
    rv = gfm_getKeyState(&action, &nAction, pGame->pCtx, pGame->p2ActionHnd);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_getKeyState(&jump, &nJump, pGame->pCtx, pGame->p2JumpHnd);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_getKeyState(&left, &nLeft, pGame->pCtx, pGame->p2LeftHnd);
    ASSERT_NR(rv == GFMRV_OK);
    rv = gfm_getKeyState(&right, &nRight, pGame->pCtx, pGame->p2RightHnd);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Get the current status of the player (if it's touching anything)
    rv = gfmSprite_getCollision(&dir, pDoc->pSpr);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Make it fall
    rv = gfmSprite_setVerticalAcceleration(pDoc->pSpr, 500);
    ASSERT_NR(rv == GFMRV_OK);
    // Make sure it doesn't clip through the floor
    if ((dir & gfmCollision_down) == gfmCollision_down) {
        // If we are touching the ground and pressed jump, JUMP
        if ((jump & gfmInput_justPressed) == gfmInput_justPressed) {
            rv = gfmSprite_setVerticalVelocity(pDoc->pSpr, -200);
            ASSERT_NR(rv == GFMRV_OK);
        }
        else {
            rv = gfmSprite_setVerticalVelocity(pDoc->pSpr, 32);
            ASSERT_NR(rv == GFMRV_OK);
        }
    }
    
    // Horizontal movement...
    if ((left & gfmInput_pressed) == gfmInput_pressed) {
        rv = gfmSprite_setHorizontalVelocity(pDoc->pSpr, -64);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if ((right & gfmInput_pressed) == gfmInput_pressed) {
        rv = gfmSprite_setHorizontalVelocity(pDoc->pSpr, 64);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else {
        rv = gfmSprite_setHorizontalVelocity(pDoc->pSpr, 0);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    // If the action button was pressed, try to shield
    if ((action & gfmInput_justPressed) == gfmInput_justPressed) {
        if (pDoc->anim != DOC_SHIELD) {
            justShielded = 1;
        }
    }
    
    // Update the animation
    rv = gfmSprite_getVelocity(&vx, &vy, pDoc->pSpr);
    ASSERT_NR(rv == GFMRV_OK);
    if (pDoc->anim == DOC_HIT) {
        int x, y;
        
        // Do nothing if we were hit
        rv = gfmSprite_setVelocity(pDoc->pSpr, 0, 0);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfmSprite_getPosition(&x, &y, pDoc->pSpr);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfmSprite_setPosition(pDoc->pSpr, x, y + 2);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if (justShielded) {
        rv = doc_play(pDoc, DOC_SHIELD);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if (pDoc->anim == DOC_SHIELD) {
        rv = gfmSprite_didAnimationFinish(pDoc->pSpr);
        if (rv == GFMRV_TRUE) {
            rv = doc_play(pDoc, DOC_STAND);
            ASSERT_NR(rv == GFMRV_OK);
        }
        
        // On this animation, force the player to stand still
        rv = gfmSprite_setVelocity(pDoc->pSpr, 0, 0);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if (vx == 0.0) {
        rv = doc_play(pDoc, DOC_STAND);
        ASSERT_NR(rv == GFMRV_OK);
    }
    else if (vx != 0.0) {
        rv = doc_play(pDoc, DOC_WALK);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    // Actually update the sprite
    rv = gfmSprite_update(pDoc->pSpr, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return GFMRV_OK;
}

/**
 * Draw the doc
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_draw(doc *pDoc, gameCtx *pGame) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    rv = gfmSprite_draw(pDoc->pSpr, pGame->pCtx);
    ASSERT_NR(rv == GFMRV_OK);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Try to hit the doc
 */
gfmRV doc_hit(doc *pDoc, gfmSprite *pBul) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    // If we are shielding, check if it's an iFrame
    if (pDoc->anim == DOC_SHIELD) {
        int frame;
        
        rv = gfmSprite_getFrame(&frame, pDoc->pSpr);
        ASSERT_NR(rv == GFMRV_OK);
        
        if (frame >= 36 && frame <= 39) {
            // Reflect the bullet
            rv = gfmSprite_setHorizontalVelocity(pBul, -200);
            ASSERT_NR(rv == GFMRV_OK);
            // Restart the animation
            rv = gfmSprite_playAnimation(pBul, 0);
            ASSERT_NR(rv == GFMRV_OK);
            rv = gfmSprite_playAnimation(pBul, 1);
            ASSERT_NR(rv == GFMRV_OK);
        }
        else {
            // We are hit!
            rv = doc_play(pDoc, DOC_HIT);
            ASSERT_NR(rv == GFMRV_OK);
        }
    }
    else if (pDoc->anim != DOC_HIT) {
        // We are hit!
        rv = doc_play(pDoc, DOC_HIT);
        ASSERT_NR(rv == GFMRV_OK);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV doc_play(doc *pDoc, docAnim anim) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    ASSERT(anim < DOC_MAX, GFMRV_ARGUMENTS_BAD);
    
    // If we just shoot, make sure it finished playing
    if (pDoc->anim == DOC_SHIELD) {
        rv = gfmSprite_didAnimationFinish(pDoc->pSpr);
        ASSERT(rv == GFMRV_TRUE, GFMRV_OK);
    }
    
    // Set the animation
    rv = gfmSprite_playAnimation(pDoc->pSpr, anim);
    ASSERT_NR(rv == GFMRV_OK);
    
    pDoc->anim = anim;
    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV doc_wasHit(doc *pDoc) {
    gfmRV rv;
    
    // Sanitize arguments
    ASSERT(pDoc, GFMRV_ARGUMENTS_BAD);
    
    ASSERT(pDoc->anim == DOC_HIT, GFMRV_FALSE);
    rv = GFMRV_TRUE;
__ret:
    return rv;
}

