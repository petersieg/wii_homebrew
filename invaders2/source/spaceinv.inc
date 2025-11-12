/* **************************************************************************************************** */
/* This file contains all functions called by the main file: "spaceinv.c"                               */
/* **************************************************************************************************** */

void init(char *title)
{
	/* Initialise SDL Video and Audio */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)  
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		
		exit(1);
	}
	
	/* Open a screen */
	
	game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN); //,0,SDL_HWPALETTE|SDL_DOUBLEBUF);
	
	if (game.screen == NULL)
	{
		printf("Couldn't set screen mode to %d x %d: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());

		exit(1);
	}
	
	/* Set the audio rate to 22050, 16 bit stereo, 2 channels and a 4096 byte buffer */
	
	#ifdef soundsupport
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) != 0)
	{
		printf("Could not open audio: %s\n", Mix_GetError());
		
		exit(1);
	}
	#endif
	
	//  Load bmp font
      bmpfont=SDL_LoadBMP("dosemu.bmp");
      if (bmpfont == NULL) {
       printf("Can't load font: %s\n", SDL_GetError()); //check stdout.txt for this
       exit(1);
      }
	
    //  Load bmp sprites
      bmpsprites=SDL_LoadBMP("invaders.bmp");
      if (bmpfont == NULL) {
       printf("Can't load font: %s\n", SDL_GetError()); //check stdout.txt for this
       exit(1);
      }
    	
	/* Set the screen title */
	
	SDL_WM_SetCaption(title, NULL);
}

void initaliens(void)
{
int i;

  for (i=1;i<12;i++){
     addALIEN(TOP_ALIEN1,    i*50+ lineoffset, topline + linedist*0);
     addALIEN(MIDDLE_ALIEN1, i*50+ lineoffset, topline + linedist*1);
     addALIEN(MIDDLE_ALIEN1, i*50+ lineoffset, topline + linedist*2);
     addALIEN(BOTTOM_ALIEN1, i*50+ lineoffset, topline + linedist*3);
     addALIEN(BOTTOM_ALIEN1, i*50+ lineoffset, topline + linedist*4); 
   }
   
   bombdelay = getmyticks()+bombspeed*2; //don't start game with a bomb        
   
}

void cleanup()
{
	/* Free the sounds */
	
	freeSounds();
	
	/* Close the mixer */
	
	#ifdef soundsupport
	Mix_CloseAudio();
	#endif
	
	/* Shut down SDL */
	
	SDL_Quit();
}

void reinit(void) {
int i=0;	    
           for (i=0;i<MAX_ENTITIES;i++) { //clear entities
           	entity[i].active = 0;
            }

            aliendirection = 1;
            alienoffset = 0;
            alienverticaloffset = 0;
            alienspeed = 1;

            initaliens();
            alienoffset = 55; //do not reuse these entities                         
            aliensleft = 55;

            initPlayer();
	
            addShelter(120, 380);
            addShelter(230, 380);
            addShelter(340, 380);
            addShelter(450, 380);
}

