#include "daisy_pod.h"

#define LONG_HOLD 2000 // 2 second
#define LED_TIMEOUT 2000 // 2 second delay

using namespace daisy;

static DaisyPod pod;

uint32_t	released = 0;

enum BUTTON_STATES {S_INIT, S_BTN_P1, S_BTN_H1};
static int state;

void UpdateHID(void);

int main(void) {
    pod.Init();
		
	state = S_INIT;

    while(1) {
		UpdateHID();
	}
}

void UpdateHID(void) {	

    pod.button1.Debounce();

	switch (state) {
		// initial state
		case S_INIT:
			if (pod.button1.RisingEdge()) {
				// button pushed
				// light red LED
				pod.ClearLeds();
				pod.led1.Set(0.3, 0, 0);
				pod.UpdateLeds();
								
				state = S_BTN_P1;
			}
			
			break;

		// button pushed
		case S_BTN_P1:
			if (pod.button1.FallingEdge()) {
				// button released
				// wait for LED_TIMEOUT then reset LEDs
				released = pod.seed.system.GetNow();
				
				state = S_INIT;
			} else if (pod.button1.Pressed() && pod.button1.TimeHeldMs() > LONG_HOLD) {
				// button held longer than 1 sec
				// brighter red RED
				pod.ClearLeds();
				pod.led1.Set(0.75, 0, 0);
				pod.UpdateLeds();
				
				state = S_BTN_H1;	
			}
			
			break;			

		// button held
		case S_BTN_H1:
			if (pod.button1.FallingEdge()) {
				// button released
				// wait for LED_TIMEOUT then reset LEDs
				released = pod.seed.system.GetNow();
								
				state = S_INIT;				
			}
			
			break;
	}
	
	// reset LEDs after timeout	
	if (state == S_INIT && pod.seed.system.GetNow() - released >= LED_TIMEOUT) {
		pod.ClearLeds();
		pod.led1.Set(0.3, 0.3, 0.3);
		pod.UpdateLeds();
	}
}
