/** FILE: int_matrix.c
 *  AUTHORS: Daniel Watt and Sheldon Zhang
 *  DATE: 16/10/2018
 *  DESCRIPTION: A series of helper functions for manipulating 8-bit 
 * "int matricies" (i.e matricies where each column is reprisented by an 
 *  8-bit number in a list, and each row is a certain bit position in 
 *  the 8-bit numbers).
 */


#ifndef INT_MATRIX_H
#define INT_MATRIX_H

#include <stdbool.h>
#include <stdint.h>



void moveColsToLeft(uint8_t* matrix, int numCols, int numMoves);
/* Moves the columns of an int matrix to the left such that column j becomes 
 * j-1. If j-1 is less than 0, then the columns loop (ie numCols-1-th 
 * column is given the data of the 0th column). Will do this operation
 * numMoves times.*/



void moveRowsDown(uint8_t intMatrix[], int maxRowNum, int numCols, int numRows, int numMoves);
/* Moves the rows of an int matrix down such that row i becomes row i+1. 
 * If i+1 is greater than to numCols-1, then the rows loop (i.e the 
 * numCols-1-th row becomes the 0th row). maxRowNum is the highest 
 * number that can be displayed in a matrix column (i.e if the matrix 
 * has 3 rows then the highest number that can be displayed is 2^3 - 1).*/



void moveColsToRight(uint8_t* matrix, int numCols, int numMoves);
/* Moves the columns of an int matrix to the right such that column j becomes 
 * j+1. If j+1 exceeds numCols-1, then the columns loop (ie numCols-th 
 * column becomes 0). Will do this operation numMoves times.*/



void moveRowsUp(uint8_t intMatrix[], int numRows, int numCols, int numMoves);
/* Moves the rows of an int matrix up such that row i becomes row i-1. 
 * If i-1 is less than 0, then it loops (i.e the numCols-1-th row is 
 * given the data of the 0th row. */



void clearIntMatrix(uint8_t matrix[], int colsNum);
/* Takes an int matrix and the number of columns it has, and clears it 
 * (i.e sets each entry to 0). */



bool isMatrixOverlap(uint8_t intMatrix1[], uint8_t intMatrix2[], int numCols);
/* Returns true if, for the same column, both int matricies have a 1 in the 
 * same bit. Otherwise, returns false. */


#endif /* INT_MATRIX_H */