/* **************************************************************************************************** */
void pressanykey()
{
	SDL_Event event;
	int done=0;
	
	while (done==0)
	{
		#ifndef wiisupport
        SDL_PollEvent(&event);  
		switch (event.type)
		{
			/*
			** Closing the Window or pressing Escape will exit the program
			*/
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						exit(0);
						break;
					
					default:
                        done=1; //exit loop on any key
					    break;
				} //switch sym
		} //switch event.type
        #endif
	
        #ifdef wiisupport
        Uint32 p_key; 
        WPAD_ScanPads();
        p_key = WPAD_ButtonsDown(0);    
		if(p_key)
        {
			done=1; //exit loop on any button
            break;
		}
        #endif
        
	} //while
}
/* **************************************************************************************************** */
void getInput()
{

	#ifndef wiisupport
	SDL_Event event;
	int done=0;
	
	/* Loop through waiting messages and process them */
	
	while (!done && SDL_PollEvent(&event))
	{
	    //#ifndef wiisupport
		switch (event.type)
		{
			/*
			** Closing the Window or pressing Escape will exit the program
			** Pressing the arrow keys will move the player
			** Pressing space will fire a bullet
			*/
			
			case SDL_QUIT:
				exit(0);
				break;
			
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
						input.up = 1;
						break;
					
					case SDLK_DOWN:
						input.down = 1;
						break;
						
					case SDLK_LEFT:
						input.left = 1;
						break;
						
					case SDLK_RIGHT:
						input.right = 1;
						break;
						
					case SDLK_SPACE:
						input.fire = 1;
						break;
					
					case SDLK_ESCAPE:
						exit(0);
						break;
					
					default:
					break;
				}
				break;
			
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
						input.up = 0;
						break;
					
					case SDLK_DOWN:
						input.down = 0;
						break;
						
					case SDLK_LEFT:
						input.left = 0;
						break;
						
					case SDLK_RIGHT:
						input.right = 0;
						break;
						
					case SDLK_SPACE:
						input.fire = 0;
						break;
					
					default:
						break;
				}
				break;
		} //switch
		//#endif
		
        
	} //while
	#endif
	
        #ifdef wiisupport
        Uint32 p_key,held; 
		input.left = 0;
		input.right = 0;
		input.fire = 0;
		
        WPAD_ScanPads();
				
        p_key = WPAD_ButtonsDown(0);    
		
		if(p_key&WPAD_BUTTON_HOME)
        {
            exit(0);
		}
		if(p_key&WPAD_BUTTON_UP)
        {
            input.left = 1;
        }
        if(p_key&WPAD_BUTTON_DOWN)
        {
            input.right = 1;
        }
        if (p_key&WPAD_BUTTON_2)
        {
			input.fire = 1;
        }

        held = WPAD_ButtonsHeld(WPAD_CHAN_0); 
		if(held&WPAD_BUTTON_UP)
        {
            input.left = 1;
        }
        else if(held&WPAD_BUTTON_DOWN)
        {
            input.right = 1;
        }
        else if (held&WPAD_BUTTON_2)
        {
			input.fire = 1;
        }
        #endif
}



/* **************************************************************************************************** */
void draw()
{
int i_tmp;
int bomb_x,bomb_y;
char message[35];
     
	/* Blank the screen */
	
	SDL_FillRect(game.screen, NULL, 0);
	
	/* Draw the score */
    sprintf(message,"SCORE: %9ld   Lives remaining: %3d",game.score,game.lives);	
	Drawstring(game.screen, 10, 10, message); //draw string with bmp font

	/* Draw the testvalues */
    //sprintf(message,"Val1: %d Val2: %d Val3: %d",testvalue1,testvalue2,testvalue3);	
	//Drawstring(game.screen, 350, 10, message); //draw string with bmp font

    if (overrunline >= 430) game.lives=0; //shelters are overrun
	
	if (game.lives==0) {
    sprintf(message,"Game over!");	
	Drawstring(game.screen, SCREEN_WIDTH/2 -40, 350, message); //draw string with bmp font
    }
  	
	/* Draw the player */
	
	drawPlayer();
	
	/* Add a bomb from time to time */
	
    if ((bombdelay <= getmyticks()) && (aliensleft>0)) { 
       i_tmp=alienspeed;
       if (i_tmp>3) i_tmp=3;
       bombdelay = getmyticks()+bombspeed/i_tmp;
       calcnextbombposition(&bomb_x,&bomb_y);       
       addBomb(bomb_x, bomb_y);
      }
		
	/* Draw the entities */
	
	if (aliensleft>0) drawEntities();
	
	/* Update the buffer */

 	SDL_Flip(game.screen);

	if (game.lives==0) {
	pressanykey(); //mdelay(10000); 
  	exit(0);
    }


}
/* **************************************************************************************************** */
void initPlayer()
{

	player.x = SCREEN_WIDTH / 2;
	player.y = SCREEN_HEIGHT - 40;
	player.pick_x=121;
    player.pick_y=24;
    player.pick_w=39;
    player.pick_h=25;
    player.sprite_type=LASER_BASE;
}

