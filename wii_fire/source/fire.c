/* Fireworks demo written by Dave Ashley */
/* dashxdr@gmail.com */
/* http://www.xdr.com/dash */
/* GPL */
// It seems like SDL for Wii (both versions) now depend on OpenGX.
// Install wii-opengx through pacman and add -lopengx to your makefile.
// CFLAGS		=	-g -O2 -Wall $(MACHDEP) $(INCLUDE) `$(PREFIX)pkg-config --cflags sdl`
// LIBS	:=	`$(PREFIX)pkg-config --libs sdl` -lopengx -lwiiuse -lbte -lasnd -lfat -logc -lm -lwiikeyboard

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/timeb.h>

//#include <SDL/SDL.h>
//#include "/opt/devkitpro/portlibs/wii/include/SDL/SDL.h"
#include <SDL.h>

#define XSIZE 640
#define YSIZE 480

SDL_Surface *thescreen;
unsigned char *vmem1,*vmem2;
int mousex,mousey;
SDL_Color themap[256];

void scrlock(void)
{
	if(SDL_MUSTLOCK(thescreen))
		SDL_LockSurface(thescreen);
}

void scrunlock(void)
{
	if(SDL_MUSTLOCK(thescreen))
		SDL_UnlockSurface(thescreen);
}

void copyup(void)
{
	SDL_UpdateRect(thescreen, 0, 0, 0, 0);
}

#define MOUSEFRAC 2
#define MAXBLOBS 512
#define BLOBFRAC 6
#define BLOBGRAVITY 5
#define THRESHOLD 20
#define SMALLSIZE 3
#define BIGSIZE 6

#define ABS(x) ((x)<0 ? -(x) : (x))

int explodenum;

char sizes[]={2,3,4,5,8,5,4,3};


struct blob {
	struct blob *blobnext;
	int blobx;
	int bloby;
	int blobdx;
	int blobdy;
	int bloblife;
	int blobsize;
} *blobs,*freeblobs,*activeblobs;


unsigned char **mul640;
int oldmode;

char sqrttab[]={
0,1,1,1,2,2,2,2,2,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,
5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,
6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
};


void nomem(void)
{
	exit(1);
}



void fire(unsigned char *dst,unsigned char *src,char *map)
{
int i;

	memset(src, 0, 2*XSIZE);
	i=XSIZE*(YSIZE-2);
	src += XSIZE*2;
	dst += XSIZE*2;
	while(i--)
	{
		*dst++=map[*src+src[-XSIZE]+src[-XSIZE-1]+src[-XSIZE+1]+src[-1]+src[1]+src[-XSIZE-XSIZE-1]+src[-XSIZE-XSIZE]+src[-XSIZE-XSIZE+1]];
		src++;
	}
}

void disk(int x,int y,int rad)
{
unsigned char *p;
int i,j,k,aj;
int rad2=rad*rad;
int w;


	for(j=-rad;j<=rad;j++)
	{
		w=sqrttab[rad2-j*j];
		aj=ABS(j)<<2;
		if(w)
		{
			p=mul640[y+j]+x-w;
			k=w+w+1;
			i=-w;
			while(k--) {*p++=255-(ABS(i)<<2)-aj;i++;}
		}
	}
}
void trydisk(void)
{
	if(mousex>10 && mousex<XSIZE-10 && mousey>10 && mousey<YSIZE-10)
		disk(mousex,mousey,8);
}

