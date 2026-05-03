#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
    // 1. Video-System initialisieren
    VIDEO_Init();
    
    // Besten Videomodus für den Fernseher wählen
    rmode = VIDEO_GetPreferredMode(NULL);

    // Framebuffer im Speicher reservieren
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Konsole auf dem Framebuffer einrichten (für printf)
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    // Video-Hardware konfigurieren
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // 2. Eingabe initialisieren
    WPAD_Init();
	// WICHTIG: Erlaube das Senden von Tasten + Beschleunigungsdaten für Controller 0
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC);
	
    printf("\x1b[2;0H"); // Cursor an Position 2,0
    printf("Neigungssensor-Test geladen!\n");

    while(1) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        // Daten der Wii-Fernbedienung holen
        WPADData *wd = WPAD_Data(0);

        // Anzeige aktualisieren
        printf("\x1b[4;0H"); // Cursor an Zeile 4
        printf("X-Achse: %03d\n", wd->accel.x);
        printf("Y-Achse: %03d\n", wd->accel.y);
        printf("Z-Achse: %03d\n", wd->accel.z);

        printf("\x1b[8;0H"); // Cursor an Zeile 8
		// Logik für die Neigung (Toleranzbereich ca. +/- 20)
		if (wd->accel.y > 524) {
    		printf("Status: <-- NEIGUNG LINKS     \n");
		} 
		else if (wd->accel.y < 500) {
    		printf("Status: NEIGUNG RECHTS -->    \n");
		} 
		else {
    		printf("Status: ZENTRIERT             \n");
		}        
		
		if (pressed & WPAD_BUTTON_HOME) exit(0);

        VIDEO_WaitVSync();
    }

    return 0;
}