void doPlayer()
{
	player.thinkTime--;
	
	if (player.thinkTime <= 0)
	{
		player.thinkTime = 0;
	}
	
	if (input.up == 1)
	{
		//player.y -= PLAYER_SPEED;
		
		/* Don't allow the player to move off the screen */
		
		if (player.y < 0)
		{
			player.y = 0;
		}
	}
	
	if (input.down == 1)
	{
		//player.y += PLAYER_SPEED;
		
		/* Don't allow the player to move off the screen */
		
		if (player.y + player.pick_h >= SCREEN_HEIGHT)
		{
			player.y = SCREEN_HEIGHT - (player.pick_h + 1);
		}
	}
	
	if (input.left == 1)
	{
		player.x -= PLAYER_SPEED;
		
		/* Don't allow the player to move off the screen */
		
		if (player.x < 0)
		{
			player.x = 0;
		}
	}
	
	if (input.right == 1)
	{
		player.x += PLAYER_SPEED;
		
		/* Don't allow the player to move off the screen */
		
		if (player.x + player.pick_w >= SCREEN_WIDTH)
		{
			player.x = SCREEN_WIDTH - (player.pick_w + 1);
		}
	}
	
	if (input.fire == 1)
	{
		/* You can only fire when the thinkTime is 0 or less */
		
		if ((player.thinkTime <= 0) && (checkifnobullet() == 0))  //just one bullet on screen
		{
			addBullet(player.x + 19, player.y );
			player.thinkTime = MAX_RELOAD_TIME;
		}
	}
}

void drawPlayer()
{
	/* Draw the image in the player structure */
	self = &player;
	drawSprite(self, self->x, self->y);
}

/* **************************************************************************************************** */
void clearEntities()
{
	int i;
	
	/* Clear the list */

	for (i=0;i<MAX_ENTITIES;i++)
	{
		memset(&entity[i], 0, sizeof(Entity));
	}
}

int getFreeEntity()
{
	int i;
	
	/* Loop through all the entities and find a free slot */
	
	for (i=alienoffset+1;i<MAX_ENTITIES;i++) //need +1 !
	{
		if (entity[i].active == 0)
		{
			memset(&entity[i], 0, sizeof(Entity));
			
			entity[i].active = 1;
			
			return i;
		}
	}
	
	/* Return -1 if you couldn't any free slots */
	
	return -1;
}

void doEntities()
{
	int i=0;
    int j=0;
    int k=0;
	int rightoffset=0; //aliens reached right side and move one line down
	int leftoffset=0;
	static Uint32 aliendelay;
    int done =0;
                	
	/* Loop through the entities and perform their action */
	
	for (i=0;i<MAX_ENTITIES;i++)
	{
		self = &entity[i];
		
        if (self->action == &moveALIEN) {
            if (aliendelay <= getmyticks()) { 
                               //check for last alien here
                if (self == &entity[firstalien+54]) {
                         aliendelay = getmyticks()+(400/alienspeed);       
                         }
	            //if (self == &entity[firstalien]) playSound(HEARTBEEP);   
	            
	            /* Determine how many columns are gone to the left and right */
                /* the top aliens are: 1,6,11,16,21,26,31,36,41,46,51 */
	            for (j=1;j<55;j++) { //check aliens for left column
	                if (entity[j].active == 1) break;
                }     
                leftoffset = (j/5+1)*50;    
                
	            for (j=55;j>0;j--) { //check right column
	                if (entity[j].active == 1) break;
                }
                rightoffset = (11-(j/5+1))*50;
                
                /* Reverse direction and move aliens one line down */

               		if (self->active == 1) 
		            {
              			self->action(); //needs to be before direction change for 54 alien
                    }   
	
	            if (self == &entity[firstalien+54]) {
                     //playSound(HEARTBEEP);   
                     if (self->x > SCREEN_WIDTH+rightoffset-50) 
	                 {
                            if (aliendirection == 1) {
                                                  if (entity[i_ufo].active == 0) addUFO(30, 26);
                                                  alienverticaloffset=alienverticaloffset+2;  }
                            aliendirection=-1;
                     } else if (self->x < 560-leftoffset)  
                     {
                            if (aliendirection == -1) {
                                                   alienverticaloffset=alienverticaloffset+2;  }
                            aliendirection=1;
                     }
                     if (self->active == 0) //the first+54 one is inactive - need update position
                        self->x = self->x + (2+alienspeed)*aliendirection;
               } //firstalien +54    

            } //aliendelay

        } else { //not alien entities - not moveALIEN
              		if (self->active == 1)
		            {
              			self->action();
               		}
        } //moveALIEN    
	} //for loop
}

