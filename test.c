/*
    1. implement timerStart - autoreload, a perios = baurdrate/4, intt enable
       1/115200 = 8.68us, 8.68/4 = 2.17us
       (for 12M clock, it will be 26 clocks)
    2. implement setGPIO
    3. create struct GSU_ctrler
    4. put callback function into timer isr
    
    5.1 OPTION, set g_gsu_timeout_flag = 0 if not necessary
    5.2 OPTION, make g_gsu_timeout_flag workable if necessary

    tx use
    1. call link function
    2. send byte
    
    rx
    wait for 
*/

#include "rz_gsimu.h"

//e.g.
void ISR_timer3() {

    //clear flag
    //gsu_isr_callback();
}

//e.g.
void setGPIO(int value) {
    /* porting here */    
}

//e.g.
int getGPIO(void) {
    /* porting here */    
}

//e.g.
void timerStart() {
    /* porting here */    
}

//e.g.
struct GSU_ctrler myGSU = {
    .set_txio = setGPIO,
    .get_rxio = getGPIO,
    .timerStart = timerStart,
};


void main() {
    unsigned char a;
    int rc;
    
    GSU_ctrler_link(&myGSU);

    gsu_txByte(0xaa);
    rc = gsu_rxByte(&a);
}


