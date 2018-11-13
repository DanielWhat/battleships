/** FILE: music.h
 * AUTHORS: Daniel Watt and Sheldon Zhang
 * DATE: 16/10/2018
 * DESCRIPTION: Plays background music/theme music.
 */


#ifndef MUSIC_H
#define MUSIC_H

#include "pio.h"
#include "../../extra/tweeter.h"
#include "../../extra/mmelody.h"

#define PIEZO_PIO PIO_DEFINE (PORT_D, 6)

#define TWEETER_TASK_RATE 10000
#define TUNE_TASK_RATE 100
#define TUNE_BPM_RATE 200


/* The theme music to be played. See mmelody.h for how to write your own
 * themes/music. */
extern char* theme;



/* A music structure containing tweeters and melody objects nessesary for sound.*/
typedef struct musicObj_s
{
    tweeter_t tweeter;
    mmelody_private_t* melody;
    mmelody_obj_t melodyInfo;
    tweeter_obj_t tweeterInfo;

} MusicObj;



void musicInit(MusicObj* musicObj);
/* Initialises the music routine.*/



void tuneTask (void* data);
/* Changes the note for tweeterTask to play when required. */



void tweeterTask (void* data);
/* Plays the note set by tuneTask. */

#endif /* MUSIC_H */
