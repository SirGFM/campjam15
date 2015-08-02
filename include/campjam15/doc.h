/**
 * @file src/doc.h
 * 
 */
#ifndef __DOC_STRUCT_H__
#define __DOC_STRUCT_H__

/** The doc's struct */
typedef struct stDoc doc;

#endif /* __DOC_STRUCT_H__ */

#ifndef __DOC_H__
#define __DOC_H__

#include <campjam15/gameCtx.h>
#include <GFraMe/gfmError.h>

enum enDocAnim {
    DOC_STAND,
    DOC_WALK,
    DOC_SHIELD,
    DOC_HIT,
    DOC_SPAWN,
    DOC_SHOOT,
    DOC_MAX
};
typedef enum enDocAnim docAnim;

/**
 * Initialize the doc and all of its animations
 * 
 * @param  ppDoc The doc
 * @param  pGame The game's context
 * @param  x     The doc's horizontal position
 * @param  y     The doc's vertical position
 * @return       ...
 */
gfmRV doc_init(doc **ppDoc, gameCtx *pGame, int x, int y);

/**
 * Release the doc
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_free(doc *pDoc);

/**
 * Collide the doc against everything
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_collide(doc *pDoc, gameCtx *pGame);

/**
 * Makes the doc wave vertically
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_update(doc *pDoc, gameCtx *pGame);

/**
 * Draw the doc
 * 
 * @param  pDoc The doc
 * @return      ...
 */
gfmRV doc_draw(doc *pDoc, gameCtx *pGame);

/**
 * Try to hit the doc
 */
gfmRV doc_hit(doc *pDoc, gfmSprite *pBul);

gfmRV doc_play(doc *pDoc, docAnim anim);

gfmRV doc_wasHit(doc *pDoc);

#endif /* __DOC_H__ */

