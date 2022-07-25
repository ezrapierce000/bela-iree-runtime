#include <Bela.h>
#include <signal.h>


#include <unistd.h>
#include <cstdlib>
#include <libgen.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <functional>

// Handle Ctrl-C by requesting that the audio rendering stop
void interrupt_handler(int var)
{
    gShouldStop = true;
}


bool mysetup (BelaContext* ctx, void* var){
	return 1;
}

void myrender (BelaContext* ctx, void* var){
	volatile int a = 6;
	a += 3;
}


int main()
{
	// Set default settings
	BelaInitSettings settings;	// Standard audio settings
	Bela_defaultSettings(&settings);
        // you must have defined these function pointers somewhere and assign them to `settings` here.
        // only `settings.render` is required.
	settings.setup = mysetup;
	settings.render = myrender;

	// Initialise the PRU audio device
	if(Bela_initAudio(&settings, 0) != 0) {
                fprintf(stderr, "Error: unable to initialise audio");
		return -1;
	}

	// Start the audio device running
	if(Bela_startAudio()) {
		fprintf(stderr, "Error: unable to start real-time audio");
		// Stop the audio device
		Bela_stopAudio();
		// Clean up any resources allocated for audio
		Bela_cleanupAudio();
		return -1;
	}

	// Set up interrupt handler to catch Control-C and SIGTERM
	signal(SIGINT, interrupt_handler);
	signal(SIGTERM, interrupt_handler);

	// Run until told to stop
	while(!gShouldStop) {
		usleep(100000);
	}

	// Stop the audio device
	Bela_stopAudio();

	// Clean up any resources allocated for audio
	Bela_cleanupAudio();

	// All done!
	return 0;
}
