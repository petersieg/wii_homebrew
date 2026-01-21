// Adapted from: https://www.rosettacode.org/wiki/Number_reversal_game#C
// 09/2024 peter.sieg2@gmx.de

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LISP_IMPLEMENTATION
#include "lisp.h"
#include "lisp_lib.h"

#define LINE_MAX 4096

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

bool quitapp=false;

void keyPress_cb( char sym) {

	if (sym > 31 ) putchar(sym);
	if (sym == 13) putchar('\n');
	
	if ( sym == 0x1b) quitapp = true;
}

static Lisp sch_load(Lisp args, LispError* e, LispContext ctx)
{
    Lisp path = lisp_car(args);
    Lisp result = lisp_read_path(lisp_string(path), e, ctx);
    if (*e != LISP_ERROR_NONE) return lisp_null();
    return lisp_eval(result, e, ctx);
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
    srand(time(NULL));//


    const char* file_path = NULL;
    int run_script = 0;
    int verbose;
#ifdef LISP_DEBUG
    verbose = 1;
#else
    verbose = 0;
#endif

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--load") == 0)
        {
            file_path = argv[i + 1];
        }
        if (strcmp(argv[i], "--script") == 0)
        {
            file_path = argv[i + 1];
            run_script = 1;
        }
    }

    //LispContext ctx = lisp_init();
    LispContext ctx = lisp_init_with_lib();
    lisp_env_define(
        lisp_cdr(lisp_env(ctx)),
        lisp_make_symbol("LOAD", ctx),
        lisp_make_func(sch_load),
        ctx
    );

    // Load as a macro is called "include" and can be used to load files containing macros.
    lisp_table_set(
            lisp_macro_table(ctx),
            lisp_make_symbol("INCLUDE", ctx),
            lisp_make_func(sch_load),
            ctx
    );

    Lisp pi = lisp_make_real(3.141592);
    lisp_env_define(lisp_cdr(lisp_env(ctx)), lisp_make_symbol("PI", ctx), pi, ctx);
/*
    clock_t start_time, end_time;

    if (file_path)
    {
        if (verbose)
        {
            printf("loading: %s\n", file_path);
        }

        start_time = clock();

        LispError error;
        Lisp l = lisp_read_path(file_path, &error, ctx);

        if (error != LISP_ERROR_NONE)
        {
            fprintf(stderr, "%s. %s\n", file_path, lisp_error_string(error));
        }

        end_time = clock();

        if (verbose)
            printf("read (us): %lu\n", 1000000 * (end_time - start_time) / CLOCKS_PER_SEC);

        start_time = clock();

        Lisp code = lisp_macroexpand(l, &error, ctx);

        if (error != LISP_ERROR_NONE)
        {
            fprintf(stderr, "%s\n", lisp_error_string(error));
            exit(1);
        }


        end_time = clock();

        if (verbose)
            printf("expand (us): %lu\n", 1000000 * (end_time - start_time) / CLOCKS_PER_SEC);


        start_time = clock();
        lisp_eval(code, &error, ctx);
        end_time = clock();

        if (error != LISP_ERROR_NONE)
        {
            fprintf(stderr, "%s\n", lisp_error_string(error));
            exit(1);
        }

        lisp_collect(lisp_null(), ctx);

        if (verbose)
            printf("eval (us): %lu\n", 1000000 * (end_time - start_time) / CLOCKS_PER_SEC);
    }
*/
    if (!run_script && !quitapp)
    {
        // REPL
        while (!feof(stdin))
        {
			// Call WPAD_ScanPads each loop, this reads the latest controller states
			WPAD_ScanPads();

			// WPAD_ButtonsDown tells us which buttons were pressed in this loop
			// this is a "one shot" state which will not fire again until the button has been released
			u32 pressed = WPAD_ButtonsDown(0);

		
			// We return to the launcher application via exit
			if ( pressed & WPAD_BUTTON_HOME ) quitapp=true;
			if (!quitapp)
			{
            printf("> ");
            char line[LINE_MAX];
            if (!fgets(line, LINE_MAX, stdin)) break;

            clock_t start_time = clock();
            LispError error;
            Lisp code = lisp_read(line, &error, ctx);

            if (error != LISP_ERROR_NONE)
            {
                fprintf(stderr, "%s\n", lisp_error_string(error));
                continue;
            }

            Lisp l = lisp_eval(code, &error, ctx);
            clock_t end_time = clock();

            if (error != LISP_ERROR_NONE)
            {
                fprintf(stderr, "%s\n", lisp_error_string(error));
            }

            lisp_printf(stdout, l);
            printf("\n");

            lisp_collect(lisp_null(), ctx);

            if (verbose)
                printf("(us): %lu\n", 1000000 * (end_time - start_time) / CLOCKS_PER_SEC);
            }
       }
    }

    lisp_shutdown(ctx);

	return 0;
}