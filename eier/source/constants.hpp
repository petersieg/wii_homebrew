/******************************************************************************************
 *
 * Eier
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "constants.cpp"
 * 
 * This file contains important parameters of the game.
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

#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <iostream>

#include <SDL/SDL.h>

//#define __DEBUG__		// Print out Some Debug Information

#define HIGHNAME ".hEierscore"

const std::string NAME	 	= "Sarah-Maries Eierjagd";
const std::string VERSION 	= "0.1";
const std::string COPYRIGHT 	= "Copyright (c) 2005, Patrick Gerdsmeier <patrick@gerdsmeier.net>";
const std::string WEBSITE 	= "http://www.gerdsmeier.net";

const double PI = 3.141592;

// SCREEN
const int SCREENWIDTH		= 640; 
const int SCREENHEIGHT 		= 480;
const int MAXPOSITIONS		= 6;
const int TICK_INTERVAL_GAME 	= 25; 
const int ANIMFRAME		= 3;
const int MAXEGGS		= 2; //3
const int MAXMISSES		= 9; //3
const int MISSEDHEIGHT		= 370;
const int CATCHHEIGHTTOP	= 292;
const int CATCHHEIGHTBOT	= 322;
const int MAXLEVEL		= 6;
const int HIGHSCORE		= 0;

// KEYBOARD. I think this Keyboard functions are Ok:
const int KEY_QUIT 		= SDLK_ESCAPE;
const int KEY_NEWGAME	 	= SDLK_F1;
const int KEY_SCREENSHOT 	= SDLK_F12;
const int KEY_LEFT		= SDLK_LEFT;
const int KEY_RIGHT		= SDLK_RIGHT;

extern void verbose( std::string info );

#define RANDOM( max, min ) ( ( max + 1 - min ) * ( rand() / ( RAND_MAX + 1.0 ) ) + min )

#define MAX( a, b ) ( ( a > b ) ? a : b )

#define SCREAM(t) std::cout << t << std::endl << std::flush;


#endif

