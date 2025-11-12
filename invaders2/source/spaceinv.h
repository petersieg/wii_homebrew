#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#ifdef soundsupport
#include <SDL/SDL_mixer.h>
#endif

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_ENTITIES 400
#define MAX_RELOAD_TIME 20

#define PLAYER_SPEED 5
#define BULLET_SPEED 10
#define UFO_SPEED 2
#define BOMB_SPEED 6


#define lineoffset 5
#define topline 50
#define linedist 50

enum
{
	PLAYER_SPRITE,
 	BULLET_SPRITE,
  	UFO_SPRITE,
  	TOP_ALIEN1,
  	TOP_ALIEN2,
  	MIDDLE_ALIEN1,
  	MIDDLE_ALIEN2,
  	BOTTOM_ALIEN1,
  	BOTTOM_ALIEN2,
    BOMB_SPRITE,
    SHELTER_SPRITE,
    LASER_BASE,
    MAX_SPRITES
};

enum
{
	BULLET_SOUND,
	HEARTBEEP,
	ALIENHIT,
	UFO,
	UFOHIT,
	PLAYERHIT,
	ALIENSHOT,
	MAX_SOUNDS
};

enum
{
	TYPE_BULLET,
 	TYPE_ENEMY,
 	TYPE_BOMB
};

typedef struct Game
{
    long int score;
    int lives;
	SDL_Surface *screen;
} Game;

typedef struct Sprite
{
	SDL_Surface *image;
} Sprite;

typedef struct Sound
{
    #ifdef soundsupport
	Mix_Chunk *effect;
	#endif
} Sound;

typedef struct Entity
{
	int active, type;
	int x, y, thinkTime;
	SDL_Surface *sprite;
	void (*action)(void);
	void (*draw)(void);
	int pick_x, pick_y,pick_w,pick_h;
	int sprite_type;
} Entity;

typedef struct Control
{
	int up, down, left, right, fire;
} Control;

Game game;
Control input;
Entity player, *self, entity[MAX_ENTITIES];
Sprite sprite[MAX_SPRITES];
Sound sound[MAX_SOUNDS];
