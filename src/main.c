#include <stdio.h>
#include "setup.h"
#include "cesi_types.h"
#include "grove_rgb.h"
#include "grove_button.h"
#include "grove_light.h"
#include "grove_rtc_ds1307.h"
#include "grove_gps_air530z.h"
#include "grove_bme680.h"
#include "sd_logger.h"
#include "app_console.h"

int main(void)
{
    Global_Init();

    while (1) {
    // 1. On écoute la console sans bloquer
    AppConsole_Process();

    // 2. On fait le reste du travail normal...
    // Mesurer la lumière, vérifier le GPS, etc.

    // 3. Exemple d'utilisation du mode maintenance
    if (mode_maintenance) {
        // printf("Mesure brute : %d\r\n", valeur_capteur);
        // HAL_Delay(1000); // Attention à ne pas mettre de gros delay ici !
    }
 }
        
}
