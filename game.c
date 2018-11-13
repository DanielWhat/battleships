/* *********************** BATTLESHIPS FOR UCFK ***********************
 * FILE: game.c
 * AUTHORS: Daniel Watt and Sheldon Zhang
 * DATE: 16/10/2018
 * DESCRIPTION: A battleships implementation for the UCFK"
 */


#include "system.h"
#include "task.h"
#include "ledmat.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "cursor.h"
#include "music.h"
#include "battleships_placement.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Task scheduler constants */
#define LOOP_RATE 250
#define PACER_RATE 500
#define COMMUNICATION_RATE 100
#define NUM_TASKS 4

#define MAX_NUM_HITS 4

#define DEFAULT_COL 2
#define DEFAULT_ROW 3

/* Specifies how many bits in an 8-bit message should be allocated to the 
 * column number. The remaining bits are allocated by default to the row number */
#define COL_BITS 4

/* Constants to control cursor blinking */
#define NUMBER_OF_ITERATIONS_CURSOR_ON 5
#define NUMBER_OF_ITERATIONS_CURSOR_OFF 5
#define START_CURSOR_DISPLAY_NUM NUMBER_OF_ITERATIONS_CURSOR_OFF
#define END_CURSOR_DISPLAY_NUM (NUMBER_OF_ITERATIONS_CURSOR_ON + NUMBER_OF_ITERATIONS_CURSOR_OFF)

#define TEXT_SPEED 10

#define BOAT_LENGTH 3
#define BOAT_MATRIX {0x00, 0x08, 0x08, 0x08, 0x00}


static uint8_t sendBuffer = 0;
static uint8_t receiveBuffer = 0;

static int playerNum; //Specifies if player 1 or 2


typedef struct gameData_s
{
    //Takes values 'P' -> PlacementLoop, 'F' -> fireLoop, 'W' -> WaitingLoop, 'E' -> EndLoop
    char phase;
    //Takes values 'P' -> PlacementLoop, 'F' -> fireLoop, 'W' -> WaitingLoop, 'E' -> EndLoop, 'N' -> None
    char lastPhase;
    Cursor* cursor;
    Cursor* shipHull;
    bool isLastMoveHit;
    int yourHits;
    int opponentHits;
    bool readyToSend;
    bool readyToReceive;
} GameData;


/* Background music */
char* theme = "<C,C,G,C,G#,C,C,F,C,G,C,G#,C,C,F,G,C,C,G,C,G#,C,C,A#,C,G#,C,G,C,C,G,G#,E#,E#,G#,E#,E#,G#,E#,G#,E#,E#,G,E#,E#,G,E#,G,D#,D#,G,D#,A#,D#,G#,G,D,D,G,D,G#,D,G,F,>1000";													   
														   
extern uint8_t shipMatrix[ROWS_NUM][COLS_NUM];						   
static uint8_t shipIntMatrix[COLS_NUM] = {0x00};



void communicationLoop (void* data)
/* Sends what is in the sendBuffer and reads into the receiveBuffer, but
 * only if readyToSend or readyToReceive are set to true respectively.*/
{
	GameData* gameData = data;
	
	if (gameData->readyToReceive) {
		if (ir_uart_read_ready_p()) {
			receiveBuffer = ir_uart_getc();
			gameData->readyToReceive = false;
		}
	}
	
	if (gameData->readyToSend) {
		ir_uart_putc(sendBuffer);
		gameData->readyToSend = false;
		sendBuffer = 0;
	}	
}



