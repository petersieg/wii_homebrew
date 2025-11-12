/* **************************************************************************************************** */
/* Main file for SDL Space Invaders game by Felix and Georg Potthast                                    */
/* **************************************************************************************************** */

#define wiisupport
#define soundsupport

#include "spaceinv.h"

#ifdef wiisupport
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <time.h>
#endif



void init(char *);
void cleanup(void);
void getInput(void);
void draw(void);
void initPlayer(void);
void doPlayer(void);
void doEntities(void);
void loadAllSprites(void);
void loadAllSounds(void);
void addUFO(int, int);
void doCollisions(void);
void delay(unsigned int);
void mdelay(int millisecs);
void drawPlayer();
void drawEntities();
void drawImage(SDL_Surface *image, int x, int y);
void freeSprites();
void freeSounds();
void addBullet(int x, int y);
int checkifnobullet(void);
void addBomb(int x, int y);
static void moveUFO(void); 
void Drawstring(SDL_Surface *screen, int X, int Y, char text[]);
void addUFO(int x, int y);
void addShelter(int x, int y);
static void moveALIEN(void);
void drawSprite(Entity *self, int x, int y);
void addALIEN(int type, int x, int y);
void initaliens(void);
static void moveShelter(void);
void loadAllSounds(void);
void playSound(int index);
void calcnextbombposition(int *bombstart_x,int *bombstart_y);
static void moveStandardBomb(void); 
static void moveUFO(void);
int getmyticks(void);   

SDL_Surface *bmpfont;
SDL_Surface *bmpsprites;
SDL_Surface *bmpimg;
SDL_Rect area;
 
static int firstalien = 0;
static int aliendirection = 1;
static int alienoffset = 0;
static int alienverticaloffset = 0;
static int aliensleft = 0;
static int alienspeed = 1;
static int bombspeed = 2000;
static int bombdelay = 0;
static int i_ufo=0;
static int ufo_channel=0; 
static int overrunline=0;
   
int testvalue1, testvalue2, testvalue3;
          
 #include "spaceinv.inc"
              
int main(int argc, char *argv[])
{
    unsigned int frameLimit = getmyticks()+16;
	int go;
	
	/* Start up SDL */
	
	init("SDL Space Invaders");
	
	/* Call the cleanup function when the program exits */
	
	atexit(cleanup);
	
	SDL_ShowCursor(SDL_DISABLE);
	
	go = 1;  
 
    game.lives = 3;
	    
	//  Load bmp image
      bmpimg=SDL_LoadBMP("isplash.bmp");
      if (bmpimg == NULL) {
       printf("Can't load image: %s\n", SDL_GetError());
       exit(1);
      }
     SDL_BlitSurface(bmpimg,0,game.screen,0);
     SDL_Flip(game.screen);

     pressanykey();  
	  
	/* Load all the sounds */
	
	loadAllSounds();
	
	/* Initialise the aliens - always first!*/
	
    initaliens();
    alienoffset = 55; //do not reuse these entities
	
	/* Intialise the player */
	
    initPlayer();
	
    addShelter(120, 380);
    addShelter(230, 380);
    addShelter(340, 380);
    addShelter(450, 380);
     	 
	/* Loop indefinitely for messages */
	
	while (go == 1)
	{
		/* Get the input */
		
		getInput();
		
		/* Update the player's position */
		
	    doPlayer();
    		
		/* Update the entities */
		
		doEntities();
		  
		/* Do the collisions */
		
		doCollisions();
		
		/* Draw everything */
		
	    draw();
	      
	    /* Another new wave of aliens? */
	    if (aliensleft == 0) { 
			mdelay(2000); 
            reinit();
            }
		/* Sleep briefly to stop sucking up all the CPU time */
	 	
	    delay(frameLimit);
		
	    frameLimit = getmyticks()+30;

	}
	
	/* Exit the program */
	
	exit(0);
}
 
