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

#define GSU_IDLE 0
#define GSU_TX 1
#define GSU_RX 2

struct GSU_ctrler{
    void (*set_txio)(unsigned char);
    int (*get_rxio)(void);
    void (*timerStart)(void);
    char txDoneFlag;
    char rxDoneFlag;
    char txData;
    char rxData;
    unsigned int cnt;
    unsigned int nowStage;
};

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
void getGPIO(int value) {
    /* porting here */    
}

//e.g.
void timerStart() {
    /* porting here */    
}

//e.g.
struct GSU_ctrler myGSU;


//======
static struct GSU_ctrler *s_gsu;
unsigned int g_gsu_timeout_Flag;

//tx rx
void gsu_isr_callback() {
    unsigned char bit;
    unsigned char tmp;
    static int preCnt = 0;

    preCnt++;
    if (preCnt % 4 == 2) {
        s_gsu->cnt++;
    }
    if (s_gsu->nowStage == GSU_TX) {
        if (s_gsu->cnt >= 1 && s_gsu->cnt <= 8) {
            bit = s_gsu->txData && 0x01;
            s_gsu->txData = s_gsu->txData >> 1;
            s_gsu->set_txio(bit);
        } else if (s_gsu->cnt == 9) {
            /* stop bit */
            s_gsu->set_txio(1);
        } else if (s_gsu->cnt >= 10) {
            s_gsu->cnt = 0 ;
            s_gsu->nowStage = GSU_IDLE;
            s_gsu->txDoneFlag = 1;
        }
    } else if (s_gsu->nowStage == GSU_RX) {
        if (s_gsu->cnt >= 1 && s_gsu->cnt <= 8) {
            tmp = tmp << 1;
            if (s_gsu->get_rxio()) {
                tmp |= 1;
            } else {
                tmp &= ~1;
            }
        } else if (s_gsu->cnt == 9) {
            /* stop bit */
        } else if (s_gsu->cnt >= 10) {
            s_gsu->cnt = 0 ;
            s_gsu->rxData = tmp;
            s_gsu->nowStage = GSU_IDLE;
            s_gsu->rxDoneFlag = 1;
        }
    }
} //isr call back

//public
void gsu_rxByte() {
    s_gsu->rxDoneFlag = 0;
    s_gsu->nowStage = GSU_RX;
    while(s_gsu->rxDoneFlag == 0 && g_gsu_timeout_Flag == 0);
}


//public
void gsu_txbyte(unsigned char data) 
{
    // start bit
    s_gsu->txData = data;
    s_gsu->txDoneFlag = 0;
    s_gsu->set_txio(0);
    s_gsu->nowStage = GSU_TX;
    while(s_gsu->txDoneFlag == 0 && g_gsu_timeout_Flag == 0);
}

//public
void GSU_ctrler_link(struct GSU_ctrler *ctrler) {
    s_gsu->nowStage = GSU_IDLE;
    ctrler->nowStage = GSU_IDLE;
    s_gsu = ctrler;

    s_gsu->timerStart();
}