void addblob(void)
{
int dx,dy;
struct blob *ablob;

	if(!freeblobs) return;
	dx=(rand()&255)-128;
	dy=(rand()%200)+340;
	ablob=freeblobs;
	freeblobs=freeblobs->blobnext;
	ablob->bloblife=(rand()&511)+256;
	ablob->blobdx=dx;
	ablob->blobdy=dy;
	ablob->blobx=(256+(rand()&127))<<BLOBFRAC;
	ablob->bloby=2<<BLOBFRAC;
	ablob->blobnext=activeblobs;
	ablob->blobsize=BIGSIZE;
	activeblobs=ablob;
}
void moveblobs(void)
{
struct blob **lastblob,*ablob;
int x,y;

	lastblob=&activeblobs;
	while((ablob=*lastblob))
	{
		x=ablob->blobx>>BLOBFRAC;
		y=ablob->bloby>>BLOBFRAC;
		if(!--ablob->bloblife || y<0 || x<10 || x>XSIZE-10)
		{
			*lastblob=ablob->blobnext;
			ablob->blobnext=freeblobs;
			freeblobs=ablob;
			continue;
		}
		ablob->blobx+=ablob->blobdx;
		ablob->bloby+=ablob->blobdy;
		ablob->blobdy-=BLOBGRAVITY;
		lastblob=&ablob->blobnext;
	}
}
void putblobs(void)
{
struct blob *ablob,*ablob2,*temp;
int x,y,dy;
int i,size;
long x2,y2,vel;

	ablob=activeblobs;
	activeblobs=0;
	while(ablob)
	{
		dy=ablob->blobdy;
		if(ablob->blobsize!=SMALLSIZE && ((dy>-THRESHOLD && dy<THRESHOLD && !(rand()&7)) || (rand()&127)==63))
		{
			i=explodenum;
			while(i-- && freeblobs)
			{
				ablob2=freeblobs;
				freeblobs=freeblobs->blobnext;
				ablob2->blobx=ablob->blobx;
				ablob2->bloby=ablob->bloby;
				for(;;)
				{
					x2=(rand()&511)-256;
					y2=(rand()&511)-256;
					vel=x2*x2+y2*y2;
					if(vel>0x3000 && vel<0x10000L) break;
				}
				ablob2->blobdx=ablob->blobdx+x2;
				ablob2->blobdy=ablob->blobdy+y2;
				ablob2->bloblife=16+(rand()&31);
				ablob2->blobsize=SMALLSIZE;
				ablob2->blobnext=activeblobs;
				activeblobs=ablob2;
				ablob->bloblife=1;
			}			
		}
		x=ablob->blobx>>BLOBFRAC;
		y=ablob->bloby>>BLOBFRAC;
		size=ablob->blobsize;
		if(size==BIGSIZE && ablob->blobdy>0 && ablob->blobdy<200)
			size=sizes[ablob->bloblife&7];
		if(x>10 && x<XSIZE-10 && y>10 && y<YSIZE-10)
			disk(x,YSIZE-1-y,size);
		temp=ablob;
		ablob=ablob->blobnext;
		temp->blobnext=activeblobs;
		activeblobs=temp;
	}
}



#define RATE 1
void normal(char *map)
{
int i,j;
	for(i=0;i<8192;i++)
	{
		j=i/9;
		map[i]=j<256 ? (j>=RATE ? j-RATE : 0) : 255;
	}
}
void bright(char *map)
{
int i;
	for(i=0;i<8192;i++) map[i]=i>>3<255 ? (i>>3) : 255;
}

void updatemap(void)
{
	SDL_SetColors(thescreen, themap, 0, 256);
}


void loadcolor(int n,int r,int g,int b)
{
	themap[n].r=r<<2;
	themap[n].g=g<<2;
	themap[n].b=b<<2;
}


void loadcolors(unsigned int which)
{
int i,j;
int r,g,b;

	which%=11;
	for(i=0;i<256;i++)
	{
		switch(which)
		{
		case 0:
			if(i<64) loadcolor(i,0,0,0);
			else if(i<128)	loadcolor(i,i-64,0,0);
			else if(i<192) loadcolor(i,63,i-128,0);
			else loadcolor(i,63,63,i-192);
			break;
		case 1:
			if(i<64) loadcolor(i,0,0,0);
			else if(i<128)	loadcolor(i,0,0,i-64);
			else loadcolor(i,(i-128)>>1,(i-128)>>1,63);
			break;
		case 2:
			loadcolor(i,i>>2,i>>2,i>>2);
			break;
		case 3:
			r=rand()&0x3f;
			g=rand()&0x3f;
			b=rand()&0x3f;
			loadcolor(i,r*i>>8,g*i>>8,b*i>>8);
			break;
		case 4:
			loadcolor(i,i>>2,0,0);
			break;
		case 5:
			loadcolor(i,0,i>>2,0);
			break;
		case 6:
			loadcolor(i,0,0,i>>2);
			break;
		case 7:
			j=i&15;
			if(i&16) j=15-j;
			j=(i>>2)*j/16;
			loadcolor(i,j,j,j);
			break;
		case 8:
			j=0;
			if(i>8 && i<128) j=63;
			loadcolor(i,j,j,j);
			break;
		case 9:
			j=(31-(i&31))<<1;
			r=i&32 ? j : 0;
			g=i&64 ? j : 0;
			b=i&128 ? j : 0;
			loadcolor(i,r,g,b);
			break;
		case 10:
			j=(i&15)<<2;
			if(i&16) j=63-j;
			r=i&32 ? j : 0;
			g=i&64 ? j : 0;
			b=i&128 ? j : 0;
			loadcolor(i,r,g,b);
			break;
		}
	}
	updatemap();
}

