/**
 * @file src/doc.c
 * 
 */
#include <campjam15/doc.h>
#include <campjam15/gameCtx.h>

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
/* SPAWN  */ 4 , 16,  0 , 35,35,35,35,
/* HIT    */ 
/* SHOOT  */ 
             0
};
int docAnimDataLen = sizeof(pDocAnimData) / sizeof(int) - 1;

struct stDoc {
    gfmSprite *pSpr;
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
            pGame->pSset32x32, -12/*offX*/, -8/*offY*/, 0/*child*/, DOC);
    ASSERT_NR(rv == GFMRV_OK);
    
    // Add every animation
    rv = gfmSprite_addAnimations(pDoc->pSpr, pDocAnimData, docAnimDataLen);
    ASSERT_NR(rv == GFMRV_OK);
    // Play the sleep animation
    rv = gfmSprite_playAnimation(pDoc->pSpr, DOC_STAND);
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
    
    // TODO Actually collide the player
    rv = gfmQuadtree_populateSprite(pGame->common.pQt, pDoc->pSpr);
    ASSERT_NR(rv == GFMRV_OK);
    
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

