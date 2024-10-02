/******************************************************************************************
 *
 * Eier
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "graphics.hpp"
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

#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__

#include <SDL/SDL.h>

#include "constants.hpp"

extern SDL_Surface* MYSDLSCREEN;

/******************************************************************************************
 *
 * Font
 *
 * Font is a class that manages a BMP (Background=0xff00ff) Font.
 *
 ******************************************************************************************/
class Font
{
public:
	Font();
	
	~Font();
	
	void print( int x, int y, std::string txt, int alpha=255 ); 

	int getLineHeight() const;

	int getWidth( std::string txt );
	
private:
	SDL_Surface *font_image;
};

/******************************************************************************************
 *
 * Sprite
 *
 * Sprite manages a BMP (Background=0xff00ff) as a screen sprite
 * If Constructed with frames-parameter, Sprite uses serialised
 * Pictures as Anim with size: Sprite-size / frames;
 *
 ******************************************************************************************/
class Sprite
{
public:	
	Sprite( char* filename, int frames=1 );

	~Sprite();

	bool is_onLastFrame();
	
	int getWidth();
		
	void setFramerate( int rate );

	void setRepeatmode( bool mode );

	void resetFrames();
	
 	void setPos( int x, int y );

	void setAlpha( int a );
		
	void draw();
	
	static void putpixel( int x, int y, Uint32 pixel, SDL_Surface *screen=MYSDLSCREEN );

private:
	SDL_Surface *sprite_image;
	SDL_Rect sprite_rect, rect;
	int x, y, width, height, alpha;
	bool repeat_mode;
	int frames, 
		actual_frame,
		frame_delay,
		frame_rate;
};

#endif

