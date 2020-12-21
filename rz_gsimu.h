#ifndef RZ_GSIMU_H
#define RZ_GSIMU_H

#define GSU_IDLE 0
#define GSU_TX 1
#define GSU_RX 2

#define RZ_GSIMU_DEFAULT_TIMEOUT_COUNTER

struct GSU_ctrler
{
    void (*set_txio)(int);
    int (*get_rxio)(void);
    void (*timerStart)(void);
    char txDoneFlag;
    char rxDoneFlag;
    char txData;
    char rxData;
    unsigned int status;
};

//== API
int gsu_rxByte(unsigned char *c);
void gsu_txbyte(unsigned char data) ;
void GSU_ctrler_link(struct GSU_ctrler *ctrler);

#endif 
