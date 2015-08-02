/**
 * @file include/campjam15/collision.h
 * 
 * Handle collisions
 */
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmQuadtree.h>

/**
 * Handle a collision that just happened
 */
gfmRV collide(gfmQuadtreeRoot *pRoot);

#endif /* __COLLISION_H__ */

