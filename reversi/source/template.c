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

int p,t,a,d,c,v,i,m[90],s,r[]={-10,-9,-8,-1,1,8,9,10};
char h[]=" - o x\n";

void keyPress_cb( char sym) {

	if (sym > 31 ) putchar(sym);
	if (sym == 13) putchar('\n');
	
	//if ( sym == 0x1b) quitapp = true;
}

void k()
{
    if(m[p]==0)
        for(i=0;i<8;i++)
        {
            for(c=0,v=p+r[i];m[v]==3-t;v+=r[i])
                c++;
            if(c && m[v] == t)
            {
                a+=c;
                v=p;
                if(d)
                    do 
                        m[v]=t,v+=r[i];
                    while(m[v]-t);
            }
        }
}


int main(int argc, char **argv) {

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

    printf("Reversi Game. human 'o' starts.\n");
    printf("Enter:h v (h=hori 1-8; v=vert 1-8)\n");

    t=1; //human 'o' starts
    for(i=1,m[41]=m[49]=2;i<10;m[i++*9]=3) m[40]=m[50]=s=1;
    for(;;a=d=0)
    {
        for(p=9;p<82;++p) k(),printf("%.2s",h+m[p]*2);
        if(a)
            for(d=a=s=p=8;a==8;k()) t-2?scanf("%d %d",&p,&i),p+=i*9:++p;
        else if(s)
            s=0,printf("pass");
        else 
            break;
        t=3-t;
        
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) break;

		// Wait for the next frame
		VIDEO_WaitVSync();
		
	} 

	return 0;
}