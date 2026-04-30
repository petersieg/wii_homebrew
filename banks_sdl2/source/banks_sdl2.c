/* De-obfuscating the Banks flight simulator from IOCCC 1998. Not finished,
I haven’t tracked down all the math though author has mentioned he used
orthogonal matrices.

Stdin is lists of 3d point coordinates. Each list is an object
drawn connect-a-dot and terminated with 0 0 0. Z upward is negative.

Get the .scene data files from the IOCCC site.

http://www0.us.ioccc.org/years.html#1998

Controls:

Arrow keys are the flight stick.
Enter re-centers stick left-right, but not forward-back.
PageUp, PageDn = throttle

HUD on bottom-left:
speed, heading (0 = North), altitude

Math note:

Angles
There are 3 angles in the math. Your compass heading, your
front-back tilt, and your sideways tilt = Tait-Bryan
angles typical in aerospace math. Also called yaw, pitch, and roll.
The Z axis is negative upward. That’s called left-handed coordinates.
The rotation matrix assumes that.

The rotation matrix is not shown in final form in the wiki article I cite,
so let’s derive it:

cx = cos(x) and so on. x=sideTilt, y=forwardTilt, z=compass

1 0 0 cy 0 -sy cz sz 0

0 cx sx * 0 1 0 * -sz cz 0

0 -sx cx sy 0 cy 0 0 1

cy 0 -sy cz sz 0

sx*sy cx sx*cy * -sz cz 0

cx*sy -sx cx*cy 0 0 1

cy*cz cy*sz -sy

sx*sy*cz-cx*sz sx*sy*sz+cx*cz sx*cy

cx*sy*cz+sx*sz cx*sy*sz-sx*cz cx*cy


It is shown in section 2.2.1 “Euler Angles” of banks1
references

wiki1 = en.wikipedia.org/wiki/Perspective_transform#Perspective_projection
wiki2 = https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
banks1 = “A DISCUSSION OF METHODS OF REAL-TIME AIRPLANE FLIGHT SIMULATION”
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.510.7499&rep=rep1&type=pdf

Obfuscation notes.

The original program has some tricky syntax.

Forms like –*(DN -N? N-DT ?N== RT?&u: & W:&h:&J )
are nested x ? y :z that return the address of a variable that
gets de-referenced by the * and finally decremented.

A comma like in c += (I = M / l, l * H + I * M + a * X) * _;

makes a sequence of statements. The last one is the value of the

(…) list.

*/

/*
 * SDL2 port of Banks Flight Simulator (IOCCC 1998)
 * Original math, projection, stdin scene, and physics preserved
 * X11 removed and replaced with SDL2 software renderer
 * Escape exits
 * HUD printed to terminal
 * Compile with: clang -o banks_sdl2 banks_sdl2.c $(sdl2-config --cflags --libs) -lm
 * Run like: cat horizon.scene pittsburgh.scene | ./banks_sdl2
 * Converted from original X11 version to SDL2 using ChatGPT in March 2026 by P. Sieg
 */

#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#ifdef WII
#include <wiiuse/wpad.h>
#endif

/* Keyboard constants */
#define Throttle_Up SDLK_PAGEUP
#define Throttle_Down SDLK_PAGEDOWN
#define Up SDLK_UP
#define Down SDLK_DOWN
#define Left SDLK_LEFT
#define Right SDLK_RIGHT
#define Enter SDLK_RETURN

#define dt 0.02

/* --- Original global variables from banks.c --- */
double  airplaneX, sideTiltRadians, P, timeDelta = dt, T, Z, D = 1, d,
        worldZ[999], E, speed = 8, I, up_down, accel,
        worldY[999], M, m, compassRadians;
double  worldX[999], forwardTiltRadians = 33e-3, airplaneZ = 1E3,
        t, left_right, v, W, S = 74.5, speedFeet = 221, X = 7.26;
double  a, gravityAccel = 32.2, airplaneY, F, H;
double  cos_compass, cos_forwardTilt, cos_sideTilt = 1, Dx, Dy, Dz,
        R11, R12, R13, R21, R22, R23, R31, R32, R33,
        worldX_rel, worldY_rel, worldZ_rel,
        sin_compass, sin_forwardTilt, sin_sideTilt;
