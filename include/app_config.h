#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Structure regroupant tous les paramètres du système
typedef struct {
    uint16_t log_interval;
    uint32_t file_max_size;
    uint16_t timeout;
    
    // Capteur Lumière
    bool     lumin;
    uint16_t lumin_low;
    uint16_t lumin_high;
    
    // Capteur Température
    bool     temp_air;
    int8_t   min_temp_air;
    int8_t   max_temp_air;
    
    // Capteur Humidité
    bool     hygr;
    int8_t   hygr_mint;
    int8_t   hygr_maxt;
    
    // Capteur Pression
    bool     pressure;
    uint16_t pressure_min;
    uint16_t pressure_max;
} AppConfig_t;

// Variable globale accessible par tout le programme
extern AppConfig_t current_config;

// Prototypes des fonctions
void AppConfig_Init(void);
void AppConfig_LoadDefaults(void);
void AppConfig_Save(void);
void AppConfig_CheckBounds(void);

#endif // APP_CONFIG_H
