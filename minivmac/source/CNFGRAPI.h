/*
	Configuration options used by platform specific code.

	This file is automatically generated by the build system,
	which tries to know what options are valid in what
	combinations. Avoid changing this file manually unless
	you know what you're doing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LOCAL
#define RomFileName "vMac.ROM"
#else
#define RomFileName "sd:/apps/minivmac/vMac.ROM" /** */
#endif
#define EnableDragDrop 1
#define EnableAltKeysMode 0
#define SwapCommandControl 0
#define VarFullScreen 1
#define WantInitFullScreen 1
#define MayFullScreen 1
#define MayNotFullScreen 1
#define WantInitMagnify 0
#define EnableMagnify 1
#define MyWindowScale 2
#define WantInitRunInBackground 0
#define WantInitNotAutoSlow 0
#define WantInitSpeedValue 1
#define NeedRequestInsertDisk 0
#define NeedDoMoreCommandsMsg 0
#define NeedDoAboutMsg 0
#define UseControlKeys 1
#define UseActvCode 0

/* version and other info to display to user */

#define NeedIntlChars 0
#define kStrAppName "Mini vMac"
#define kAppVariationStr "minivmac-3.3.3-lx64"
#define kStrCopyrightYear "2013"
#define kMaintainerName "unknown"
#define kStrHomePage "http://minivmac.sourceforge.net/"
