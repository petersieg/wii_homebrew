// 09/2024 peter.sieg2@gmx.de

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void keyPress_cb( char sym) {

	if (sym > 31 ) putchar(sym);
	if (sym == 13) putchar('\n');
	
	//if ( sym == 0x1b) quitapp = true;
}

#define WIDTH 6
#define HEIGHT 6

enum field {
  EMPTY = 0,
    PL1,
    PL2
};

enum result {
  INV,
  USED,
  WON,
  NE,
  FULL
};

const int field_char[3] = {
  ' ',
  'O',
  '#'
};
const int frame_char[2] = {
  '+',
  '-'
};
const char * player_str[2] = {
  "computer",
  "Mensch"
};

enum field xy[WIDTH][HEIGHT];
int xy_count = 0;

void print_field(void) {
  int ix, iy;

  putchar('\n');

  for (ix = 0; ix < WIDTH; ix++)
    printf(" %d", ix + 1);
  putchar('\n');

  for (ix = 0; ix <= WIDTH * 2; ix++)
    putchar(frame_char[ix % 2]);
  putchar('\n');

  for (iy = 0; iy < HEIGHT; iy++) {

    putchar('|');
    for (ix = 0; ix < WIDTH; ix++) {
      putchar(field_char[xy[ix][HEIGHT - iy - 1]]);
      putchar('|');
    }
    putchar('\n');

  }

  for (ix = 0; ix <= WIDTH * 2; ix++)
    putchar(frame_char[ix % 2]);
  putchar('\n');

  for (ix = 0; ix < WIDTH; ix++)
    printf(" %d", ix + 1);
  putchar('\n');

  putchar('\n');
}

int next_empty_y(int ix) {
  int iy;

  for (iy = 0; iy < HEIGHT; iy++)
    if (xy[ix][iy] == EMPTY)
      return iy;

  return -1;
}

int xy_pl(int player, int ix, int iy) {
  return ix >= 0 && ix < WIDTH &&
    iy >= 0 && iy < HEIGHT &&
    xy[ix][iy] == player;
}

enum result place(int player, int ix) {
  int tlbr_cnt = 0;
  int tmbm_cnt = 0;
  int mlmr_cnt = 0;
  int trbl_cnt = 0;
  int i, iy;

  if ((player != PL1 && player != PL2) ||
    ix < 0 || ix >= WIDTH)
    return INV;

  if ((iy = next_empty_y(ix)) == -1)
    return USED;

  xy[ix][iy] = player;
  xy_count++;

  #define CHECK4(player, ix, iy, counter)\
  for (i = 1; i <= 3; i++)\
    if (!xy_pl(player, ix, iy))\
      break;
  
  CHECK4(player, ix - i, iy - i, tlbr_cnt);
  CHECK4(player, ix + i, iy + i, tlbr_cnt);
  CHECK4(player, ix, iy - i, tmbm_cnt);
  CHECK4(player, ix, iy + i, tmbm_cnt);
  CHECK4(player, ix - i, iy, mlmr_cnt);
  CHECK4(player, ix + i, iy, mlmr_cnt);
  CHECK4(player, ix + i, iy - i, trbl_cnt);
  CHECK4(player, ix - i, iy + i, trbl_cnt);

  if (tlbr_cnt > 2 ||
    tmbm_cnt > 2 ||
    trbl_cnt > 2 ||
    mlmr_cnt > 2)
    return WON;

  return NE;
}

enum result human_new_pos(int player) {
  int new_x, c;

  printf("Spieler #%d (%c), Neue Position (1-%d): ",
    player, field_char[player], WIDTH);
  new_x = (c = getchar()) - '1';
  //while (c != '\n')
    //c = getchar();

  if (new_x < 0 || new_x >= WIDTH)
    return INV;

  return place(player, new_x);
}

enum result computer_new_pos(int player) {
  static int randomize = 1;

  if (randomize) {

    srand(time(NULL));
    randomize = 0;
  }

  return place(player, (double) rand() / RAND_MAX * WIDTH);
}

enum result play_new_pos(int player, int human)

{

  if (xy_count == WIDTH * HEIGHT)
    return FULL;

  return human ?
    human_new_pos(player) :
    computer_new_pos(player);
}

void init_field(void) {
  int ix, iy;
  for (ix = 0; ix < WIDTH; ix++)
    for (iy = 0; iy < HEIGHT; iy++)
      xy[ix][iy] = EMPTY;

  xy_count = 0;
}

void select_modus(int human[2]) {
  int chosen, c;

  while (1) {

    printf("Spiel Modus:\n");
    printf("1-Mensch gegen Mensch\n");
    printf("2-computer gegen Mensch\n");
    printf("3-Mensch gegen computer\n");
    printf("4-computer gegen computer\n");
    printf("Waehle (1-4): ");

    chosen = (c = getchar()) - '0';

    //while (c != '\n')
      //c = getchar();

    if (chosen >= 1 && chosen <= 4)
      break;
    else
      printf("Modus gibt es nicht! Bitte waehle noch einmal\n");
  }

  printf("\n");
  
  human[0] = chosen == 1 || chosen == 3;
  human[1] = chosen == 1 || chosen == 2;

  printf("Spieler #1: %s\n", player_str[human[0]]);
  printf("Spieler #2: %s\n", player_str[human[1]]);
}


int main(int argc, char **argv) {

	// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[0;0H");
	
	if (KEYBOARD_Init(keyPress_cb) == 0) printf("keyboard initialised\n");

  	enum result result;
  	int human[2];

  	select_modus(human);

  	init_field();

  	while (1) {
    	print_field();
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) break;

		// Wait for the next frame
		VIDEO_WaitVSync();

    	do
      		result = play_new_pos(PL1, human[0]);
    	while (result == INV || result == USED);

    	if (result == WON) {
      		print_field();
      		printf("Spieler 1 (%s) won!\n\n", player_str[human[0]]);
      		break;
    	} else if (result == FULL) break;

    	print_field();

    	do
      		result = play_new_pos(PL2, human[1]);
    	while (result == INV || result == USED);

    	if (result == WON) {
      		print_field();
      		printf("Spieler 2 (%s) won!\n\n", player_str[human[1]]);
      		break;
    	} else if (result == FULL) break;
  	}

  	if (result == FULL) {
    	print_field();
    	printf("Unendschieden!");
  	} 

	return 0;
}