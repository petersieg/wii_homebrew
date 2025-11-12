This is the source code for the SDL Space Invaders game.

It consists of the main file spaceinv.c, the include file spaceinv.inc and the header file spaceinv.h. 
So it does not need separate compilations which simplifies the makefile and makes compiling on 
different platforms easier. You should have SDL.DLL in your path. The BMP and WAV files are loaded by
the program when run. See http://www.libsdl.org/index.php for details about SDL.

The project file is called Project1. For Windows and DOS use DEV-CPP to compile. 
See: http://www.bloodshed.net/devcpp.html . The project file specifies the icon for the Windows version.

For DOS comment out #define soundsupport in spaceinv.c, since the DOS port runs with HXDOS and that 
does not support sound. The necessary HXDOS files are included in the binary release file.
See http://www.japheth.de/hx.html for details about HXDOS.

For the WII uncomment #define WIIsupport in spaceinv.c and compile with devkitpro. 
See http://www.devkitpro.org/ and http://www.wiibrew.org/wiki/Main_Page for details.

20 Dec 2008 Georg Potthast