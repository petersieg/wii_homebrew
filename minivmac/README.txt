Port to Wii in May 2025 by P.Sieg.

Due to Copyright reasons, you have to supply your own copy of vMac.ROM
and additional (bootable) disk image file named: diskN.dsk. N=1..4
and place them inside sd:/apps/minivmac/

make = makes wii minivmac.elf and minivmac.dol. cp minivmac.dol boot.dol. Uses build dir.
make -f Makefile.org = makes MacOS/Linux minivmac (-DLOCAL). Uses bld dir.
make -f Makefile.mingw64 = makes Windows minivmax.exe (-DLOCAL). Uses bld dir. Untested after wii dev.

Found at: https://github.com/vanfanel/minivmac_sdl2
    
Copyright (C) 2009 Bernd Schmidt, Philip Cummins, Paul C. Pratt

MnvM_bld: README
Paul C. Pratt
www.gryphel.com
July 27, 2005

MnvM_bld is the build system for Mini vMac,
a miniature Macintosh emulator.

Further information may be found at
"http://minivmac.sourceforge.net/".

You can redistribute MnvM_bld and/or modify it under the terms
of version 2 of the GNU General Public License as published by
the Free Software Foundation.  See the included file COPYING.

MnvM_bld is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
license for more details.

