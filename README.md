# wii_homebrew
Some simple and small homebrew apps

All need USB keyboard attached to wii!

* hello - hello world with getchar/scanf input
* reverse - reverse the numbers to get 1..9
* reversi - reversi/othello game

Using devkitpro wii-dev under IMac 2010 27" none Metal GPU with OCLP Ventura.

Pre requisit: To get the Xcode command line tools run `xcode-select --install` from Terminal.

Use the pkg installer to install devkitPro pacman (dkp-pacman) - see: https://devkitpro.org/wiki/Getting_Started

In terminal: sudo dkp-pacman -S wii-dev to install the tools and libraries you wish to develop for. 

Groups currently available are :-

*    gba-dev
*    gp32-dev
*    nds-dev
*    3ds-dev
*    gamecube-dev
*    wii-dev
*    wiiu-dev
*    switch-dev

mkdir apps; cd apps

cp -a /opt/devkitpro/examples/wii/templates/application .

mv application hello

make hello

ich@iMac-von-ich hello % ls -l
total 3600
-rw-r--r--  1 ich  staff     5617 18 Jan  2024 Makefile
drwxr-xr-x  5 ich  staff      160 20 Sep 17:47 build
-rw-r--r--  1 ich  staff   291904 20 Sep 17:47 hello.dol
-rwxr-xr-x  1 ich  staff  1536600 20 Sep 17:47 hello.elf
drwxr-xr-x  3 ich  staff       96 18 Sep 21:10 source

mv hello.dol boot.dol

supply png icon file and meta.xml file accordingly to your liking.

Place hello folder on sd card under apps folder; start homebrew channel and your hello should appear to be started.