void tinygl_general_init(void)
/* A general initialisation function for tinygl (text routine).*/
{
    tinygl_init(LOOP_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (TEXT_SPEED);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
}



void endLoop (GameData* gameData)
/* Display the end game screen. Win or loose messages are displayed 
 * depending of if the player won and lost the game.*/
{
    if (gameData->lastPhase != 'E') { //i.e if we were previously in a different Loop/Phase
        tinygl_general_init();
        gameData->lastPhase = 'E';

        if (gameData->yourHits >= MAX_NUM_HITS) {
            tinygl_text ("YOU WIN! ");
        } else {
            tinygl_text ("YOU LOOSE ");
        }
    }
    tinygl_update();
}



void waitingLoop(GameData* gameData)
/* A loop that handles the waiting screen that players see while they 
 * are waiting for the opponent to end their turn. This loop also 
 * listens for a fire/shoot message from the opponent and gives them hit
 * or miss feedback accordingly. Once hit/miss feedback is given to the 
 * opponent, the player is moved to the fire phase or end phase. */
{
	int column = 0;
	int row = 0;
	static uint8_t displayCol = 0;
	static bool isFirstCall = false;
	
    if (gameData->lastPhase != 'W') { //i.e if we were previously in a different Loop/Phase
        tinygl_general_init();
        ledmat_init();
        gameData->readyToReceive = true;
        
        /*If we were previously in the placement phase, then it is the first time we have called this function */
        isFirstCall = gameData->lastPhase == 'P' ? true : false;

        if (gameData->isLastMoveHit) {
            tinygl_text ("HIT! ");
        } else {
            tinygl_text ("MISS ");
        }
        gameData->lastPhase = 'W';
    }
    
    
    if (receiveBuffer != 0) {
		row = receiveBuffer >> COL_BITS;  
		column = receiveBuffer - (row << COL_BITS); 
		
		/* Check that column and row are in the acceptable ranges */
		if (column <= COLS_NUM && row <= ROWS_NUM) {
			if (shipMatrix[row-1][column-1] == 1) {
				shipMatrix[row-1][column-1] = 0; /* You can't hit the same place twice */
				gameData->opponentHits++;
				sendBuffer = 'H';
			} else {
				sendBuffer = 'M';
			}
			
			gameData->readyToSend = true;
			receiveBuffer = 0;
			gameData->phase = gameData->opponentHits >= MAX_NUM_HITS ? 'E' : 'F';
			
		} else {
			receiveBuffer = 0;
			gameData->readyToReceive = true;
		}
	}
	
    button_update();
    /* If the button is down show the player their ship positions */
    if (button_down_p(BUTTON1) || isFirstCall) {
		ledmat_display_column(shipIntMatrix[displayCol], displayCol);
		displayCol++;
		displayCol %= COLS_NUM;
    } else {
		tinygl_update();
	}
}



bool isHit(void)
/* Returns true if the receiveBuffer contains the hit character 'H'. 
 * Otherwise, returns false. */
{
	if (receiveBuffer == 'H') {
		return true;
	} else {
		return false;
	}
}



bool recordShot(GameData* gameData, uint8_t hitMatrix[], uint8_t missMatrix[])
/* Checks to see if a shot has been made. Returns true if a shot is 
 * made, false otherwise. If a shot has been made, this function 
 * queries the other UCFK to see if it was a hit. If it was a hit, then
 * it is recorded in the hitMatrix and otherwise it is recorded in the 
 * missMatrix. */
{
	Cursor* cursor = gameData->cursor;
	/* Since 0 in the receiveBuffer is considered "no message has arrived", we 
	 * need to add one to row and col so that the position (0,0) can be selected. */
    uint8_t cursorPosition = ((cursor->row + 1) << COL_BITS) + (cursor->column + 1);
     
    if (navswitch_down_p (NAVSWITCH_PUSH)) {
		sendBuffer = cursorPosition;
		ir_uart_putc(sendBuffer);
		 
		while (!ir_uart_read_ready_p()) {
			button_update();
			if (button_push_event_p(BUTTON1)) {
				ir_uart_putc(sendBuffer);
			}
		}
		 
		receiveBuffer = ir_uart_getc();
		if (receiveBuffer != 0) {
			if (isHit()) {
				hitMatrix[cursor->column] |= cursor->rowNum;
				gameData->isLastMoveHit = true;
				gameData->yourHits++;
				 
			} else {
				missMatrix[cursor->column] |= cursor->rowNum;
				gameData->isLastMoveHit = false;
			}
			receiveBuffer = 0;
			return true; 
		}
		return false;
    } else {
        return false;
    }
}



void fireLoop (GameData* gameData)
/* A loop that handles the firing phase of the game. When players push 
 * the navswitch down, their shot will be recorded as hit or miss and 
 * they will move on to the waiting phase or end phase. */
{
    Cursor* cursor = gameData->cursor;
    static bool isTurnOver = false;
    static int frameCounter = 0;
    static int i = 0;
    static uint8_t hitMatrix[COLS_NUM] = {0x00};
    static uint8_t missMatrix[COLS_NUM] = {0};
    uint8_t* currentMatrix = hitMatrix; /* A pointer to the current matrix being displayed */

    if (gameData->lastPhase != 'F') { //i.e if we were previously in a different Loop/Phase
        ledmat_init();
        button_init();
        gameData->lastPhase = 'F';
    }

    navswitch_update();
    button_update();
    updateCursorPosition(cursor);

    if (button_down_p(BUTTON1)) {
            currentMatrix = missMatrix;
        } else {
            currentMatrix = hitMatrix;
    }

    //Display the currently selected Matrix
    if (i == cursor->column) {
		//Only display the cursor for certain frame intervals to simulate blinking
        if (frameCounter > START_CURSOR_DISPLAY_NUM) {
            ledmat_display_column(currentMatrix[i] | cursor->rowNum, i);
            frameCounter = frameCounter >= END_CURSOR_DISPLAY_NUM ? 0 : frameCounter + 1;
        } else {
            ledmat_display_column(currentMatrix[i], i);
            frameCounter++;
        }
        
    } else {
            ledmat_display_column(currentMatrix[i], i);
    }
    
    isTurnOver = recordShot(gameData, hitMatrix, missMatrix);
    if (isTurnOver) {
        gameData->phase = gameData->yourHits >= MAX_NUM_HITS ? 'E' : 'W';
        isTurnOver = false;
    }
    i++;
    i = i % COLS_NUM;
}



int selectPlayer(void) 
/* Allows players to select whether they are player 1 or 2. Player 1 
 * goes first. Returns player number (1 or 2). */
{
	int playerNumber = 1;
	bool isSelected = false;
	system_init();
	tinygl_init(PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text("1");
    
    pacer_init(PACER_RATE);
    
    while (!isSelected) {
		pacer_wait();
		tinygl_update();
		navswitch_update();
		
		if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
			tinygl_text("2");
			playerNumber = 2;
		} else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
			tinygl_text("1");
			playerNumber = 1;
		}
		
		if (navswitch_release_event_p(NAVSWITCH_PUSH)) {
			isSelected = true;
		}	
	}
	return playerNumber;
}