void drawEntities()
{
	int i;
	
	/* Loop through the entities and perform their action */
	
	for (i=0;i<MAX_ENTITIES;i++)
	{
		self = &entity[i];
		
		if (self->active == 1)
		{
			self->draw();
		}
	}
}

void drawStandardEntity()
{
    if (self->action == &moveALIEN) {
    drawSprite(self, self->x, self->y + alienverticaloffset);                 
    } else {
    drawSprite(self, self->x, self->y);
    }
}

/* **************************************************************************************************** */
void drawSprite(Entity *self, int x, int y)
{
	SDL_Rect dest;
	SDL_Rect pick;
	
    pick.x=self->pick_x;
    pick.y=self->pick_y;
    pick.w=self->pick_w;
    pick.h=self->pick_h;
	
	/* Set the blitting rectangle to the size of the src image */
	
	dest.x = x;
	dest.y = y;
	dest.w = self->pick_w;
	dest.h = self->pick_h;
	
	/* Blit the entire image onto the screen at coordinates x and y */
	
	SDL_BlitSurface(bmpsprites, &pick, game.screen, &dest);
}

/* **************************************************************************************************** */
void loadSound(int index, char *name)
{
	
	#ifdef soundsupport
	/* Load the sound specified by the filename */
	
	if (index < 0 || index >= MAX_SOUNDS)
	{
		printf("Invalid index for sound! Index: %d Maximum: %d\n", index, MAX_SOUNDS);
		
		exit(1);
	}
	
	sound[index].effect = Mix_LoadWAV(name);

	if (sound[index].effect == NULL)
	{
		printf("Failed to load sound %s\n", name);
		
		exit(1);
	}
	#endif
}


void playSound(int index)
{
     #ifdef soundsupport
	/* Play the sound on the first free channel and only play it once */
	
	if (index < 0 || index >= MAX_SOUNDS)
	{
		printf("Invalid index for sound! Index: %d Maximum: %d\n", index, MAX_SOUNDS);
		
		exit(1);
	}
	
	Mix_PlayChannel(-1, sound[index].effect, 0);
	#endif
}

void loadAllSounds()
{
    #ifdef soundsupport
	loadSound(BULLET_SOUND, "shot.wav");
	//loadSound(HEARTBEEP, "heart-beep.wav");
	loadSound(ALIENHIT, "alien-hit.wav");
	loadSound(UFO, "ufo.wav");
	loadSound(UFOHIT, "ufo-hit.wav");
	loadSound(PLAYERHIT, "player-hit.wav");        	
	loadSound(ALIENSHOT, "alien-shot.wav");        	
	#endif
} 

void freeSounds()
{
	#ifdef soundsupport
	int i;
	
	for (i=0;i<MAX_SOUNDS;i++)
	{
		if (sound[i].effect != NULL)
		{
			Mix_FreeChunk(sound[i].effect);
		}
	}
	#endif
}

/* **************************************************************************************************** */

void addShelter(int x, int y)
{
int is,js;
     
  for (js=0;js<4;js++) {
    for (is=0;is<6;is++) {
    
    //skip black arc at bottom of shelter - swallows bullets
    if ((is==2) && (js==3)) continue;
    if ((is==3) && (js==3)) continue;
     
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for a Shelter!\n");
		
		return;
	}

	entity[i].x = x+is*9;
	entity[i].y = y+js*10;
	entity[i].action = &moveShelter;
	entity[i].draw = &drawStandardEntity;
	entity[i].type = TYPE_ENEMY; 
    entity[i].pick_x=19+is*9;
    entity[i].pick_y=214+js*10;
    entity[i].pick_w=9;
    entity[i].pick_h=10;           
    entity[i].sprite_type=SHELTER_SPRITE;

    } //for is
   } //for js

}

static void moveShelter(void)
{
	/* Do nothing, just sit there */
}

/* **************************************************************************************************** */


