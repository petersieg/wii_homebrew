#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

/* collatz sequence 
   see: https://en.wikipedia.org/wiki/Collatz_conjecture
   (C) 2023 P.Sieg - CC-BY-SA

x,n: number to sequence
  c: count sequence
  h: highest value in sequence for x,n

*/

#define NTIMES 100000

/*
void main()
{
    unsigned long x,n,c,h,l,ul = 0;
    printf("Enter starting value (>=5): ");
    scanf("%lu",&l);
    if (l < 5) l = 5;				 // l=lower starting value *
	ul = l + NTIMES;				 // ul=upper limit value *
    printf("\n"); 					 // next line *
	
    for (n=l; n<=ul; n++) {
      x = n;
      c = 0;
      h = 0;
      do {
      	if ((x % 2) == 0) x = x / 2; // if number is equal then div 2 *
      	else x = x * 3 + 1;          // if number is not equal then times 3 plus 1 *
      	c++;						 // increase counter *
      	if (x > h) h = x;			 // update highest number *
      	printf("n=%ld - c=%ld - h=%ld\r",n,c,h);
      } while (x > 1);
      printf("\n"); // next number in new line *
    }
}
*/

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
    unsigned long x,n,c,h,l = 5,ul = 0;
	bool waiting = false; // waiting mode
	
	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[0;0H");

	ul = l + NTIMES;				 /* ul=upper limit value */

	while(1) {

    	for (n=l; n<=ul; n++) {
      	x = n;
      	c = 0;
      	h = 0;
      	
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		if ( pressed & WPAD_BUTTON_A ) waiting = true;
		if ( pressed & WPAD_BUTTON_B ) waiting = false;
		
		// Wait for the next frame
		VIDEO_WaitVSync();
		
      	if (!waiting) {
      	do {
      		if ((x % 2) == 0) x = x / 2; /* if number is equal then div 2 */
      		else x = x * 3 + 1;          /* if number is not equal then times 3 plus 1 */
      		c++;						 /* increase counter */
      		if (x > h) h = x;			 /* update highest number */
      		printf("n=%ld - c=%ld - h=%ld\r",n,c,h);
      	} while (x > 1);
      	printf("\n"); /* next number in new line */
    	}
    	}
	}

	return 0;
}
