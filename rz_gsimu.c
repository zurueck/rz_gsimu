#include <stdio.h>
#include "rz_gsimu.h"

static struct GSU_ctrler *s_gsu;
unsigned int g_gsu_timeout_Flag;

//tx rx
void gsu_isr_callback() {
    static int s_preCnt = 0;
    static int s_byteCnt;
    static int s_cntTarget;
    unsigned char bit;

    s_preCnt++;

    if (s_gsu->status == GSU_IDLE) {
        s_cntTarget = s_preCnt + 8;
        return;
    }
    if (s_preCnt == s_cntTarget) {
        s_byteCnt++;
        s_cntTarget += 8;
    } 
    if (s_gsu->status == GSU_TX) {
        if (s_byteCnt == 0) {
            s_gsu->set_txio(0);
        } else if (s_byteCnt >= 1 && s_byteCnt <= 8) {
            bit = s_gsu->txData && 0x01;
            s_gsu->txData = s_gsu->txData >> 1;
            s_gsu->set_txio(bit);
        } else if (s_byteCnt == 9) {
            /* stop bit */
            s_gsu->set_txio(1);
        } else if (s_byteCnt >= 10) {
            s_byteCnt = 0 ;
            s_gsu->status = GSU_IDLE;
            s_gsu->txDoneFlag = 1;
        }
    } else if (s_gsu->status == GSU_RX) {
        if (s_byteCnt == 0) {
            if (s_gsu->get_rxio()) {
                s_cntTarget = s_preCnt + 8;
            }
        } else if (s_byteCnt >= 1 && s_byteCnt <= 8) {
            s_gsu->rxData = s_gsu->rxData << 1;
            if (s_gsu->get_rxio()) {
                s_gsu->rxData |= 1;
            } else {
                s_gsu->rxData &= ~1;
            }
        } else if (s_byteCnt == 9) {
            /* stop bit */
        } else if (s_byteCnt >= 10) {
            s_byteCnt = 0 ;
            s_gsu->status = GSU_IDLE;
            s_gsu->rxDoneFlag = 1;
        }
    }
} //isr call back


//public
int gsu_rxByte(unsigned char *c)
{
    s_gsu->rxDoneFlag = 0;
    g_gsu_timeout_Flag = 0;
    s_gsu->status = GSU_RX;
#ifdef RZ_GSIMU_DEFAULT_TIMEOUT_COUNTER
    g_gsu_timeout_Flag = 20000;
    while (g_gsu_timeout_Flag) {
#else
    g_gsu_timeout_Flag = 0;
    while (!g_gsu_timeout_Flag) {
#endif
        if (s_gsu->rxDoneFlag) {
            *c = s_gsu->rxData;
            s_gsu->rxDoneFlag = 0;
            return 0;
        }
#ifdef RZ_GSIMU_DEFAULT_TIMEOUT_COUNTER
        g_gsu_timeout_Flag--;
#endif
    }
    return -1;
}


//public
void gsu_txbyte(unsigned char data)
{
    // start bit
    s_gsu->txData = data;
    s_gsu->txDoneFlag = 0;
    s_gsu->status = GSU_TX;
    while(!s_gsu->txDoneFlag);
}

//public
void GSU_ctrler_link(struct GSU_ctrler *ctrler)
{
    s_gsu->status = GSU_IDLE;
    ctrler->status = GSU_IDLE;
    s_gsu = ctrler;

    s_gsu->timerStart();
}

