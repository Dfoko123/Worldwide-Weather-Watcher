#include "app_config.h"
#include "grove_rtc_ds1307.h" // Pour lire/écrire dans la RAM de l'horloge
#include <string.h>

#define CONFIG_MAGIC_WORD 0xABCD // Code secret pour vérifier si la RAM a déjà été initialisée

// Structure englobant le mot magique ET nos paramètres pour la sauvegarde
typedef struct {
    uint16_t magic_word;
    AppConfig_t config;
} RTC_Storage_t;

// La variable globale qui sera utilisée dans le main()
AppConfig_t current_config;

/*
 * Charge les valeurs par défaut (usine)
 */
void AppConfig_LoadDefaults(void) {
    current_config.log_interval = 10;       // 10 minutes
    current_config.file_max_size = 4096;    // 4 Ko
    current_config.timeout = 30;            // 30 secondes
    
    current_config.lumin = true;
    current_config.lumin_low = 255;
    current_config.lumin_high = 768;
    
    current_config.temp_air = true;
    current_config.min_temp_air = -10;
    current_config.max_temp_air = 60;
    
    current_config.hygr = true;
    current_config.hygr_mint = 0;
    current_config.hygr_maxt = 100;
    
    current_config.pressure = true;
    current_config.pressure_min = 850;
    current_config.pressure_max = 1080;
}

/*
 * Vérifie et corrige les valeurs aberrantes (Vérification des bornes)
 */
void AppConfig_CheckBounds(void) {
    // Si l'intervalle est à 0, on le force à 1 pour éviter les divisions par zéro ou les bugs
    if (current_config.log_interval == 0) current_config.log_interval = 1;
    
    // On s'assure que le minimum n'est pas supérieur au maximum !
    if (current_config.lumin_low > current_config.lumin_high) {
        current_config.lumin_low = current_config.lumin_high - 1;
    }
    if (current_config.min_temp_air > current_config.max_temp_air) {
        current_config.min_temp_air = current_config.max_temp_air - 1;
    }
    
    // On bride l'humidité entre 0 et 100%
    if (current_config.hygr_mint < 0) current_config.hygr_mint = 0;
    if (current_config.hygr_maxt > 100) current_config.hygr_maxt = 100;
}

/*
 * Initialisation au démarrage : tente de lire la RAM, sinon charge les défauts
 */
void AppConfig_Init(void) {
    RTC_Storage_t storage;
    
    // On lit tout le bloc de RAM depuis l'offset 0
    if (GroveRTC_ReadRAM(0, (uint8_t*)&storage, sizeof(RTC_Storage_t))) {
        
        // Si on reconnaît le mot magique, c'est que les paramètres sont valides !
        if (storage.magic_word == CONFIG_MAGIC_WORD) {
            // On copie les paramètres de la RAM vers notre variable globale
            memcpy(&current_config, &storage.config, sizeof(AppConfig_t));
            
            // On vérifie les bornes au cas où[cite: 8]
            AppConfig_CheckBounds();
            return; // Succès, on quitte.
        }
    }
    
    // Si on arrive ici, c'est que la RAM était vide, illisible, ou sans le mot magique.
    // On charge donc les valeurs par défaut[cite: 8] et on sauvegarde la configuration.
    AppConfig_LoadDefaults();
    AppConfig_Save();
}

/*
 * Sauvegarde la configuration actuelle en RAM
 */
void AppConfig_Save(void) {
    // On vérifie les bornes avant d'écrire en mémoire[cite: 8]
    AppConfig_CheckBounds();
    
    RTC_Storage_t storage;
    storage.magic_word = CONFIG_MAGIC_WORD;
    
    // On copie notre variable globale dans la structure de sauvegarde
    memcpy(&storage.config, &current_config, sizeof(AppConfig_t));
    
    // On écrit le tout dans la RAM de l'horloge
    GroveRTC_WriteRAM(0, (uint8_t*)&storage, sizeof(RTC_Storage_t));
}