void addALIEN(int type, int x, int y)
{
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for an ALIEN!\n");
		
		return;
	}
	
	if (firstalien == 0) firstalien = i;
	
	entity[i].x = x;
	entity[i].y = y;
	entity[i].action = &moveALIEN;
	entity[i].draw = &drawStandardEntity;
	entity[i].type = TYPE_ENEMY;
	
	switch (type)
    {
           case TOP_ALIEN1:
                entity[i].pick_x=38;
                entity[i].pick_y=74;
                entity[i].pick_w=26;
                entity[i].pick_h=25;
                entity[i].sprite_type=TOP_ALIEN1;
                break;
                
           case TOP_ALIEN2:
                entity[i].pick_x=128;
                entity[i].pick_y=75;
                entity[i].pick_w=26;
                entity[i].pick_h=25;           
                entity[i].sprite_type=TOP_ALIEN2;
                break;     

           case MIDDLE_ALIEN1:
                entity[i].pick_x=29;
                entity[i].pick_y=121;
                entity[i].pick_w=35;
                entity[i].pick_h=26;           
                entity[i].sprite_type=MIDDLE_ALIEN1;
                break;
                
           case MIDDLE_ALIEN2:
                entity[i].pick_x=124;
                entity[i].pick_y=121;
                entity[i].pick_w=35;
                entity[i].pick_h=26;           
                entity[i].sprite_type=MIDDLE_ALIEN2;
                break;     

           case BOTTOM_ALIEN1:
                entity[i].pick_x=28;
                entity[i].pick_y=166;
                entity[i].pick_w=38;
                entity[i].pick_h=26;           
                entity[i].sprite_type=BOTTOM_ALIEN1;                
                break;
                
           case BOTTOM_ALIEN2:
                entity[i].pick_x=123;
                entity[i].pick_y=166;
                entity[i].pick_w=39;
                entity[i].pick_h=26;           
                entity[i].sprite_type=BOTTOM_ALIEN2;                
                break;     
	}
}

static void moveALIEN(void)
{

	/* Move the alien across the screen */
	
	self->x = self->x + (2+alienspeed)*aliendirection;
	
	switch (self->pick_x) {
           case 38:
                self->pick_x=128;
                break;
           case 128:
                self->pick_x=38;
                break;
           case 29:
                self->pick_x=124;
                break;
           case 124:
                self->pick_x=29;
                break;
           case 28:
                self->pick_x=123;
                break;
           case 123:
                self->pick_x=28;
                break; 
           }
	
}

/* **************************************************************************************************** */
int collision(int, int, int, int, int, int, int, int);

void doCollisions()
{
	int i, j;
	int x_tmp=0;
	static long int oldscore=0;

	/* Check each entity against the rest, skipping over inactive ones */

	for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == 0)
		{
			continue;
		}

		for (j=0;j<MAX_ENTITIES;j++)
		{
			/* Don't collide with yourself, inactive entities or entities of the same type */
			
			if (i == j || entity[j].active == 0 || entity[j].type == entity[i].type)
			{
				continue;
			}
			
			/* Test the collision */

			if (collision(entity[i].x, entity[i].y, entity[i].pick_w, entity[i].pick_h, entity[j].x, entity[j].y, entity[j].pick_w, entity[j].pick_h) == 1)
			{
                           
				/* If a collision occured, remove both Entities */
				
				entity[j].active = 0;
				
				entity[i].active = 0;
				
                /* Add points to the score */			
                
                if (entity[j].action == moveALIEN || entity[i].action == moveALIEN) //alien involved?
				{
                playSound(ALIENHIT);                                        
                if (entity[j].action == moveALIEN) {
                                     x_tmp=entity[j].pick_x;
                                     entity[j].pick_x=27;
                                     entity[j].pick_y=273;
                                     entity[j].pick_w=36;
                                     entity[j].pick_h=30;           
                                     self = &entity[j];
                                     drawSprite(self, self->x, self->y + alienverticaloffset);
                                     SDL_Flip(game.screen);
                                     }
                if (entity[i].action == moveALIEN) {
                                     x_tmp=entity[i].pick_x;
                                     entity[i].pick_x=27;
                                     entity[i].pick_y=273;
                                     entity[i].pick_w=36;
                                     entity[i].pick_h=30;           
                                     self = &entity[i];
                                     drawSprite(self, self->x, self->y + alienverticaloffset);
               	                     SDL_Flip(game.screen);
                                     }
                
                switch (x_tmp) {
                       case 38:
                       case 128: //TOP alien
                            game.score += 30;
                            break;
                       case 29:
                       case 124: //MIDDLE alien
                            game.score += 20;
                            break;
                       case 28:
                       case 123: //BOTTOM alien
                            game.score += 10;
                            break;
                       }
                
                } else if (entity[j].action == moveUFO || entity[i].action == moveUFO) //UFO hit?
				{
                            #ifdef soundsupport
							Mix_HaltChannel(ufo_channel);
							#endif
                            playSound(UFOHIT);
                            //reuse x_tmp to generate random number                
                            int x_tmp = rand() % 6;           
                            x_tmp++; //if 0
                            game.score += x_tmp*50; //add random from 50-300 - 1-6*50
                }//moveALIEN
				break; //break for j loop
				
            //collision with player /laser base?
            } else if (entity[j].action == moveStandardBomb) {
                if (collision(entity[j].x, entity[j].y, entity[j].pick_w, entity[j].pick_h, player.x, player.y, player.pick_w, player.pick_h) == 1) {
                   playSound(PLAYERHIT);                        
                   //convert the bomb entity into a destroyed laser base
                   entity[j].active = 0;  //don't show again
                   entity[j].x=player.x;
                   entity[j].y=player.y;
				   entity[j].pick_x=115;
                   entity[j].pick_y=278;
                   entity[j].pick_w=50;
                   entity[j].pick_h=25;           
                   self = &entity[j];
                   drawSprite(self, self->x-5, self->y);
                   SDL_Flip(game.screen);
				   mdelay(1000); 
                   game.lives--;
                   break; //break for j loop
                   }
			} //if collision
		} //for j loop
	} // for i loop

    /* Determine how many aliens are left to increase speed and reinit if zero aliens left*/
    aliensleft=0;
    for (j=1;j<56;j++) {
        if (entity[j].active == 1) aliensleft++; 
    }
    alienspeed=1+(55-aliensleft)/10;

	if ((game.score-oldscore) >1500){
       game.lives++;
       oldscore=(game.score/100)*100;  //round to 100
    }            
}