void placementLoop(GameData* gameData)
/* Allows each player to place their ships. */
{
	Cursor* shipHull = gameData->shipHull;
	static int shipsPlaced = 0;
	static int currentColumn = 0;
	int column = 0;
	static int boatLength = BOAT_LENGTH;
	static uint8_t cursorMatrix[COLS_NUM] = BOAT_MATRIX;
	
	if (gameData->lastPhase != 'P') { // i.e if we were previously in a different phase
		ledmat_init();
		ir_uart_init();
		gameData->lastPhase = 'P';
	}
	
	navswitch_update();
	button_update();
	ledmat_display_column(shipIntMatrix[currentColumn] | cursorMatrix[currentColumn], currentColumn);
	updateShipPosition(cursorMatrix, shipHull);
	updateShipRotation(cursorMatrix, boatLength, shipHull);
	currentColumn++;
	currentColumn %= COLS_NUM;
	
	if (navswitch_release_event_p(NAVSWITCH_PUSH)) {
		if (!isMatrixOverlap(cursorMatrix, shipIntMatrix, COLS_NUM)) { /* Since you can't place a ship on top of another ship*/
			/* Copy cursorMatrix into shipIntatrix */
			for (column = 0; column < COLS_NUM; column++) {
				if (cursorMatrix[column] != 0) {
					shipIntMatrix[column] |= cursorMatrix[column];
				}
			}
			shipsPlaced++;
			
			if (shipsPlaced == 1) {
				boatLength = 1;
				clearIntMatrix(cursorMatrix, COLS_NUM); 
				cursorMatrix[DEFAULT_COL] = (1 << DEFAULT_ROW);
				shipHull->column = DEFAULT_COL;
				shipHull->row = DEFAULT_ROW;
			}
			if (shipsPlaced == 2) {
				recordMatrix(shipIntMatrix, ROWS_NUM, COLS_NUM);
				if (playerNum == 1) {
					gameData->phase = 'F';
				} else {
					gameData->phase = 'W';
				}
			}
		}	
	}	
}



void playLoop (void* data)
/* Switches between game phases depending on the gameData->phase 
 * variable.*/
{
    GameData* gameData = data;
    
    if (gameData->phase == 'P') {
		placementLoop(gameData);
	} else if (gameData->phase == 'F') {
        fireLoop(gameData);
    } else if (gameData->phase == 'W') {
        waitingLoop(gameData);
    } else if (gameData->phase == 'E') {
        endLoop(gameData);
    }
}



void generalInit(void)
/* A grouping of initialisation functions. */
{
    system_init();
    navswitch_init();
    button_init();
    ir_uart_init();
}



int main (void)
{    
    playerNum = selectPlayer();
    MusicObj musicObj;
    musicInit(&musicObj);
    Cursor cursor = {.column = DEFAULT_COL, .row = DEFAULT_ROW, .rowNum = (1 << DEFAULT_ROW)};
    Cursor shipHull = {.column = DEFAULT_COL, .row = DEFAULT_ROW, .rowNum = (1 << DEFAULT_ROW)};
    generalInit();
    GameData gameData = {.phase = 'P', .lastPhase = 'N', .cursor = &cursor, .shipHull = &shipHull, .isLastMoveHit = false, .yourHits = 0, .opponentHits = 0, .readyToReceive = false, .readyToSend = false};

    task_t tasks[] =
    {
        {.func = playLoop, .period = TASK_RATE / LOOP_RATE, .data=&gameData},
        {.func = tweeterTask, .period = TASK_RATE / TWEETER_TASK_RATE, .data=&musicObj},
        {.func = tuneTask, .period = TASK_RATE / TUNE_TASK_RATE, .data=&musicObj},
        {.func = communicationLoop, .period = TASK_RATE / COMMUNICATION_RATE, .data=&gameData},
    };

    task_schedule (tasks, NUM_TASKS);
    return 0;
}
