# wii_homebrew
Some simple and small homebrew apps. Create "apps" folder on SD card and place these folders inside. Connect a USB keyboard to Wii.

Zum Installieren von HomeBrew dieser Anleitung folgen - To install homebrew follow this guide: 

https://de.wikihow.com/Homebrew-auf-einer-Wii-mit-Men%C3%BC-4.3-installieren

Some sources are from other authors. If someone from the original authors wants some changes/alterations or remove their code, please let me know.

Most need USB keyboard attached to Wii!

* hello - hello world with getchar/scanf input
* c4 - Connect 4 (Is not correctly detecting winner)
* collatz - calc collatz sequence. A stop; B cont. (See: https://en.wikipedia.org/wiki/Collatz_conjecture)
* reverse - reverse the numbers to get 1..9 (Adapted from: https://www.rosettacode.org/wiki/Number_reversal_game)
* reversi - reversi/othello game (https://dev.to/iigura/reversiothello-game-program-which-you-can-choose-the-first-or-the-second-in-7-lines-code-of-c-3799)
* wii_fire - Fireworks demo for SDL(1). Author: Dashxdr. GPL. Just Makefile updated to compile. See: Source and https://wiibrew.org/wiki/Fire
* f030 - Old + simple SDL demo from Atari Falcon 030 experiments compiled for wii
* pong - Simple SDL Pong game using USB keyboard!
* pong2 - Simple SDL Pong game using Wiimote (A; Up/Dn/+/-; HOME) - not perfect; need something like autorepeat buttons
* eier - Catch golden+colored eggs, avoid bad/brown eggs. Copyright (c) 2005 Patrick Gerdsmeier (patrick@gerdsmeier.net) GPL2 - Wii port: Peter Sieg 10/2024 with small changes to make it easier. Needs sdl_image to compile and USB keyboard.
* invaders - Space Invaders from: https://github.com/flightcrank/space-invaders
* asteroids - From: https://github.com/flightcrank/asteroids - GPL2 - Did not allow/hit last asteroid? Unfinished game play.
* breakout - From: https://github.com/uncleBlobby/breakout-c - No live lost code. Unfinished game play.
* See also: https://github.com/petersieg/wii-bhspitmonkey - need wiisprite lib!
  
(Pong is maybe also coming from: https://github.com/flightcrank/pong)

Using devkitpro wii-dev under IMac 2010 27" none Metal GPU with OCLP Ventura.

Pre requisit: To get the Xcode command line tools run `xcode-select --install` from Terminal.

Use the pkg installer to install devkitPro pacman (dkp-pacman) - see: https://devkitpro.org/wiki/Getting_Started

In terminal: sudo dkp-pacman -S wii-dev to install the tools and libraries you wish to develop for (will be installed to /opt/devkitpro/*). 

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

```
ich@iMac-von-ich hello % ls -l
total 3600
-rw-r--r--  1 ich  staff     5617 18 Jan  2024 Makefile
drwxr-xr-x  5 ich  staff      160 20 Sep 17:47 build
-rw-r--r--  1 ich  staff   291904 20 Sep 17:47 hello.dol
-rwxr-xr-x  1 ich  staff  1536600 20 Sep 17:47 hello.elf
drwxr-xr-x  3 ich  staff       96 18 Sep 21:10 source
```

mv hello.dol boot.dol

supply png icon file and meta.xml file accordingly to your liking.

Place hello folder on sd card under apps folder; start homebrew channel and your hello should appear to be started.

---


Wem eine Wii Sensorleiste fehlt: https://www.youtube.com/watch?v=f7uKSUT9pH4

Heute auf einem Steckbrett ausprobiert und funktioniert prima! 4x IR Leds in Reihe mit ca. 20-30 Ohm Widerstand an +5V. Leds von hier: https://www.ebay.de/itm/175773023902

PS: Die Bezeichnung "Sensorleiste" ist mind, irreführend. Ist eigentlich ja nur eine zweifache Infrarotquelle mit ca. 15-20cm Abstand.

![wii-diy](wii-sensor-diy.jpeg)

---

Fire (use -/+ to change color):
![fire](screens/fire.jpeg)

Pong:
![pong](screens/pong.jpeg)

Eier:
![eier](screens/eier.jpeg)

Invaders:
![invaders](screens/invaders.jpeg)

Asteroids:
![asteroids](screens/asteroids.jpeg)

Breakout:
![breakout](screens/breakout.jpeg)

---

Use dkp-pacman to:
* `sudo dkp-pacman -Sl dkp-libs` To show only the libraries supplied by devkitPro
* To list packages use `dkp-pacman -Sl`
* To install a package use `sudo dkp-pacman -S <name of package>`
* `sudo dkp-pacman -R <name of package>` will remove an already installed package.
  
Examples:

* sudo dkp-pacman -S wii-sdl - to install sdl(1)
* sudo dkp-pacman -S wii-opengx - that seems to be required now by SDL!

---
These wii devkit libs are available as of 09/2024:
```
dkp-libs wii-cmake 1.0.0-2 [installed]
dkp-libs wii-examples 20240118-1 [installed]
dkp-libs wii-freeglut 3.6.0-4
dkp-libs wii-glu 9.0.3-1
dkp-libs wii-opengx 0.9.0-1 [installed]
dkp-libs wii-physfs 3.2.0-3
dkp-libs wii-pkg-config 0.28-5 [installed]
dkp-libs wii-sdl 1.2.15-16 [installed]
dkp-libs wii-sdl2 2.28.5-13
dkp-libs wii-sdl2_gfx 1.0.4-3
dkp-libs wii-sdl2_image 2.0.4-2
dkp-libs wii-sdl2_mixer 2.6.3-1
dkp-libs wii-sdl2_ttf 2.22.0-2
dkp-libs wii-sdl_gfx 2.0.27-1
dkp-libs wii-sdl_image 1.2.12-5
dkp-libs wii-sdl_mixer 1.2.12-10
dkp-libs wii-sdl_ttf 2.0.11-5
```
