/**
 * @file src/collision.c
 * 
 * Handle collisions
 */
#include <campjam15/collision.h>
#include <campjam15/doc.h>
#include <campjam15/gameCtx.h>
#include <campjam15/player.h>

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTypes.h>

/**
 * Collide a player against the world
 */
gfmRV collidePlayerWorld(gfmObject *pPplObj, gfmObject *pWorld) {
    gfmRV rv;
    
    // Collide it horizontally and vertically
    rv = gfmObject_separateHorizontal(pPplObj, pWorld);
    ASSERT_NR(rv == GFMRV_OK || rv == GFMRV_COLLISION_NOT_TRIGGERED);
    rv = gfmObject_separateVertical(pPplObj, pWorld);
    ASSERT_NR(rv == GFMRV_OK || rv == GFMRV_COLLISION_NOT_TRIGGERED);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Handle a collision that just happened
 */
gfmRV collide(gfmQuadtreeRoot *pRoot) {
    gfmRV rv;
    
    // Handle the first collision and overlaps everything else
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv == GFMRV_QUADTREE_OVERLAPED) {
        gfmObject *pObj1, *pObj2;
        gfmSprite *pSpr1, *pSpr2;
        void *pChild1, *pChild2;
        int type1, type2;
        
        // Get which objects overlaped
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pRoot);
        ASSERT_NR(rv == GFMRV_OK);
        
        // Retrieve each object's type
        rv = gfmObject_getChild((void*)&pSpr1, &type1, pObj1);
        ASSERT_NR(rv == GFMRV_OK);
        rv = gfmObject_getChild((void*)&pSpr2, &type2, pObj2);
        ASSERT_NR(rv == GFMRV_OK);
        
        // If the object is a sprite, retrieve its child
        if (type1 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild1, &type1, pSpr1);
            ASSERT_NR(rv == GFMRV_OK);
        }
        if (type2 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild2, &type2, pSpr2);
            ASSERT_NR(rv == GFMRV_OK);
        }
        
        if      (type1 == COLLIDEABLE && type2 == PLAYER)
            rv = collidePlayerWorld(pObj2/*pl*/, pObj1/*world*/);
        else if (type2 == COLLIDEABLE && type1 == PLAYER)
            rv = collidePlayerWorld(pObj1/*pl*/, pObj2/*world*/);
        else
            rv = GFMRV_FUNCTION_FAILED;
        ASSERT_NR(rv == GFMRV_OK);
        
        rv = gfmQuadtree_continue(pRoot);
        ASSERT_NR(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

