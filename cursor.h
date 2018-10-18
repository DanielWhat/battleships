/** FILE: cursor.h
 *  AUTHORS: Daniel Watt and Sheldon Zhang
 *  DATE: 16/10/2018
 *  DESCRIPTION: Implements an abstraction of a cursor to track the
 *  movement of a dot or object on the screen.
 */


#ifndef CURSOR_H
#define CURSOR_H

#include "int_matrix.h"
#include "navswitch.h"
#include "system.h"
 
#define MAX_ROW_NUM 0x7f
#define MIN_ROW_NUM 0x01
#define COLS_NUM LEDMAT_COLS_NUM
#define ROWS_NUM LEDMAT_ROWS_NUM



typedef struct cursor_s
{
    int column;
    int row;     
    int rowNum;

} Cursor;
 
 
 
void updateCursorPosition(Cursor* cursor);
/* Updates the cursor position by checking to see if the navswitch
* has been pushed and updates cursor position accordingly (i.e pushing
* navswitch NORTH moves cursor up etc.) */



void updateMatrixPosition(uint8_t cursorMatrix[], Cursor* cursor);
/* Similar to updateCursorPosition but instead of moving only the cursor,
 * moves the entire matrix as well. Ideal to use if the cursor is 
 * tracking the centre of a large object on the screen. */

#endif /* CURSOR_H */
