// Adapted from: https://www.rosettacode.org/wiki/Number_reversal_game#C
// 09/2024 peter.sieg2@gmx.de

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

bool quitapp=false;

void keyPress_cb( char sym) {

	if (sym > 31 ) putchar(sym);
	if (sym == 13) putchar('\n');
	
	if ( sym == 0x1b) quitapp = true;
}

int check_array(int *arr, int len)
{
    int i;
    for (i=--len; i>2; i--)
    {
        if(arr[i]!=(arr[i-1]+1))
            return 0;
    }
    return 1;
}

void do_flip(int *list, int length, int num)
{
    //Flip a part on an array
    int swap;
    int i;
    for(i=2;i<--num;i++)
    {
        swap=list[i];
        list[i]=list[num];
        list[num]=swap;
    }
}
 
void shuffle_list(int *list, int len)
{
    //We'll just be swapping 100 times. Could be more/less. Doesn't matter much.
    int n=50;
    int a=0;
    int b=0;
    int buf = 0;
    int i;

    i=len-2;
    while(n--)
    {
        //for(i=2;i<11;i++)printf("%d ",list[i]); printf("\n");
        //Get random locations and swap
        a = 2+rand()%i; 
        b = 2+rand()%i; 
        buf = list[a];
        list[a] = list[b];
        list[b] = buf;
        
        //do_flip(list, len, rand()%len);
    }
    // "shuffled to ordered state" fix:
    if (check_array(list, len)) shuffle_list (list, len);
}


int main(int argc, char **argv) {
    int list[11] = {0,0,1,2,3,4,5,6,7,8,9};
    int tries=0;
    unsigned int i;
    int input;

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
	VIDEO_SetBlack(FALSE);

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
	
	if (KEYBOARD_Init(keyPress_cb) == 0) printf("keyboard initialised\n");
    srand(time(NULL));//

    printf("Number Reversal Game. Type 2-9 to flip the first n numbers.\n");
    printf("Win by sorting the numbers in ascending order.\n");
    printf("Anything besides numbers  are ignored.\n");
    printf(" |1_2_3_4_5_6_7_8_9|\n");
    shuffle_list(list,11);

	do {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		//char key = getchar();
        printf("\n%d:", tries);
        for(i=2;i<11;i++)printf("%d ",list[i]);
        printf(" ?: ");
        while(1)
        {
            //Just keep asking for proper input
            scanf("%d", &input);

            if(input>1&&input<10)
                break;
 
            printf("\n%d - Please enter a number between 2 and 9: ", (int)input);
        }
        tries++;
        input +=2;
        do_flip(list, 11, input);

		if (check_array(list, 11)) {
			printf("Hurray! You solved it in %d moves!\n", tries);
			quitapp=true;
		}
		
		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) quitapp=true;

		// Wait for the next frame
		VIDEO_WaitVSync();
	} while(!quitapp);

	return 0;
}