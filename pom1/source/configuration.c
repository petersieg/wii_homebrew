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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "memory.h"

static char *_romdir;

void freeRomDirectory(void)
{
	free(_romdir);
}

void setRomDirectory(const char *romdir)
{
	if (_romdir)
		free(_romdir);

	_romdir = strdup(romdir);
}

const char *getRomDirectory(void)
{
	return _romdir;
}

void loadConfiguration(void)
{
}

void saveConfiguration(void)
{
}
