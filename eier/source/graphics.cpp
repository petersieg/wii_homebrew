/******************************************************************************************
 *
 * Eier
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "graphics.cpp"
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
#include <stdio.h>
#include <cmath>

#include "graphics.hpp"

/******************************************************************************************
 *
 * Font
 *
 ******************************************************************************************/
Font::Font()
{
	verbose( "Initializing Font" );
	
	if ( ( font_image=SDL_LoadBMP( "gfx/font.bmp" ) ) == NULL ) {
		std::cout << "Error in Font: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	SDL_SetColorKey( font_image, 
		SDL_RLEACCEL | SDL_SRCCOLORKEY, 
		SDL_MapRGB( font_image->format, 255, 0, 255 ) );
	font_image = SDL_DisplayFormat(font_image);
}

Font::~Font() 
{
	verbose( "Deleting Font" );

	SDL_FreeSurface(font_image);
}

int Font::getLineHeight() const 
{
	return font_image->h + 4;
}

void Font::print( int x, int y, std::string txt, int alpha ) 
{
	SDL_Rect src, dst;
	
	src.y = 0;
	src.h = dst.h = font_image->h;
	dst.y = y;
	dst.x = x;
	
	SDL_SetAlpha( font_image, SDL_SRCALPHA, alpha );

	for ( int i=0; i < (int)txt.length(); i++ ) {
		int srcp = (int)txt[i]-48;
	
		if ( srcp >= 0 && srcp < 43 ) {
			src.x = srcp * 16;
			src.w = dst.w = 16;

			if ( dst.x > SCREENWIDTH || dst.y > SCREENHEIGHT ) 
				break;

			if ( dst.x + dst.w >= 0 && dst.y + dst.h >= 0 && src.w > 0 )
				SDL_BlitSurface( font_image, &src, MYSDLSCREEN, &dst );

		}
		x += 16;
		dst.x = x;
	}	
}

int Font::getWidth( std::string txt )
{
	return txt.length() * 16;
}

/******************************************************************************************
 *
 * Sprite
 *
 ******************************************************************************************/
Sprite::Sprite( char* filename, int frames ) :
	x( 0 ), 
	y( 0 ),
	alpha( 255 ),
	repeat_mode( true ),
	frames( frames ), 
	actual_frame( 0 ),
	frame_delay( ANIMFRAME ),
	frame_rate( ANIMFRAME )
{
	verbose( "Initializing Sprite: " + std::string(filename) );

	if ( ( sprite_image = SDL_LoadBMP( filename ) ) == NULL ) {
		std::cout << "Error in Sprite: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_SetColorKey( sprite_image, 
		SDL_RLEACCEL | SDL_SRCCOLORKEY, 
		SDL_MapRGB( sprite_image->format, 255, 0, 255 ) );
	sprite_image = SDL_DisplayFormat(sprite_image);

	width = sprite_image->w / frames;
	height = sprite_image->h;
}

Sprite::~Sprite()
{
	verbose( "Deleting Sprite" );
	
	SDL_FreeSurface( sprite_image );
}	

bool Sprite::is_onLastFrame()
{
	return ( actual_frame == frames-1 && frame_delay == 0 ) ? true : false;
}

int Sprite::getWidth()
{
	return width; 
}
	
void Sprite::setFramerate( int rate )
{	
	frame_rate = rate; 
	frame_delay = rate;
}

void Sprite::setRepeatmode( bool mode )
{
	repeat_mode = mode; 
}

void Sprite::resetFrames() 
{
	actual_frame = 0;
}

void Sprite::setPos( int x, int y )
{
	this->x = x;
	this->y = y;
} 

void Sprite::setAlpha( int alpha )
{
	this->alpha = alpha;
} 
	
void Sprite::draw()
{
	rect.x = x - width/2;
	rect.y = y - height/2;
	rect.w = width;
	rect.h = height;
	sprite_rect.x = actual_frame * width;
	sprite_rect.y = 0;
	sprite_rect.w = width;
	sprite_rect.h = height;

	SDL_SetAlpha( sprite_image, SDL_SRCALPHA, alpha);
	SDL_BlitSurface( sprite_image, &sprite_rect, MYSDLSCREEN, &rect );
		
	if ( frame_delay-- < 1 ) {
		frame_delay = frame_rate;

		if ( ++actual_frame >= frames ) {

			if (repeat_mode) 
				actual_frame = 0;
			else 
				actual_frame = frames-1;
		}	
	}
}

void Sprite::putpixel( int x, int y, Uint32 pixel, SDL_Surface *screen )
{
	// NOTE: The surface must be locked before calling this!
	if ( x >= 0 && x < SCREENWIDTH && y >= 0 && y < SCREENHEIGHT) {
		
		int bpp = screen->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to set */
		Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;
	
		switch(bpp) {
			case 1:
				*p = pixel;
				break;
	
			case 2:
				*(Uint16 *)p = pixel;
				break;
	
			case 3:
				if( SDL_BYTEORDER == SDL_BIG_ENDIAN ) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				} else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;
	
			case 4:
				*(Uint32 *)p = pixel;
				break;
		}
	}
}
