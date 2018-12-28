#include "keybow.h"

#ifndef KEYBOW_NO_USB_HID
#include "gadget-hid.h"
#endif

#ifndef KEYBOW_HOME
#define KEYBOW_HOME "/boot/"
#endif

#include "lights.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <bcm2835.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <unistd.h>

int hid_output;
int running = 0;
int key_index = 0;

const char * bcm_err_name[] = { "BCM2835_I2C_REASON_OK", "BCM2835_I2C_REASON_ERROR_NACK", "BCM2835_I2C_REASON_ERROR_CLKT", "BCM2835_I2C_REASON_ERROR_DATA" };

const char * bcm_err_string[] = { "Success", "Received a NACK", "Received Clock Stretch Timeout", "Not all data is sent / received" };

pthread_t t_run_lights;

#define KEYBOW_JOYBONNET

#ifdef KEYBOW_JOYBONNET
/* 
 * This parameter disable four GPIO from the original KeyBow code and
 *  replace this by the I2C scanning for JoyBonnet analog joystic.
 * 
 * It use fake GPIO number of 1000 and above to keep the same code structure.
 * The four direction of the joystic are translated to the missing GPIO.
 * 
 * The code added is inspired by Adafruit JoyBonnet.py that can be found here:
 * https://raw.githubusercontent.com/adafruit/Adafruit-Retrogame/master/joyBonnet.py
 *
 */
#endif

void signal_handler(int dummy) {
    running = 0;
}

keybow_key get_key(unsigned short index){
    keybow_key key;
    index *= 3;
    key.gpio_bcm = mapping_table[index + 0];
    key.hid_code = mapping_table[index + 1];
    key.led_index = mapping_table[index + 2];
    return key;
}

void add_key(unsigned short gpio_bcm, unsigned short hid_code, unsigned short led_index){
    mapping_table[(key_index * 3) + 0] = gpio_bcm;
    mapping_table[(key_index * 3) + 1] = hid_code;
    mapping_table[(key_index * 3) + 2] = led_index;
    key_index+=1;
}

#ifdef KEYBOW_JOYBONNET
/* Init I2C stuff */

/* Strongly inspired by Adafruit JoyBonnet Python code */

uint16_t my_clk_div = BCM2835_I2C_CLOCK_DIVIDER_2500; // This seems to work
uint8_t my_slave_address = 0x48; // I2C address

#define MAX_LEN 4
char my_wvbuf[MAX_LEN];
char my_w0buf[MAX_LEN];
char my_w1buf[MAX_LEN];

char my_rbuf[MAX_LEN];
char my_r0buf[MAX_LEN];
char my_r1buf[MAX_LEN];

int i2c_read_init() {
    // Sequence to request on shoot reading of analog 0:
    my_w0buf[0] = 0x01; my_w0buf[1] = 0xC3; my_w0buf[2] = 0x83;

    // Sequence to request on shoot reading of analog 1:
    my_w1buf[0] = 0x01; my_w1buf[1] = 0xD3; my_w1buf[2] = 0x83;

    // Pointer for reading the analog value:
    my_wvbuf[0] = 0x00;

    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }

    bcm2835_i2c_setSlaveAddress(my_slave_address);
    bcm2835_i2c_setClockDivider(my_clk_div);
    bcm2835_i2c_begin();

    return 0;
}


/*
 *  Reading ADC 0 and ADC 1 with selection and delay.
 */
int prep_adc0() {
    int err=bcm2835_i2c_write(my_w0buf,3);
    if (err) printf("ERROR prep_adc0: %s %s\n", bcm_err_name[err], bcm_err_string[err]);
}

int prep_adc1() {
    int err=bcm2835_i2c_write(my_w1buf,3);
    if (err) printf("ERROR prep_adc1: %s %s\n", bcm_err_name[err], bcm_err_string[err]);
}

int read_adc0() {
    int err=bcm2835_i2c_write(my_wvbuf,1);
    if (err) printf("ERROR read_adc0_1: %s %s\n", bcm_err_name[err], bcm_err_string[err]);
    err = bcm2835_i2c_read(my_r0buf, 2);
    if (err) printf("ERROR read_adc0_2: %s %s\n", bcm_err_name[err], bcm_err_string[err]);

    int ret = ((my_r0buf[0]<<8)+my_r0buf[1])/16;
    return ret;
}

int read_adc1() {
    int err=bcm2835_i2c_write(my_wvbuf,1);
    if (err) printf("ERROR read_adc1_1: %s %s\n", bcm_err_name[err], bcm_err_string[err]);
    err = bcm2835_i2c_read(my_r1buf, 2);
    if (err) printf("ERROR read_adc1_2: %s %s\n", bcm_err_name[err], bcm_err_string[err]);

    int ret = ((my_r1buf[0]<<8)+my_r1buf[1])/16;
    return ret;
}

#endif

