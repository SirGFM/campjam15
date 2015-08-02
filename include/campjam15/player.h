/**
 * @file src/player.c
 * 
 */
#ifndef __PLAYERSTRUCT_H__
#define __PLAYER_STRUCTH__

/** The player's struct */
typedef struct stPlayer player;

#endif /* __PLAYER_STRUCTH__ */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <campjam15/gameCtx.h>
#include <GFraMe/gfmError.h>

enum enPlayerAnim {
    PL_SLEEP = 0,
    PL_DOWN,
    PL_NORM_STAND,
    PL_NORM_HAPPY,
    PL_STAND,
    PL_WALK,
    PL_SHOOT,
    PL_HIT,
    PL_MAX
};
typedef enum enPlayerAnim playerAnim;

/**
 * Initialize the player and all of its animations
 * 
 * @param  ppPl  The player
 * @param  pGame The game's context
 * @param  x     The player's horizontal position
 * @param  y     The player's vertical position
 * @return       ...
 */
gfmRV player_init(player **ppPl, gameCtx *pGame, int x, int y);

/**
 * Release the player
 * 
 * @param  pPl   The player
 * @return       ...
 */
gfmRV player_free(player *pPl);

/**
 * Make the player play an animation
 * 
 * @param  pPl  The player
 * @param  anim The animation
 * @return      ...
 */
gfmRV player_play(player *pPl, playerAnim anim);

/**
 * Collide the player against everything
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_collide(player *pPl, gameCtx *pGame);

/**
 * Makes the player wave vertically
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_updateWave(player *pPl, gameCtx *pGame);

/**
 * Draw the player
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_draw(player *pPl, gameCtx *pGame);

#endif /* __PLAYER_H__ */