int     N, x, integerConvert, num_pts, speedKnots, idx;
int     prevX, prevY, y;
char    infoStr[52];

/* --- SDL2 variables --- */
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
bool running = true;

/* --- Load stdin scene --- */
void loadMapFiles() {
	FILE *file;
	file = fopen("banks.sc", "r");
    if (file == NULL) {
        fprintf(stderr,"Error opening banks.sc scene file\n");
    }
	else {
    	for (; fscanf(file,"%lf%lf%lf", worldX + num_pts, worldY + num_pts, worldZ + num_pts) + 1; num_pts++);
    	fprintf(stderr, "Loaded %d points from scene banks.sc\n", num_pts);
    	fclose(file);
	}
}

/* --- Sleep for dt --- */
void sleepForInterval() {
    //struct timeval sleeptime = {0, dt * 1e6};
    //select(0, 0, 0, 0, &sleeptime);
    SDL_Delay(dt);
}

/* --- Angle calculations --- */
void calculateTrigonometricValues() {
    cos_forwardTilt = cos(forwardTiltRadians);
    sin_forwardTilt = sin(forwardTiltRadians);
    cos_compass = cos(compassRadians);
    cos_sideTilt = cos(sideTiltRadians);
    sin_sideTilt = sin(sideTiltRadians);
    sin_compass = sin(compassRadians);
}
void updateF() { F += timeDelta * P; }
void updateCompassRadians() { compassRadians += cos_sideTilt * timeDelta * F / cos_forwardTilt + d / cos_forwardTilt * sin_sideTilt * timeDelta; }
void updateForwardTiltRadians() { forwardTiltRadians += d * timeDelta * cos_sideTilt - timeDelta * F * sin_sideTilt; }
void updateSideTiltRadians() { sideTiltRadians += (sin_sideTilt * d / cos_forwardTilt * sin_forwardTilt + v + sin_forwardTilt / cos_forwardTilt * F * cos_sideTilt) * timeDelta; }
void updateRotationMatrix() {
    R11 = cos_forwardTilt * cos_compass;
    R12 = cos_forwardTilt * sin_compass;
    R13 = -sin_forwardTilt;
    R21 = cos_compass * sin_sideTilt * sin_forwardTilt - sin_compass * cos_sideTilt;
    R22 = cos_sideTilt * cos_compass + sin_sideTilt * sin_compass * sin_forwardTilt;
    R23 = sin_sideTilt * cos_forwardTilt;
    R31 = sin_compass * sin_sideTilt + cos_sideTilt * sin_forwardTilt * cos_compass;
    R32 = sin_forwardTilt * sin_compass * cos_sideTilt - sin_sideTilt * cos_compass;
    R33 = cos_sideTilt * cos_forwardTilt;
}
void calculateAngles() {
    calculateTrigonometricValues();
    updateF();
    updateCompassRadians();
    updateForwardTiltRadians();
    updateSideTiltRadians();
    updateRotationMatrix();
}

/* --- World point transforms --- */
void shiftWorldObjectVertex(int idx) {
    worldX_rel = worldX[idx] - airplaneX;
    worldY_rel = worldY[idx] - airplaneY;
    worldZ_rel = worldZ[idx] + airplaneZ;
}
void applyRotationMatrix() {
    Dx = R11 * worldX_rel + R12 * worldY_rel + R13 * worldZ_rel;
    Dy = R21 * worldX_rel + R22 * worldY_rel + R23 * worldZ_rel;
    Dz = R31 * worldX_rel + R32 * worldY_rel + R33 * worldZ_rel;
}
void project3DPointOnto2DPlane() {
    x = Dy / Dx * 384 + 64;
    y = Dz / Dx * 384 + 64;
}

/* --- Draw lines --- */
void drawLine() {
    if (prevX - 1E4)
        SDL_RenderDrawLine(renderer, prevX, prevY, x, y);
    prevX = x;
    prevY = y;
}

/* --- Update HUD (terminal) --- */
void updateInfoString() {
    speedKnots = speedFeet / 1.7;
    sprintf(infoStr, "% 5d % 3d % 7d", speedKnots, (int)(compassRadians * 57.3) % 360, (int)airplaneZ);
    printf("%s\n", infoStr);
}