/* Very standard 2D collision detection routine */

int collision(int x0, int y0, int w0, int h0, int x2, int y2, int w1, int h1)
{
	int x1 = x0 + w0;
	int y1 = y0 + h0;

	int x3 = x2 + w1;
	int y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

/* **************************************************************************************************** */
static void moveStandardBullet(void);

void addBullet(int x, int y)
{
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for a bullet!\n");
		
		return;
	}

	entity[i].x = x;
	entity[i].y = y;
	entity[i].action = &moveStandardBullet;
	entity[i].draw = &drawStandardEntity;
	entity[i].type = TYPE_BULLET;
    entity[i].pick_x=139;
    entity[i].pick_y=243;
    entity[i].pick_w=3;
    entity[i].pick_h=12;
    entity[i].sprite_type=BULLET_SPRITE;

	/* Play a sound when the shot is fired */
	
	playSound(BULLET_SOUND);

}

static void moveStandardBullet()
{
	/* Move the bullet across the screen */
	
	self->y = self->y - BULLET_SPEED;
	
	/* Kill the bullet if it moves off the screen */
	
	if (self->y <= 0) //top of screen reached
	{
		self->active = 0;
	}
}

int checkifnobullet(void)
{
int i;
for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == 0)
		{
			continue;
		}
        if (entity[i].action == moveStandardBullet) return -1;
    }
    return 0;   

}

/* **************************************************************************************************** */
void addBomb(int x, int y)
{
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for a Bomb!\n");
		
		return;
	}

	entity[i].x = x;
	entity[i].y = y;
	entity[i].action = &moveStandardBomb;
	entity[i].draw = &drawStandardEntity;
	entity[i].type = TYPE_BULLET;
    entity[i].pick_x=138;
    entity[i].pick_y=212;
    entity[i].pick_w=6;
    entity[i].pick_h=15;
    entity[i].sprite_type=BOMB_SPRITE;

	/* Play a sound when the shot is fired */
	
	playSound(ALIENSHOT);

}

static void moveStandardBomb(void)
{
	/* Move the Bomb across the screen */
	
	self->y = self->y + BOMB_SPEED;
	
	/* Kill the Bomb if it moves off the screen */
	
	if (self->y >= SCREEN_HEIGHT) //bottom of screen reached
	{
		self->active = 0;
	}
}

int checkifnobomb(void)
{
int i;
for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == 0)
		{
			continue;
		}
        if (entity[i].action == moveStandardBomb) return -1;
    }
    return 0;   

}

