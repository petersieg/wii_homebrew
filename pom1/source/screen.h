// Pom1 Apple 1 Emulator
// Copyright (C) 2000 Verhille Arnaud
// Copyright (C) 2012 John D. Corrado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef __SCREEN_H__
#define __SCREEN_H__

int loadCharMap(void);
void resetScreen(void);
void setPixelSize(int ps);
int getPixelSize(void);
void setScanlines(int scanlines);
int getScanlines(void);
void setTerminalSpeed(int ts);
int getTerminalSpeed(void);
void redrawScreen(void);
void updateScreen(void);
void drawCharacter(int xPosition, int yPosition, unsigned char r, unsigned char g, unsigned char b, unsigned char characNumber);
void setFullscreen(int fullscreen);
int getFullscreen(void);
void drawCursor(void);
void setBlinkCursor(int blickCursor);
int getBlinkCursor(void);
void setBlockCursor(int blockCursor);
int getBlockCursor(void);
void initScreen(void);

#endif