/* --- Update display --- */
void updateDisplay() {
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);

    for (idx = 0, prevX = 1E4; idx < num_pts; ++idx) {
        shiftWorldObjectVertex(idx);
        applyRotationMatrix();
        if (worldX[idx]+worldY[idx]+worldZ[idx]==0 || Dx<fabs(Dy) || Dx<fabs(Dz))
            prevX = 1e4;
        else {
            project3DPointOnto2DPlane();
            drawLine();
        }
    }
    SDL_RenderPresent(renderer);
}

/* --- Handle keys --- */
void handleKeyPress() {
#ifdef WII 
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) running=false; //exit(0);
		
		// Turn Remote horizontally
		if ( pressed & WPAD_BUTTON_UP ) ++left_right;
		if ( pressed & WPAD_BUTTON_DOWN ) --left_right;
		if ( pressed & WPAD_BUTTON_RIGHT ) ++up_down;
		if ( pressed & WPAD_BUTTON_LEFT ) --up_down;
		if ( pressed & WPAD_BUTTON_1 ) --speed;
		if ( pressed & WPAD_BUTTON_2 ) ++speed;
		if ( pressed & WPAD_BUTTON_A ) left_right=0;
		if ( pressed & WPAD_BUTTON_B ) left_right=0;
		
#endif
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case Up: ++up_down; break;
                case Down: --up_down; break;
                case Left: ++left_right; break;
                case Right: --left_right; break;
                case Throttle_Up: ++speed; break;
                case Throttle_Down: --speed; break;
                case Enter: left_right=0; break;
                case SDLK_ESCAPE: running=false; break;
            }
        }
    }
}

/* --- Physics calculations (unchanged from original) --- */
void updateMomentum() { M += H * timeDelta; }
void calculateInertia() { I = M / speedFeet; }
void updateAirplanePosition() {
    airplaneX += (R11 * speedFeet + R21 * M + R31 * X) * timeDelta;
    airplaneY += (R12 * speedFeet + I * M + R32 * X) * timeDelta;
    airplaneZ += (-R13 * speedFeet - R23 * M - R33 * X) * timeDelta;
}
void calculateIntermediateValues() { m = 15 * F / speedFeet; E = 0.1 + X * 4.9 / speedFeet; T = X*X + speedFeet*speedFeet + M*M; t = T*m/32 - I*T/24; }
void calculateH() { H = gravityAccel * R23 + v*X - F*speedFeet + t/S; }
void calculateAcceleration() { accel = F*M + (speed*1e4/speedFeet - (T+E*5*T*E)/3e2)/S - X*d - sin_forwardTilt*gravityAccel; }
void updateSpeed() { speedFeet += accel*timeDelta; }
void calculateNextStepValues() { a = 2.63/speedFeet*d; X += (d*speedFeet - T/S*(0.19*E + a*0.64 + up_down/1e3) - M*v + gravityAccel*R33)*timeDelta; W = d; d += T*(0.45-14/speedFeet*X - a*130 - up_down*0.14)*timeDelta/125e2 + F*timeDelta*v; D = v/speedFeet*15; }
void calculateP() { P = (T*(47*I - m*52 + E*94*D - t*0.38 + left_right*0.21*E)/1e2 + W*179*v)/2312; }
void updateV() { v -= (W*F - T*(0.63*m - I*0.086 + m*E*19 - D*25 - 0.11*left_right)/107e2)*timeDelta; }

void updatePhysics() {
    updateMomentum();
    calculateInertia();
    updateAirplanePosition();
    calculateIntermediateValues();
    calculateH();
    calculateAcceleration();
    updateSpeed();
    updateInfoString();
    calculateNextStepValues();
    calculateP();
    updateV();
}

/* --- Main --- */
int main(int argc, char *argv[]) {
    /* SDL2 initialization */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr,"SDL_Init error: %s\n",SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("Banks Flight Simulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              384, 384, 0);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);

#ifdef WII 
	// This function initialises the attached controllers
	WPAD_Init();
#endif
    /* Load stdin scene */
    loadMapFiles();

    /* Simulation loop */
    while (running) {
        sleepForInterval();
        calculateAngles();
        handleKeyPress();
        updatePhysics();
        updateDisplay();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
