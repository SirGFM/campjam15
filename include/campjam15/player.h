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
    SLEEP = 0,
    DOWN,
    NORM_STAND,
    NORM_HAPPY,
    STAND,
    WALK,
    SHOOT,
    HIT
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
 * Collide the player against everything
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_collide(player *pPl, gameCtx *pGame);

/**
 * Draw the player
 * 
 * @param  pPl The player
 * @return     ...
 */
gfmRV player_draw(player *pPl, gameCtx *pGame);

#endif /* __PLAYER_H__ */

