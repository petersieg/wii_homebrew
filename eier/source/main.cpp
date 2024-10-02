/******************************************************************************************
 *
 * Eier
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "main.cpp"
 *
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "constants.hpp"
#include "graphics.hpp"

#define MISSMAX 5
int MISSCNT = 0;

class Playfield;

SDL_Surface *MYSDLSCREEN;
Playfield *pf;
Font *font;

Uint32 videoflags = 0, 
	bitplanes = 0,
	fullscreen = 0;

bool VERBOSE=false; void verbose( std::string info ) { if (VERBOSE) std::cout << info << std::endl << std::flush; }

//-----------------------------------------------------------------------------------------
class Gametimer 
//-----------------------------------------------------------------------------------------
{
public:

	Gametimer( Uint32 ticks ) : next_time( SDL_GetTicks() + ticks ), ticks(ticks) {}

	void init()
       	{
	  	next_time = SDL_GetTicks() + ticks;
	}
		
	void delay()
	{
		int delay = next_time-SDL_GetTicks();

		if ( delay > 0 ) 
			SDL_Delay( (Uint32)delay );

		// prevent from speedup! 
		if ( delay < 0 )
			next_time=SDL_GetTicks();

        	next_time += ticks;
	}
			      
private:

	Uint32 next_time, ticks;
};

//-----------------------------------------------------------------------------------------
class Marie
//-----------------------------------------------------------------------------------------
{
public:

	Marie()
	{
		marie[0] = new Sprite( "sd:/apps/eier/gfx/sm1.bmp" );
		marie[1] = new Sprite( "sd:/apps/eier/gfx/sm2.bmp" );
		marie[2] = new Sprite( "sd:/apps/eier/gfx/sm3.bmp" );
		marie[3] = new Sprite( "sd:/apps/eier/gfx/sm4.bmp" );
		marie[4] = new Sprite( "sd:/apps/eier/gfx/sm5.bmp" );
		marie[5] = new Sprite( "sd:/apps/eier/gfx/sm6.bmp" );

		marie_foreground[0] = new Sprite( "sd:/apps/eier/gfx/sm1fg.bmp" );
		marie_foreground[1] = new Sprite( "sd:/apps/eier/gfx/sm2fg.bmp" );
		marie_foreground[2] = new Sprite( "sd:/apps/eier/gfx/sm3fg.bmp" );
		marie_foreground[3] = new Sprite( "sd:/apps/eier/gfx/sm4fg.bmp" );
		marie_foreground[4] = new Sprite( "sd:/apps/eier/gfx/sm5fg.bmp" );
		marie_foreground[5] = new Sprite( "sd:/apps/eier/gfx/sm6fg.bmp" );
	}

	~Marie() 
	{
		for ( int i=0; i<MAXPOSITIONS; i++ ) delete marie[i];

		for ( int i=0; i<MAXPOSITIONS; i++ ) delete marie_foreground[i];
	}

	void reset()
	{
		score = 0;
		lives = MAXMISSES;
		position = 3;
		bonus = 0;
		setMaxLives( 1 );
	}
	
	bool hasFailed() const 
	{
		return lives <= 0;
	}
	
	void setMaxLives( int level )
	{
		int m = maxlives;

		maxlives = level + 2;
		
		if ( maxlives > 9 ) maxlives = 9;
		
		if ( maxlives > m ) lives = maxlives;
	}

	int getMaxLives() const
	{
		return maxlives;
	}

	int getLives() const
	{
		return lives;
	}

	int getScore() const
	{
		return score;
	}
	
	int getPosition() const
	{
		return position;
	}
	
	void miss()
	{
		//if ( --lives < 0 ) lives = 0;
		if (++MISSCNT > MISSMAX) {
			if ( --lives < 0 ) lives = 0;
			MISSCNT = 0;
		}		
	}

	void addScore( int score )
	{	
		if ( score >= 5 ) {
			
			if ( bonus++ ==5 ) {
				bonus = 0;
				
				if ( lives ++ >= maxlives ) lives = maxlives;
			}
		}
		this->score += score;
	}

	void move( int direction ) 
	{
		position += direction;
		
		if ( position < 0 ) position = 0;

		if ( position >= MAXPOSITIONS ) position = MAXPOSITIONS - 1;
	}
	
	void draw( bool foreground = false )
	{
		static int x[ MAXPOSITIONS ] = { 110, 202, 306, 328, 426, 522 }; 
		static int x_fg[ MAXPOSITIONS ] = { -46, -40, -48, +37, +40, +46 }; 
		static int y_fg[ MAXPOSITIONS ] = { -3, 1, 2, 3, 3, 3 }; 
		static int y = 314;
		
		if ( foreground ) {
			marie_foreground[ position ]->setPos( x[ position ] + x_fg[ position ],  y + y_fg[ position ] );
			marie_foreground[ position ]->draw();
		} else {
			marie[ position ]->setPos( x[ position ],  y );
			marie[ position ]->draw();
		}	
	}
		
private:
	
	int position;
	int lives;
	int score;
	int bonus;
	int maxlives;

	Sprite *marie[ MAXPOSITIONS ];
	Sprite *marie_foreground[ MAXPOSITIONS ];
};

//-----------------------------------------------------------------------------------------
class Egg
//-----------------------------------------------------------------------------------------
{
public:

	Egg()
	:	
		egg( NULL ),
		killed( -1 )
	{
		ugly = new Sprite( "sd:/apps/eier/gfx/egg_ugly.bmp" );
		hit = new Sprite( "sd:/apps/eier/gfx/egg_hit.bmp" );
		miss = new Sprite( "sd:/apps/eier/gfx/egg_miss.bmp" );
		border = new Sprite( "sd:/apps/eier/gfx/egg_border.bmp" );
		broken = new Sprite( "sd:/apps/eier/gfx/egg_broken.bmp" );
		stinky = new Sprite( "sd:/apps/eier/gfx/stink.bmp", 8 );
		stinky->setRepeatmode( false );
		stinky->setFramerate( 1 );

		init( 1 );
	}

	~Egg() 
	{
		delete ugly;
		delete egg;
		delete hit;
		delete miss;
		delete border;
		delete broken;
	}
	
	void init( int level )
	{
		if ( egg != NULL ) delete egg;

		mode = (Eggtype)RANDOM( 2, 0 );	
		
		switch ( mode ) {
			
			case EGG_GOLD:
				egg = new Sprite( "sd:/apps/eier/gfx/egg_gold.bmp" );
				speed = level + 2;
				break;

			default:
				
				// Eastereggs... =)
				switch ( (int)RANDOM( 3, 0 ) ) {
				
					case 0:
						egg = new Sprite( "sd:/apps/eier/gfx/egg_red.bmp" );
						break;
						
					case 1:
						egg = new Sprite( "sd:/apps/eier/gfx/egg_yellow.bmp" );
						break;
						
					case 2:
						egg = new Sprite( "sd:/apps/eier/gfx/egg_blue.bmp" );
						break;
						
					case 3:
						egg = new Sprite( "sd:/apps/eier/gfx/egg_green.bmp" );
						break;
						
					default:
						egg = new Sprite( "sd:/apps/eier/gfx/egg_white.bmp" );
						break;					
				}
				
				speed = level + 1;
		}

		x = y = 0;
		line = (int)RANDOM( MAXPOSITIONS-1, 0);
		pos_start = 0;
		pos_height = (int)RANDOM ( SCREENHEIGHT / 3 * 2, SCREENHEIGHT / 2 );
		
		if ( line < 2 ) pos_direction = -1;
		
		else if ( line > 3 ) pos_direction = 1;
		
		else pos_direction = ( (int)RANDOM( 1, 0 ) == 1 ) ? -1 : 1;
	}
	
	// kill() returns
	// -1 if an ugly egg was caught 
	//    or a normal egg was missed.
	// 10 if a normal egg and
	// 50 if a gold egg was caught!.
	int kill()
	{
		killed = 15;
		missed = false;
		
		if ( mode == EGG_NORMAL ) {
			
			if ( y >= MISSEDHEIGHT ) { missed = true; return -1; }
			else { missed = false; return 1; }
		}
		
		if ( mode == EGG_UGLY ) {

			stinky->resetFrames();
			
			if ( y >= MISSEDHEIGHT ) { missed = false; return 0; }
			else { missed = true; return -1; }
		}
		
		if ( mode == EGG_GOLD ) {

			if ( y >= MISSEDHEIGHT ) { missed = false; return 0; }
			else { missed = false; return 5; }
 		}

		return 0;
	}

	int getLine() const
	{
		return line;
	}
	
	// move() returns:
	// -1 if Egg was missed
	//  1 if Egg is in catch-position
	//  0 else
	int move( int level )
	{
		if ( --killed > 0 ) return 0;
		
		if ( killed == 0 ) {
			init( level );
		}
		
		static int linepos[ MAXPOSITIONS ] = { 63, 160, 259, 365, 466, 568 };

		killed = -1;
		
		pos_start += (double)speed / 2;
		x = linepos[ line ] + ( - pos_direction * ( ( 180-45 ) * 5 ) ) + pos_direction * (int)( pos_start * 5 );
		y = (int)( CATCHHEIGHTTOP + 5 - sin ( ( (double)pos_start + 45 ) * PI / 180 ) * pos_height );
		
		if ( y >=MISSEDHEIGHT ) return -1;

		if ( y >=CATCHHEIGHTTOP && y <=CATCHHEIGHTBOT ) return 1;
		
		return 0;
	
	}
	
	void draw()
	{
		if ( killed > 0 ) {
			
			if ( mode == EGG_NORMAL && missed ) {
				broken->setPos( x, y );
				broken->setAlpha( 120 + killed * 5 );
				broken->draw();						

			} else if ( mode == EGG_UGLY && missed ) {
				static int linepos[ MAXPOSITIONS ] = { 63, 160, 259, 365, 466, 568 };
		
				stinky->setPos( linepos[ line ], y + 5 );
				stinky->setAlpha( killed * 16 );
				stinky->draw();						

			} else {
				egg->setPos( x, y );
				egg->setAlpha( 255-150 + killed * 10 );
				egg->draw();
				
				Sprite *s = ( missed ) ? miss : hit;
				
				s->setAlpha( 60 + killed * 10 );
				s->setPos( x, y );
				s->draw();
				border->setAlpha( 35 );
				border->setPos( x, y );
				border->draw();
			}
		} else {

			if ( mode == EGG_UGLY ) {
			 	egg->setAlpha( 255 - (int)pos_start / 2 );
				egg->setPos( x, y );
				egg->draw();
				
				if ( pos_start > 70 ) {
				 	ugly->setAlpha( (int)( ( pos_start - 70 ) * 3.2 ) );
					ugly->setPos( x, y );
					ugly->draw();
				}

				border->setAlpha( 35 );
				border->setPos( x, y );
				border->draw();
			} else {
			 	egg->setAlpha( 255 );
				egg->setPos( x, y );
				egg->draw();
				border->setAlpha( 35 );
				border->setPos( x, y );
				border->draw();
			}
		}

	}
		
private:

	enum Eggtype { EGG_NORMAL, EGG_GOLD, EGG_UGLY } mode;

	int line;
	int x, y;
	int speed;
	double pos_start;
	int pos_height,
	    pos_direction;
	    	
	Sprite *egg,
		*miss,
		*hit,
		*border,
		*broken,
		*ugly,
		*stinky;

	int killed;
	bool missed;
};

//-----------------------------------------------------------------------------------------
class Playfield
//-----------------------------------------------------------------------------------------
{
public:

	Playfield() 
	:
		is_done( false ),
		game_running( false ),
		highscore( HIGHSCORE )
	{
		verbose( "Initializing Playfield" ); 

		for ( int i=0; i<MAXEGGS; i++ ) eggs[i] = new Egg();

		marie = new Marie();
		bg = new Sprite ( "sd:/apps/eier/gfx/bg.bmp" );
		
		init_Game();
		
		// Get saved highscore
		std::ifstream highfile( HIGHNAME );
		
		if ( highfile )	highfile >> highscore;
	}
	
	~Playfield()
	{
		verbose( "Deleting Playfield" );

		for ( int i=0; i<MAXEGGS; i++ ) delete eggs[i];

		delete marie;
		delete bg;

		// Get saved highscore
		std::ofstream highfile( HIGHNAME );
		
		if ( highfile )	highfile << highscore;
	}

	bool is_Done() const
	{ 
		return is_done;
	}
		
	void draw()
	{
		draw_Background();
		
		marie->draw();
		
		for ( int i=0; i<MAXEGGS; i++ ) eggs[i]->draw();

		marie->draw( true );

		draw_Text();

		SDL_Flip(MYSDLSCREEN);	
	}
	
	void play() 
	{
		verbose( "Starting Gameloop" ); 

		Gametimer timer = Gametimer( TICK_INTERVAL_GAME );

		init_Game();

		bool keypressed = false;
		SDL_Event event;
		
		do {		
			
			while ( SDL_PollEvent( &event ) ) {
			
				switch (event.type) {
				
					case SDL_QUIT:
						is_done = true;
						break;
					
					case SDL_KEYUP:
						keypressed = false;
						break;
						
				}
			}
					
			Uint8 *keys = SDL_GetKeyState(NULL);

			// Ingame-Tasten
			if (!keypressed) {

				if( keys[KEY_QUIT] ) {
					is_done = true;
				}

				if( keys[KEY_NEWGAME] && !keypressed  ) {
					init_Game();
					game_running = true;
					keypressed = true;
				}

				if ( keys[KEY_LEFT] && !keypressed && game_running ) {
					marie->move( -1 );
					keypressed = true;
				}
		
				if ( keys[KEY_RIGHT] && !keypressed && game_running ) {
					marie->move( 1 );
					keypressed = true;
				}

			}
			
			marie->setMaxLives( level );

			for ( int i=0; i<MAXEGGS; i++ ) {

				switch ( eggs[i]->move( level ) ) {
				
					case 1:
						if ( eggs[i]->getLine() == marie->getPosition() ) {
							int s = eggs[i]->kill();
							
							if ( s >= 0 && game_running ) marie->addScore(s);
							
							if ( s < 0 && game_running ) marie->miss();								
						}
						
						break;

					case -1:
						if ( eggs[i]->kill() < 0 & game_running ) marie->miss();

						break;

					default:
						;
				}
			}
			
			draw();
			
			int ms = marie->getScore();
			
			highscore = MAX( ms, highscore );
			level = get_Level( ms );
			
			if ( marie->hasFailed() ) game_running = false;
			
			if( keys[KEY_SCREENSHOT] && !keypressed ) {
				static int screenshotid = 0;

				if ( screenshotid++ > 25 )
					screenshotid = 25;

				char filename[] = "screenshot__.bmp";
				filename[11] = (char)(screenshotid+96);

				SDL_SaveBMP( MYSDLSCREEN, filename );

				keypressed = true;
			}
			
			timer.delay();	

		} while ( !is_done );
	}
	
private:

	bool is_done, game_running;
	int level, last_level, level_cnt;
	int highscore;
	Egg *eggs[ MAXEGGS ];
	Marie *marie;
	Sprite *bg;
		
	int get_Level( int score ) const
	{
		int e = score / 100 + 1;
		
		return ( e > MAXLEVEL ) ? MAXLEVEL : e;	
	}
	
	void draw_Text()
	{
		std::ostringstream s, l;
		
		s << "EIER " << marie->getScore();
		l << "LEVEL " << level;
		
		font->print( 1 * 16, font->getLineHeight() * 1, s.str(), ( ( game_running ) ? 210 : 100 ) );
		font->print( 1 * 16, font->getLineHeight() * 2, l.str(), ( ( game_running ) ? 210 : 100 ) );

		int x = 1 * 16 + 7;
		
		static Sprite *eg1 = new Sprite( "sd:/apps/eier/gfx/egg_1.bmp" );
		static Sprite *eg2 = new Sprite( "sd:/apps/eier/gfx/egg_0.bmp" );
		
		for ( int i=0; i<marie->getLives(); i++ ) { 
			eg1->setAlpha( ( ( game_running ) ? 210-20 : 100-20 ) );
			eg1->setPos( x, font->getLineHeight() * 3 + 7 );
			eg1->draw();
			x += 20;
		}

		for ( int i=0; i<marie->getMaxLives() - marie->getLives(); i++ ) { 
			eg2->setAlpha( ( ( game_running ) ? 210-20 : 100-20 ) );
			eg2->setPos( x, font->getLineHeight() * 3 + 7 );
			eg2->draw();
			x += 20;
		}

		if ( !game_running ) {
			std::ostringstream s[ 20 ];
		
			s[0] << "SARAH>MARIES EIERJAGD";
			s[1] << "";
			s[2] << "SARAH>MARIE HAT EINEN";
			s[3] << "FERIENJOB AUF DER EIERFARM:";
			s[4] << "";
			s[5] << "SIE MUSS ALLE GEFAERBTE";
			s[6] << "EIER IN KOERBEN SAMMELN:";
			s[7] << "DABEI DARF SIE ABER";
			s[8] << "KEINE FAULEN EIER FANGEN:";
			s[9] << "";
			s[10] << "MACHT SIE BEI DER ARBEIT";
			s[11] << "EINEN FEHLER; KANN SIE";
			s[12] << "DEN SCHADEN MIT 5 GOLDENEN";
			s[13] << "EIERN WIEDER BEGLEICHEN:";
			s[14] << "";
			s[15] << "DER BESTE EIERFAENGER";
			s[16] << "HAT " << highscore << " EIER GEFANGEN:";
			s[17] << "";
			s[18] << "DRUECKE F1 UM IHN ZU";
			s[19] << "UEBERTRUMPFEN:::";
		
			for ( int i=0; i<20; i++ ) {
				font->print( ( SCREENWIDTH - 22 * 16 ) / 2, font->getLineHeight() * ( 1 + i ), s[i].str(), 210 );
			}
		} else {

			if ( last_level != level ) {
				level_cnt = 100;
				last_level = level;
			}
			
			if ( level_cnt > 0 ) {
				level_cnt--;
				std::ostringstream l1, l2;
				
				l1 << "TOLLE ARBEIT;" << level;
				l2 << "JETZT WEITER IN LEVEL " << level;
				font->print( ( SCREENWIDTH - font->getWidth( l1.str() ) ) / 2, SCREENHEIGHT / 2 - font->getLineHeight() * 2, l1.str(), level_cnt * 2 );
				font->print( ( SCREENWIDTH - font->getWidth( l2.str() ) ) / 2, SCREENHEIGHT / 2, l2.str(), level_cnt * 2 );
			}
		}
	}

	void draw_Background()
	{		
		bg->setPos( SCREENWIDTH / 2, SCREENHEIGHT / 2 );
		bg->draw();
	}

	void init_Game()
	{
		verbose ( "Initializing Game" );

		is_done = false;
		level = 1;
		last_level = level;
		level_cnt = 0;
		marie->reset();

		for ( int i=0; i<MAXEGGS; i++ ) eggs[i]->init( level );
	} 	
};

//-----------------------------------------------------------------------------------------
int main( int argc, char *argv[] )
//-----------------------------------------------------------------------------------------
{
	// Check for Options
	const std::string __BLUE   = "\x1b[34m"; 
	const std::string __RED    = "\x1b[31m"; 
	const std::string __NORMAL = "\x1b[0m"; 

	bool get_video_info = false, param_ok;
/*
	for (int i=1; i < argc; i++) {
		param_ok = false;
		char *arg = argv[i];

		// Print out Version and Exit
		if ( arg == (std::string)"--version" || arg == (std::string)"-v" ) {
			std::cout << NAME << " v" << VERSION << std::endl
				<< COPYRIGHT << std::endl
				<< __BLUE << WEBSITE << __NORMAL
				<< std::endl;
			exit(0);
		}		

		// Print out Help and Exit
		if ( arg == (std::string)"--help" || arg == (std::string)"-h" ) {
			std::cout << NAME << " v" << VERSION << std::endl
				<< COPYRIGHT << std::endl
				<< __BLUE << WEBSITE << __NORMAL
				<< std::endl
				<< "Usage: "
				<< argv[0]
				<< " [-h --help|-v --version|-f --fullscreen|--videoinfo|--verbose]" << std::endl
				<< __RED
				<< "[ESCAPE] Quit Game." << std::endl 
				<< "[F1]     New Game." << std::endl			
				<< "\x1b[0m";
			exit(0);
		}
		
		// Print out videoinfo and Exit
		if ( arg == (std::string)"--videoinfo" || arg == (std::string)"-vi" ) {
			get_video_info = true;	
			param_ok = true;
		}

		// Start Game in Fullscreen-mode
		if ( arg == (std::string)"--fullscreen" || arg == (std::string)"-f" ) {
			fullscreen = SDL_FULLSCREEN;
			bitplanes = 16;
			param_ok = true;
		}
		
		// Start Game with verbose-Output
		if ( arg == (std::string)"--verbose" ) {
			VERBOSE = true;
			param_ok = true;
		}
		
		if ( !param_ok ) {
			std::cout << arg << ": Unknown option." << std::endl;
			exit(1);
		}
	}
*/	
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) == -1 ) {
		std::cout << "Can't init SDL: " << SDL_GetError() << std::endl;
	    exit(1);
	}
	
 	atexit(SDL_Quit); 

	const SDL_VideoInfo *video_Info = SDL_GetVideoInfo();

	if (get_video_info) {
		std::cout << "Video Memory size is: " 
			<< ( video_Info->video_mem ) << " KB"
			<< std::endl
			<< "Hardware Surfaces are: "
			<< ( ( video_Info->hw_available ) ? "enabled" : __RED+"disabled"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware blits are: " 
			<< ( ( video_Info->blit_hw ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware colorkeyblits are: " 
			<< ( ( video_Info->blit_hw_CC ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware alphablits are: " 
			<< ( ( video_Info->blit_hw_A ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware blits are: " 
			<< ( ( video_Info->blit_sw ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware colorkeyblits are: " 
			<< ( ( video_Info->blit_sw_CC ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware alphablits are: "
			<< ( ( video_Info->blit_sw_A ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< " " << std::endl;
		exit(0);
	}
	
	// Init SDL
	videoflags |= SDL_ANYFORMAT | fullscreen;
	if ( video_Info->hw_available ) {
		verbose ( "Game is Hardware-accelerated." );

		videoflags |= SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL;
	} else {
		videoflags|= SDL_SWSURFACE;
	}

	#ifndef __WIN32__
	//SDL_WM_SetIcon( IMG_Load( "gfx/logo.png" ), NULL );
	#endif

    	if ( ( MYSDLSCREEN = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, bitplanes, videoflags ) ) == NULL ) {
		std::cout << "Can't set video mode: " << SDL_GetError() << std::endl;
        	exit(1);
    	}
	
    	SDL_WM_SetCaption( "Sarah-Maries Eierjagd", NULL );
	SDL_ShowCursor(SDL_DISABLE);
	//SDL_WM_GrabInput(SDL_GRAB_ON);

	//sound = new Soundset();

	srand(time(NULL));
	
	pf = new Playfield();
	font = new Font();
	
	pf->play();

	delete pf;
	delete font;

	verbose( "Bye..." );
	
    	return 0;
}

