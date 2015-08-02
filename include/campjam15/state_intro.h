/**
 * @file src/state_intro.h
 * 
 * Intro state; Plays a little animation and then start the battle againts the
 * doc
 */
#ifndef __STATE_INTO_H__
#define __STATE_INTO_H__

#include <campjam15/gameCtx.h>

/**
 * The game's first state
 * 
 * @param  gameCtx The game's context
 * @return         GFMRV_OK, GFMRV_ARGUMENTS_BAD, ...
 */
gfmRV intro(gameCtx *pGame, int jumpIntro);

/**
 * @return GFMRV_TRUE (if did shoot), GFMRV_FALSE (otherwise), ...
 */
gfmRV shoot_bullet(gameCtx *pGame, int x, int y);

#endif /* __STATE_INTO_H__ */