int initGPIO() {
    if (!bcm2835_init()){
        return 1;
    }
#ifdef KEYBOW_JOYBONNET
    i2c_read_init();
#endif
    int x = 0;
    for(x = 0; x < NUM_KEYS; x++){
        keybow_key key = get_key(x);
#ifdef KEYBOW_JOYBONNET
        if (key.gpio_bcm<1000){
#endif
            bcm2835_gpio_fsel(key.gpio_bcm, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_set_pud(key.gpio_bcm, BCM2835_GPIO_PUD_UP);
#ifdef KEYBOW_JOYBONNET
        }
#endif
    }
    return 0;
}

int updateKeys() {
    int state;
    int x = 0;
    for(x = 0; x < NUM_KEYS; x++){
        keybow_key key = get_key(x);
#ifdef KEYBOW_JOYBONNET
        if (key.gpio_bcm<1000){
#endif
            state = bcm2835_gpio_lev(key.gpio_bcm) == 0;
#ifdef KEYBOW_JOYBONNET
        } else {
            state = analog_states[key.gpio_bcm-1000];
        }
#endif
        if(state != last_state[x]){
            luaHandleKey(x, state);            
        }
        last_state[x] = state;
    }
}

#ifdef KEYBOW_JOYBONNET

int joystic_y(int ads) {
  int y = 800 - ads;

  if (y > ANALOG_THRESH_POS) analog_states[0] = 0;
  if (y < ANALOG_THRESH_POS) analog_states[0] = 1;
  if (y < ANALOG_THRESH_NEG) analog_states[1] = 0;
  if (y > ANALOG_THRESH_NEG) analog_states[1] = 1;
}

int joystic_x(int ads) {
  int x = ads - 800;

  if (x < ANALOG_THRESH_NEG) analog_states[2] = 0;
  if (x > ANALOG_THRESH_NEG) analog_states[2] = 1;
  if (x > ANALOG_THRESH_POS) analog_states[3] = 0;
  if (x < ANALOG_THRESH_POS) analog_states[3] = 1;
}

#endif


void *run_lights(void *void_ptr){
    while(running){
        int delta = (millis() / (1000/60)) % height;
        if (lights_auto) {
            pthread_mutex_lock( &lights_mutex );
            lights_drawPngFrame(delta);
            pthread_mutex_unlock( &lights_mutex );
        }
        lights_show();
        usleep(16666); // About 60fps
    }
}

int main() {
    int ret;
    chdir(KEYBOW_HOME);

    pthread_mutex_init ( &lights_mutex, NULL );

    add_key(RPI_V2_GPIO_P1_38, 0x24, 0);
    add_key(RPI_V2_GPIO_P1_31, 0x21, 1);
    add_key(RPI_V2_GPIO_P1_15, 0x1e, 2);
    add_key(RPI_V2_GPIO_P1_36, 0x25, 4);
    add_key(RPI_V2_GPIO_P1_32, 0x22, 5);
    add_key(RPI_V2_GPIO_P1_37, 0x26, 8);
    add_key(RPI_V2_GPIO_P1_33, 0x23, 9);
    add_key(RPI_V2_GPIO_P1_16, 0x28, 11);
#ifdef KEYBOW_JOYBONNET
    add_key(1000, 0x27, 3);
    add_key(1001, 0x37, 7);
    add_key(1002, 0x1f, 6);
    add_key(1003, 0x20, 10);
#else
    add_key(RPI_V2_GPIO_P1_11, 0x27, 3);
    add_key(RPI_V2_GPIO_P1_13, 0x37, 7);
    add_key(RPI_V2_GPIO_P1_18, 0x1f, 6);
    add_key(RPI_V2_GPIO_P1_29, 0x20, 10);
#endif

    if (initGPIO() != 0) {
        return 1;
    }

#ifndef KEYBOW_NO_USB_HID
    ret = initUSB();
    //if (ret != 0 && ret != USBG_ERROR_EXIST) {
    //    return 1;
    //}

    do {
        hid_output = open("/dev/hidg0", O_WRONLY | O_NDELAY);
    } while (hid_output == -1 && errno == EINTR);
    if (hid_output == -1){
        printf("Error opening /dev/hidg0 for writing.\n");
        return 1;
    }
#else
    printf("Opening /dev/null for output.\n");
    hid_output = open("/dev/null", O_WRONLY);
#endif

#ifdef KEYBOW_DEBUG
    printf("Initializing LUA\n");
#endif

    ret = initLUA();
    if (ret != 0){
        return ret;
    }

    int x = 0;
    for(x = 0; x < NUM_KEYS; x++){
        last_state[x] = 0;
    }

#ifdef KEYBOW_JOYBONNET
    for(x = 0; x < NUM_ANAL_KEYS; x++){
        analog_states[x] = 0;
    }
#endif

    lights_auto = 1;

#ifdef KEYBOW_DEBUG
    printf("Initializing Lights\n");
#endif
    initLights();
    read_png_file("default.png");

    printf("Running...\n");
    running = 1;
    signal(SIGINT, signal_handler);

    luaCallSetup();

    if(pthread_create(&t_run_lights, NULL, run_lights, NULL)) {
        printf("Error creating lighting thread.\n");
        return 1;
    }

    while (running){
        /*int delta = (millis() / (1000/60)) % height;
        if (lights_auto) {
            lights_drawPngFrame(delta);
        }
        lights_show();*/

        updateKeys();
#ifndef KEYBOW_JOYBONNET
        usleep(1000);
#else
        prep_adc0();
        usleep(500);
        joystic_y(read_adc0());

        prep_adc1();
        usleep(500);
        joystic_x(read_adc1());
#endif
        //usleep(250000);
    }      

    pthread_join(t_run_lights, NULL);

    //cleanupUSB();
    bcm2835_close();
    luaClose();
    return 0;
}