void dograb(void)
{
char temp[1024*3];
int fd;
int res;
int len;
uint8_t *p;
int i,j;

	fd = open("/fire_grab.ppm", O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if(fd<0) return;
	len=sprintf(temp, "P6\n%d %d\n255\n", XSIZE, YSIZE);
	res=write(fd, temp, len);
	for(j=0;j<YSIZE;++j)
	{
		p = vmem1 + j*XSIZE;
		for(i=0;i<XSIZE;++i)
		{
			SDL_Color *c;
			c = themap + p[i];
			temp[i*3+0] = c->r;
			temp[i*3+1] = c->g;
			temp[i*3+2] = c->b;
		}
		res=write(fd, temp, i*3);
	}
	close(fd);
}



int main(int argc, char **argv)
{
int i;
char *remap,*remap2;
int flash;
int whichmap;
int ispaused;
unsigned long videoflags;
int done;
int time;
SDL_Event event;
int buttonstate;

extern void fatInitDefault(void);
	fatInitDefault();

	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
		exit(1);
	videoflags = SDL_SWSURFACE;

	thescreen = SDL_SetVideoMode(XSIZE, YSIZE, 8, videoflags);
	if ( thescreen == NULL )
		exit(5);
	SDL_JoystickOpen(0);

	/* Set the surface pixels and refresh! */
	scrlock();
	memset(thescreen->pixels,0,XSIZE*YSIZE);
	scrunlock();
	vmem1=calloc(XSIZE*YSIZE+1, 1);
	vmem2=calloc(XSIZE*YSIZE+1, 1);
	if(!vmem1 || !vmem2)
		exit(1);
	mul640=malloc(YSIZE*sizeof(char *));
	if(!mul640) nomem();
	remap=malloc(16384);
	if(!remap) nomem();
	remap2=malloc(16384);
	if(!remap2) nomem();
	blobs=malloc(MAXBLOBS*sizeof(struct blob));
	if(!blobs) nomem();

	freeblobs=activeblobs=0;
	for(i=0;i<MAXBLOBS;i++)
	{
		blobs[i].blobnext=freeblobs;
		freeblobs=blobs+i;
	}

	normal(remap);
	bright(remap2);

	for(i=0;i<YSIZE;i++) mul640[i]=i*XSIZE+vmem1;
	flash=0;
	whichmap=0;
	loadcolors(whichmap);
	ispaused=0;
	addblob();
	done = 0;
	time=0;
	buttonstate=0;
	mousex=mousey=0;

	while(!done)
	{
		int button;
		if(!ispaused)
		{
			time++;
			if(!flash)
			{
				if((explodenum>96 && explodenum<160 && !(rand()&511)) || (buttonstate&8))
					flash=60;
			} else --flash;
			explodenum=(time>>4)+1;if(explodenum==320) time=0;
			if(explodenum>256) explodenum=256;
			if(!(rand()&31))
				addblob();
			moveblobs();
			putblobs();
			if(buttonstate&2) trydisk();
			memcpy(vmem2, vmem1, XSIZE*YSIZE);
			fire(vmem1, vmem2, flash ? remap2 :remap);
			scrlock();
			SDL_ShowCursor(0);
			for(i=0;i<YSIZE;++i)
				memcpy(thescreen->pixels + i*thescreen->pitch, mul640[i], XSIZE);
			SDL_ShowCursor(1);
			scrunlock();
			copyup();
		}

		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				buttonstate|=1<<event.button.button;
				mousex=event.button.x;
				mousey=event.button.y;
				if(!ispaused && buttonstate&2) trydisk();
				break;
			case SDL_MOUSEBUTTONUP:
				buttonstate&=~(1<<event.button.button);
				mousex=event.button.x;
				mousey=event.button.y;
				break;
			case SDL_MOUSEMOTION:
				mousex=event.motion.x;
				mousey=event.motion.y;
				if(!ispaused && buttonstate&2) trydisk();
				break;
			case SDL_JOYBUTTONDOWN:
				switch((button=event.jbutton.button))
				{
				case 0: // A (comes back as mouse event)
				case 1: // B (comes back as mouse event)
					break;
				case 3: // 2
					dograb();
					break;
				case 2: // 1
					ispaused = !ispaused;
					break;
				case 6: // Home
					done = 1;
					break;
				case 4: // -
				case 5: // +
					if(button==4) --whichmap;
					else ++whichmap;
					loadcolors(whichmap);
					break;
				}		
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
			}
		}
	}

	SDL_Quit();
	exit(0);
}
