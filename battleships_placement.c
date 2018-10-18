/** FILE: battleships_placement.c
 * AUTHORS: Daniel Watt and Sheldon Zhang
 * DATE: 16/10/2018
 * DESCRIPTION: A series of helper functions to assist with a 
 * battleships placement implementation. Specifically handles ship 
 * rotation, updating ship position after a navswitch push, and 
 * recording an int matrix into a normal matrix.
 */


#include "cursor.h"
#include "button.h"
#include <stdbool.h>

#define HORIZONTAL 0
#define VERTICAL 1



uint8_t shipMatrix[ROWS_NUM][COLS_NUM] = {{0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0},
										  {0, 0, 0, 0, 0}};



void updateShipPosition (uint8_t cursorMatrix[], Cursor* shipCursor) 
/* Updates the position of the ship the player is placing given what 
 * navswitch buttons the player has pressed. */
{
	updateMatrixPosition(cursorMatrix, shipCursor) ;
}



void recordMatrix (const uint8_t shipPositionsIntMatrix[], int numRows, int numCols)
/* Records/copies a given int matrix into the global variable shipMatrix */
{
	int row = 0;
	int col = 0;
	for (row=numRows-1; row >= 0; row--) {
		for (col=0; col < numCols; col++) {
			shipMatrix[row][col] = (shipPositionsIntMatrix[col] >> row) & 1;
		}
	}
}



bool isRotateAllowed(const uint8_t cursorMatrix[], int shipDirection)
/* Check if a rotate is legal at the current ship position. Return true 
 * if legal, else false.*/
{
	if (shipDirection == HORIZONTAL) {	
		if ((cursorMatrix[0] & cursorMatrix[COLS_NUM-1])) {
			return false;
		}
	} 
	return true;
}



void updateShipRotation(uint8_t cursorIntMatrix[], int boatLength, Cursor* shipCursor) 
/* Checks to see if the player has pressed the white button to rotate 
 * the ship. If they have, then this function rotates the ship in the 
 * cursorIntMatrix by 90 degrees if the rotation is legal.*/
{
	int columnNum = 0;
	int bit = 0;
	int column = 0;
	static int shipDirection = HORIZONTAL;
	
	if (button_push_event_p(0) && boatLength != 1) {
		if (isRotateAllowed(cursorIntMatrix, shipDirection)) {
			if (shipDirection == HORIZONTAL) {
				columnNum = 0;
				/* For each column, take the shipHull->row-th bit (the bit on the row we are currently at) and put it into columnNum */
				for (column = 0; column < LEDMAT_COLS_NUM; column++) {
					bit = cursorIntMatrix[column] >> shipCursor->row;
					columnNum += bit;
					columnNum <<= 1;
					cursorIntMatrix[column] = 0;
				}
				columnNum >>= 1;
				/* Get rid of any leading 0s */
				while (!(columnNum & 1)) {
					columnNum >>= 1;
				}
				/* Add the proper amount of leading 0s given the row we are on */
				columnNum <<= (shipCursor->row - 1);
				if (columnNum > MAX_ROW_NUM) {
					columnNum &= ~(1 << LEDMAT_ROWS_NUM); /* Clear the overflow bit */
					columnNum |= 1; /* Add the overflow bit to the top of the screen */
				}
				cursorIntMatrix[shipCursor->column] = columnNum;
				shipDirection = VERTICAL;
				
				
			} else if (shipDirection == VERTICAL) {
				clearIntMatrix(cursorIntMatrix, LEDMAT_COLS_NUM);
				/* Put the boat on the screen in horizontal position */
				for (column=0; column < boatLength; column++) {
					cursorIntMatrix[column] = (1 << shipCursor->row);
				}
				/* Move the boat into the correct position */
				moveColsToRight(cursorIntMatrix, LEDMAT_COLS_NUM, shipCursor->column - 1);
				shipDirection = HORIZONTAL;
			}
		}
	}	
}
