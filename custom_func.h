/*_____ I N C L U D E S ____________________________________________________*/
// #include    <stdbool.h>

/*_____ D E C L A R A T I O N S ____________________________________________*/
#define bool  _Bool


#ifndef PASS
#define PASS              					            (1)
#endif

#ifndef FAIL
#define FAIL              					            (0)
#endif

#ifndef OK
#define OK              					            (1)
#endif

#ifndef NG
#define NG              					            (0)
#endif

#ifndef TRUE
#define TRUE              					            (1)
#endif

#ifndef FALSE
#define FALSE              					            (0)
#endif

#ifndef true
#define true              					            (1)
#endif

#ifndef false
#define false              					            (0)
#endif

#ifndef ACTIVE_LOW
#define ACTIVE_LOW              					    (0)
#endif

#ifndef ACTIVE_HIGH
#define ACTIVE_HIGH              					    (1)
#endif

#ifndef HIGH
#define HIGH              					            (1)
#endif

#ifndef LOW
#define LOW              					            (0)
#endif

#ifndef PRESSED
#define PRESSED              					        (1)
#endif

#ifndef RELEASED
#define RELEASED              					        (0)
#endif

/*_____ D E F I N I T I O N S ______________________________________________*/


#define ENABLE_IIC10_CB_ERR_LOG
#define ENABLE_IIC10_RD_ERR_LOG
#define ENABLE_IIC10_WR_ERR_LOG

// #define ENABLE_IIC11_CB_ERR_LOG
// #define ENABLE_IIC11_RD_ERR_LOG
// #define ENABLE_IIC11_WR_ERR_LOG

// #define ENABLE_BUTTON_DEBOUNCE

// button , schematic : pull high
#define BTN1                                            (P9_bit.no0)
#define BTN2                                            (P9_bit.no1)
#define BTN3                                            (P9_bit.no2)
#define BTN4                                            (P9_bit.no3)
#define BTN5                                            (P9_bit.no4)
#define BTN6                                            (P9_bit.no5)
#define BTN7                                            (P9_bit.no6)
#define BTN8                                            (P9_bit.no7)

/*
    touch IC
    INT pin : from slave to master (OUTPUT) , USE P61 , default LOW , active HIGH
    SYNC pin : from master to slave (INPUT) , P32/INTP7 , default HIGH , active LOW
*/

#define TOUCH_INT                                       (P6_bit.no1)    //OUTPUT
#define TOUCH_SYNC                                      (P3_bit.no2)    //INPUT
// #define ENALBE_SPECIFIC_FUNCTION

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


void Timer_TAU0_Channel0_10us_IRQ(void);
void Timer_TAU1_Channel0_1ms_IRQ(void);
void Timer_1ms_IRQ(void);

void delay_ms(unsigned long ms);

void UARTx_Process(unsigned char rxbuf);

void LED_Toggle(void);
void loop(void);

void RL78_soft_reset(unsigned char flag);
void hardware_init(void);
