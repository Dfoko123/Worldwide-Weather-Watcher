#ifndef APP_CONSOLE_H
#define APP_CONSOLE_H

#include <stdbool.h>

// Variable globale pour le mode maintenance
extern bool mode_maintenance;

// Fonction à appeler EN BOUCLE dans le while(1) du main
void AppConsole_Process(void);

#endif // APP_CONSOLE_H
