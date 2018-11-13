/** FILE: cursor.c
 *  AUTHORS: Daniel Watt and Sheldon Zhang
 *  DATE: 16/10/2018
 *  DESCRIPTION: Implements an abstraction of a cursor to track the
 *  movement of a dot or object on the screen.
 */


#include "cursor.h"
#include "int_matrix.h"
#include "navswitch.h"
#include "system.h"
 
#define MAX_ROW_NUM 0x7f
#define MIN_ROW_NUM 0x01
#define COLS_NUM LEDMAT_COLS_NUM
#define ROWS_NUM LEDMAT_ROWS_NUM



static void simpleMod(int* variable, int modNum) 
/* Calculates *variable modulus modNum and puts the answer in the 
 * variable pointer. Ideal keeping a number within 0 to numCols-1 when
 * iterating over an int matrix. */
{
	*variable = *variable % modNum;
    *variable = *variable < 0 ? modNum + *variable : *variable;
}



void updateCursorPosition(Cursor* cursor)
/* Updates the cursor position by checking to see if the navswitch
* has been pushed and updates cursor position accordingly (i.e pushing
* navswitch NORTH moves cursor up etc.) */
{
	if (navswitch_push_event_p (NAVSWITCH_EAST)) {
		cursor->column++;
	} else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
		cursor->column--;
	} else if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
		cursor->rowNum >>= 1;
		cursor->row--;
	} else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
		cursor->rowNum <<= 1;
		cursor->row++;
	}
	
	//Stops the cursor from running off the top, bottom, and sides
	if (cursor->rowNum > MAX_ROW_NUM) {
		cursor->rowNum = 0x01;
	} else if (cursor->rowNum < MIN_ROW_NUM) {
		cursor->rowNum = (1 << (ROWS_NUM-1));
	}
	simpleMod(&cursor->row, ROWS_NUM);
	simpleMod(&cursor->column, COLS_NUM);
}
 
 
 
void updateMatrixPosition(uint8_t cursorMatrix[], Cursor* cursor) 
/* Similar to updateCursorPosition but instead of moving only the cursor,
 * moves the entire matrix as well. Ideal to use if the cursor is 
 * tracking the centre of a large object on the screen. */
{
	if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
		moveRowsUp(cursorMatrix, ROWS_NUM, COLS_NUM, 1);
		cursor->rowNum >>= 1;
		cursor->row--;
			
	} else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
		moveRowsDown(cursorMatrix, MAX_ROW_NUM, COLS_NUM, ROWS_NUM, 1);
		cursor->rowNum <<= 1;
		cursor->row++;
			
	} else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
		moveColsToLeft(cursorMatrix, COLS_NUM, 1);
		cursor->column--;
			
	} else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
		moveColsToRight(cursorMatrix, COLS_NUM, 1);
		cursor->column++;
	}
	
	//Stops the cursor from running off the top, bottom, and sides
	if (cursor->rowNum > MAX_ROW_NUM) {
        cursor->rowNum = 0x01;
    } else if (cursor->rowNum < MIN_ROW_NUM) {
        cursor->rowNum = (1 << (ROWS_NUM-1));
    }
	simpleMod(&cursor->row, ROWS_NUM);
	simpleMod(&cursor->column, COLS_NUM);
}
