/** FILE: battleships_placement.h
 * AUTHORS: Daniel Watt and Sheldon Zhang
 * DATE: 16/10/2018
 * DESCRIPTION: A series of helper functions to assist with a 
 * battleships placement implementation. Specifically handles ship 
 * rotation, updating ship position after a navswitch push, and 
 * recording an int matrix into a normal matrix.
 */

#ifndef BATTLESHIPS_PLACEMENT
#define BATTLESHIPS_PLACEMENT

#include "cursor.h"
#include "button.h"
#include <stdbool.h>

#define HORIZONTAL 0
#define VERTICAL 1



uint8_t shipMatrix[ROWS_NUM][COLS_NUM];



void updateShipPosition (uint8_t cursorMatrix[], Cursor* shipCursor);
/* Updates the position of the ship the player is placing given what 
 * navswitch buttons the player has pressed. */



void recordMatrix (const uint8_t shipPositionsIntMatrix[], int numRows, int numCols);
/* Records/copies a given int matrix into the global variable shipMatrix */



bool isRotateAllowed(const uint8_t cursorMatrix[], int shipDirection);
/* Check if a rotate is legal at the current ship position. Return true 
 * if legal, else false.*/



void updateShipRotation(uint8_t cursorIntMatrix[], int boatLength, Cursor* shipCursor);
/* Checks to see if the player has pressed the white button to rotate 
 * the ship. If they have, then this function rotates the ship in the 
 * cursorIntMatrix by 90 degrees if the rotation is legal.*/


#endif /* BATTLESHIPS_PLACEMENT */
