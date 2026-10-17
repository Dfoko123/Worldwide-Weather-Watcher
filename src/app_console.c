#include "app_console.h"
#include "app_config.h"
#include "board_pins.h"      // Inclut probablement les définitions HAL de votre équipe
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Déclaration du port série utilisé (souvent huart2 sur STM32 Nucleo)
extern UART_HandleTypeDef huart2;

bool mode_maintenance = false;

// Mémoire tampon (buffer) pour stocker la phrase en cours de frappe
#define RX_BUFFER_SIZE 64
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;

/*
 * Fonction interne : Exécute la commande une fois la touche "Entrée" pressée
 */
static void AppConsole_ParseCommand(char *cmd_line) {
    // 1. On sépare le texte autour du signe "="
    // ex: "LOG_INTERVAL=10" -> key = "LOG_INTERVAL", value_str = "10"
    char *key = strtok(cmd_line, "=");
    char *value_str = strtok(NULL, "=");

    if (key == NULL) return; // Ligne vide

    // --- COMMANDES SIMPLES (Sans signe = ) ---
    
    if (strcmp(key, "VERSION") == 0) {
        printf("Station Meteo STM32 - Equipe GitHub v1.0\r\n");
    }
    else if (strcmp(key, "RESET") == 0) {
        AppConfig_LoadDefaults();
        AppConfig_Save();
        printf("OK : Configuration remise a zero (Usine).\r\n");
    }
    else if (strcmp(key, "MAINTENANCE") == 0) {
        mode_maintenance = !mode_maintenance; // Bascule On/Off
        if (mode_maintenance) {
            printf("OK : Mode MAINTENANCE ACTIVE (Affichage en direct)\r\n");
        } else {
            printf("OK : Mode MAINTENANCE DESACTIVE\r\n");
        }
    }
    
    // --- COMMANDES DE CONFIGURATION (Avec un signe = ) ---
    
    else if (value_str != NULL) {
        int int_value = atoi(value_str); // Convertit le texte "10" en vrai nombre entier 10

        if (strcmp(key, "LOG_INTERVAL") == 0) {
            current_config.log_interval = int_value;
            AppConfig_Save(); // On sauvegarde immédiatement dans l'horloge !
            printf("OK : LOG_INTERVAL = %d\r\n", int_value);
        }
        else if (strcmp(key, "FILE_MAX_SIZE") == 0) {
            current_config.file_max_size = int_value;
            AppConfig_Save();
            printf("OK : FILE_MAX_SIZE = %d\r\n", int_value);
        }
        else if (strcmp(key, "LUMIN_LOW") == 0) {
            current_config.lumin_low = int_value;
            AppConfig_Save();
            printf("OK : LUMIN_LOW = %d\r\n", int_value);
        }
        // TODO : Vous pouvez copier-coller les blocs "else if" ci-dessus 
        // pour ajouter vos autres variables (TEMP_AIR, HYGR_MINT, PRESSURE_MAX...)
        
        // --- COMMANDES SPÉCIALES RTC ---
        else if (strcmp(key, "CLOCK") == 0) {
            // value_str contient "hh:mm:ss"
            int h, m, s;
            if (sscanf(value_str, "%d:%d:%d", &h, &m, &s) == 3) {
                // Ici, vous ferez appel à la fonction de votre driver GroveRTC 
                // pour régler l'heure. Ex: GroveRTC_SetTime(h, m, s);
                printf("OK : Heure reglee sur %02d:%02d:%02d\r\n", h, m, s);
            } else {
                printf("ERREUR : Format attendu CLOCK=hh:mm:ss\r\n");
            }
        }
        else {
            printf("ERREUR : Parametre inconnu (%s)\r\n", key);
        }
    } 
    else {
        printf("ERREUR : Commande non reconnue.\r\n");
    }
}

/*
 * Fonction principale : Lit l'UART sans bloquer le programme
 */
void AppConsole_Process(void) {
    uint8_t rx_char;
    
    // Tente de lire 1 caractère avec un Timeout de 0 ms (Non-Bloquant !)
    if (HAL_UART_Receive(&huart2, &rx_char, 1, 0) == HAL_OK) {
        
        // On renvoie le caractère à l'écran pour que l'utilisateur voie ce qu'il tape (Écho)
        printf("%c", rx_char);

        // Si l'utilisateur appuie sur "Entrée" (Retour chariot \r ou Nouvelle ligne \n)
        if (rx_char == '\r' || rx_char == '\n') {
            printf("\n"); // Saut de ligne propre
            
            if (rx_index > 0) { // Si la phrase n'est pas vide
                rx_buffer[rx_index] = '\0'; // Termine proprement la chaîne de caractères
                AppConsole_ParseCommand(rx_buffer); // On envoie au "cerveau"
                rx_index = 0; // On vide le buffer pour la prochaine commande
            }
        }
        // Sinon, on stocke la lettre et on avance (en évitant de déborder la mémoire)
        else if (rx_index < (RX_BUFFER_SIZE - 1)) {
            rx_buffer[rx_index] = (char)rx_char;
            rx_index++;
        }
    }
}
