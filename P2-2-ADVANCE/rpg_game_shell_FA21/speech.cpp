#include "speech.h"

#include "globals.h"
#include "hardware.h"

/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0,90,110,110, 0x33FFF);
}

void erase_speech_bubble()
{
    uLCD.filled_rectangle(0,90,110,110, BLACK);
}
#define TOP    0
#define BOTTOM 1
void draw_speech_line(const char* line, int which)
{
    if (which==TOP){
    uLCD.locate(1,12);
    uLCD.printf(line);
    }
    else {
    uLCD.locate(1,13);
    uLCD.printf(line);        
    }
        
}

void speech_bubble_wait()
{
    GameInputs inputs = read_inputs();
    while (true) { 
        inputs = read_inputs();
        if (inputs.b3==0)
        {
            break; }
    }
    

}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void long_speech(const char* lines[], int n)
{
  for (int i = 0; i < n; i+=2) {
        draw_speech_bubble();
        draw_speech_line(lines[i], TOP);
        if ((i + 1) < n) {
            draw_speech_line(lines[i + 1], BOTTOM);
        }
        speech_bubble_wait();
    }  
}
