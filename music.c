/** FILE: music.c
 * AUTHORS: Daniel Watt and Sheldon Zhang
 * DATE: 16/10/2018
 * DESCRIPTION: Plays background music/theme music.
 */

#include "music.h"
#include "pio.h"
#include "../../extra/tweeter.h"
#include "../../extra/mmelody.h"

#define PIEZO_PIO PIO_DEFINE (PORT_D, 6)

#define TWEETER_TASK_RATE 10000
#define TUNE_TASK_RATE 100
#define TUNE_BPM_RATE 200



void musicInit(MusicObj* musicObj)
/* Initialises the music routine.*/
{
    static tweeter_scale_t scale_table[] = TWEETER_SCALE_TABLE(TWEETER_TASK_RATE);
    musicObj->tweeter = tweeter_init(&musicObj->tweeterInfo, TWEETER_TASK_RATE, scale_table);
    pio_config_set(PIEZO_PIO, PIO_OUTPUT_LOW);

    musicObj->melody = mmelody_init(&musicObj->melodyInfo, TUNE_TASK_RATE, (mmelody_callback_t) tweeter_note_play, musicObj->tweeter);
    mmelody_speed_set (musicObj->melody, TUNE_BPM_RATE);
    mmelody_play (musicObj->melody, theme);
}



void tuneTask (void* data)
/* Changes the note for tweeterTask to play when required. */
{
    MusicObj* musicObj = data;
    mmelody_update(musicObj->melody);
}



void tweeterTask (void* data)
/* Plays the note set by tuneTask. */
{
    MusicObj* musicObj = data;
    pio_output_set(PIEZO_PIO, tweeter_update(musicObj->tweeter));
}
