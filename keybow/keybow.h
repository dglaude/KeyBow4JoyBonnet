#include <bcm2835.h>
#include <pthread.h>
#include "lights.h"
#include "lua-config.h"

#define NUM_KEYS 12
#define NUM_ANAL_KEYS 4

#define ANALOG_THRESH_POS -600
#define ANALOG_THRESH_NEG 600

pthread_mutex_t lights_mutex;

unsigned short last_state[NUM_KEYS];
unsigned short analog_states[NUM_ANAL_KEYS];

int lights_auto;

typedef struct keybow_key {
    unsigned short gpio_bcm;
    unsigned short hid_code;
    unsigned short led_index;
} keybow_key;

unsigned short mapping_table[36];

keybow_key get_key(unsigned short index);
int initUSB();
int initGPIO();
int main();