void calcnextbombposition(int *bombstart_x,int *bombstart_y) {
/* the top aliens are: 1,6,11,16,21,26,31,36, 41,46,51 */     
int j, nj, currentalienoffset;
int bombcolumn = 0;

if (aliensleft<1) return;

//find random column which is not empty
while (aliensleft>0) {
      bombcolumn = rand() % 11;           
      bombcolumn++; //if 0
      //is column empty?
      for (j=0;j<5;j++){
          nj=j+(bombcolumn-1)*5+1;
          if (entity[nj].active == 1) break; //valid column found - exit for
          }    
     if (entity[nj].active == 1) break; //valid column found - exit while
} //while

for (j=4;j>-1;j--){ //just one column - bottom up
     nj=j+(bombcolumn-1)*5+1; //see top aliens above   
     if (entity[nj].active == 0) {
         continue;
     } else {
         j++; //we need the empty one       
         break;
     }
}

*bombstart_y =topline + j*linedist -10 + alienverticaloffset; //reduce last linedist by 10 to start shot from alien    
overrunline=*bombstart_y;

currentalienoffset=(lineoffset+11*50) - entity[firstalien+54].x; //this is always updated even if active==0
*bombstart_x=bombcolumn*50 -currentalienoffset +25; //currentalienoffset can be negative! - 25 for middle of alien

}
/* **************************************************************************************************** */
void addUFO(int x, int y)
{
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for a UFO!\n");
		
		return;
	}
	
	i_ufo = i;
	
	entity[i].x = x;
	entity[i].y = y;
	entity[i].action = &moveUFO;
	entity[i].draw = &drawStandardEntity;
	entity[i].type = TYPE_ENEMY;
    entity[i].pick_x=26;
    entity[i].pick_y=25;
    entity[i].pick_w=49;
    entity[i].pick_h=23;
    entity[i].sprite_type=0;
    
    #ifdef soundsupport
   	ufo_channel=Mix_PlayChannel(-1, sound[UFO].effect, 100);
	#endif
    
}

static void moveUFO()
{
	/* Move the UFO across the screen */
	
	self->x += UFO_SPEED;
	
	/* Kill the UFO if it moves off the screen */
	
	if (self->x >= SCREEN_WIDTH)
	{
		self->active = 0;
		#ifdef soundsupport
		Mix_HaltChannel(ufo_channel);
		#endif
	}
}

/* **************************************************************************************************** */

void DrawChar(SDL_Surface *screen, SDL_Surface *bmpfont, int X, int Y, int width, int height, int asciicode)
{

     SDL_Rect pick;

     pick.x=(asciicode % 32)*width;
     pick.y=(asciicode / 32)*height;
     pick.w=width;
     pick.h=height;
     area.x=X;
     area.y=Y;
     area.w=width;
     area.h=height;
                
     SDL_BlitSurface(bmpfont,&pick,screen,&area);
}

/* **************************************************************************************************** */
void Drawstring(SDL_Surface *screen, int X, int Y, char text[]) {
  int i=0;
  int asciicode;
  area.x=X; 
  area.y=Y; 

  for (i=0;i<55;i++){
      asciicode=text[i];    
      if (asciicode == 0 ) {
                    break;
      }
      DrawChar(game.screen, bmpfont, area.x, area.y, 8, 16, asciicode);    
      area.x=area.x+8;
      }
}

/* **************************************************************************************************** */
void mdelay(int millisecs) {
	int msecs_start;
	msecs_start = getmyticks();
	while (1) 
	if (getmyticks()>=(msecs_start+millisecs)) break;
	
}
/* **************************************************************************************************** */
int getmyticks(void)
{
int ticks;
#ifdef wiisupport
ticks = ticks_to_millisecs(gettime());
#else
ticks = SDL_GetTicks(); 
#endif
return (ticks);
}
/* **************************************************************************************************** */
void delay(unsigned int frameLimit)
{
	unsigned int ticks = getmyticks(); 

	if (frameLimit < ticks)
	{
		return;
	}
	
	if (frameLimit > ticks + 30)
	{
		mdelay(30); 
	}
	else
	{
		mdelay(frameLimit - ticks);
	}
}
/* **************************************************************************************************** */
/* **************************************************************************************************** */

