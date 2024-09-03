/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_cg_userdefine.h"

#include "misc_config.h"
#include "custom_func.h"

#include "simple_i2c_driver.h"
#include "IICA0_slave_driver.h"
#include "button_sw_debounce.h"

#include "CRC8_SAE_J1850_ZERO.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_8bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_REVERSE1                              (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 	            (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned long counter_tick = 0;
volatile unsigned long btn_counter_tick = 0;
volatile unsigned long counter_tau1_ch0_tick = 0;


#define BTN_PRESSED_LONG                                (2500)

#define BUILD_TIME()                                    {printf("\r\n[build time]%s,%s\r\n", __DATE__ ,__TIME__ );}

// #define I2C_SLV_ADDR_7BIT                               (0x50)	// 7BIT
// #define I2C_SLV_ADDR_8BIT                               (I2C_SLV_ADDR_7BIT << 1)    // 0x28


/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
// extern volatile uint8_t   g_iica0_slave_status_flag;

unsigned long btn_get_tick(void)
{
	return (btn_counter_tick);
}

void btn_set_tick(unsigned long t)
{
	btn_counter_tick = t;
}

void btn_tick_counter(void)
{
	btn_counter_tick++;
    if (btn_get_tick() >= 60000)
    {
        btn_set_tick(0);
    }
}

unsigned long get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned long t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

void Timer_TAU1_Channel0_1ms_IRQ(void)
{
    counter_tau1_ch0_tick++;
}


void delay_ms(unsigned long ms)
{
	#if 1

    R_TAU1_Channel0_Start();
    counter_tau1_ch0_tick = 0;

    while(counter_tau1_ch0_tick < ms);

    R_TAU1_Channel0_Stop();

    #else
    unsigned long tickstart = get_tick();
    unsigned long wait = ms;
	unsigned long tmp = 0;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000 -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
	
	#endif
}


void CRC_TEST(void)
{
    unsigned char buffer[16] = {0};
    unsigned char res = 0l;
    unsigned char len = 0l;

    /*
        0x50 
        [0x83 0x00] 
        0x1D
    */
    len = 2;
    buffer[0] = 0x83;
    buffer[1] = 0x00;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST 0 = 0x%02X\r\n",res);
    
    /*
        0x50 
        [0x00 0x00]
        [0x00 0x3C 0x00 0x3C 0x00] 
        0x0B
    */
    len = 7;
    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    buffer[3] = 0x3C;
    buffer[4] = 0x00;
    buffer[5] = 0x3C;
    buffer[6] = 0x00;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST aa = 0x%02X\r\n",res);


    /*
        0x50 
        [0x00 0x00] 
        [0x00 0x7F 0xA3 0x7F 0x00] 
        0x67
    */
    len = 7;
    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    buffer[3] = 0x7F;
    buffer[4] = 0xA3;
    buffer[5] = 0x7F;
    buffer[6] = 0x00;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST bb = 0x%02X\r\n",res);

    /*
        0x50 
        [0x83 0x01]
        [0xFF 0x01 0xFF 0x03 0xFF 0x05 0x07] 
        0xDA
    */
    len = 2 + 7*1;
    buffer[0] = 0x83;
    buffer[1] = 0x01;

    buffer[2] = 0xFF;
    buffer[3] = 0x01;
    buffer[4] = 0xFF;
    buffer[5] = 0x03;
    buffer[6] = 0xFF;
    buffer[7] = 0x05;
    buffer[8] = 0x07;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST 1-1 = 0x%02X\r\n",res);

    /*
        0x50 
        [0x83 0x01] 
        [0x7F 0x02 0x7F 0x04 0x7F 0x06 0x17] 
        0xBF
    */
    len = 2 + 7*1;
    buffer[0] = 0x83;
    buffer[1] = 0x01;

    buffer[2] = 0x7F;
    buffer[3] = 0x02;
    buffer[4] = 0x7F;
    buffer[5] = 0x04;
    buffer[6] = 0x7F;
    buffer[7] = 0x06;
    buffer[8] = 0x17;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST 1-2 = 0x%02X\r\n",res);


    /*
        0x50 
        [0x83 0x02] 
        [0x01 0x02 0x03 0x04 0x05 0x06 0x07] 
        [0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E] 
        0x1E
    */
    len = 2 + 7*2;
    buffer[0] = 0x83;
    buffer[1] = 0x02;

    buffer[2] = 0x01;
    buffer[3] = 0x02;
    buffer[4] = 0x03;
    buffer[5] = 0x04;
    buffer[6] = 0x05;
    buffer[7] = 0x06;
    buffer[8] = 0x07;
    
    buffer[9] = 0x08;
    buffer[10] = 0x09;
    buffer[11] = 0x0A;
    buffer[12] = 0x0B;
    buffer[13] = 0x0C;
    buffer[14] = 0x0D;
    buffer[15] = 0x0E;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST 2-1 = 0x%02X\r\n",res);


    /*
        0x50 
        [0x83 0x02] 
        [0x11 0x12 0x13 0x14 0x15 0x16 0x17] 
        [0x28 0x29 0x2A 0x2B 0x2C 0x2D 0x2E]         
        0x9E
    */
    len = 2 + 7*2;
    buffer[0] = 0x83;
    buffer[1] = 0x02;

    buffer[2] = 0x11;
    buffer[3] = 0x12;
    buffer[4] = 0x13;
    buffer[5] = 0x14;
    buffer[6] = 0x15;
    buffer[7] = 0x16;
    buffer[8] = 0x17;
    
    buffer[9] = 0x28;
    buffer[10] = 0x29;
    buffer[11] = 0x2A;
    buffer[12] = 0x2B;
    buffer[13] = 0x2C;
    buffer[14] = 0x2D;
    buffer[15] = 0x2E;

    res = Crc8_saeJ1850Zero(&buffer[0],len);

    printf("CRC TEST 2-2 = 0x%02X\r\n",res);
}

void Timer_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000) == 0)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
    }

    if ((get_tick() % 50) == 0)
    {

    }	

    #if defined (ENABLE_BUTTON_DEBOUNCE)
    btn_timer_irq();
    #endif

}


/*
    F13 target board
    LED1 connected to P66, LED2 connected to P67
*/
// void LED_Toggle(void)
// {
//     // PIN_WRITE(6,6) = ~PIN_READ(6,6);
//     // PIN_WRITE(6,7) = ~PIN_READ(6,7);
//     P6_bit.no6 = ~P6_bit.no6;
//     P6_bit.no7 = ~P6_bit.no7;
// }

void loop(void)
{
	static unsigned long LOG1 = 0;
	static unsigned long specific_cnt = 0;

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;

        if (++specific_cnt == 10)
        {
            printf("log(timer):%4d\r\n",LOG1++);
            specific_cnt = 0;
        }          
    }

    #if defined (ENABLE_BUTTON_DEBOUNCE)
    btn_task();
    #endif
    

}

// F24 EVB , P137/INTP0 , set both edge 
// void Button_Process_long_counter(void)
// {
//     if (FLAG_PROJ_TRIG_BTN2)
//     {
//         btn_tick_counter();
//     }
//     else
//     {
//         btn_set_tick(0);
//     }
// }

// void Button_Process_in_polling(void)
// {
//     static unsigned char cnt = 0;

//     if (FLAG_PROJ_TRIG_BTN1)
//     {
//         FLAG_PROJ_TRIG_BTN1 = 0;
//         printf("BTN pressed(%d)\r\n",cnt);

//         if (cnt == 0)
//         {
//             FLAG_PROJ_TRIG_BTN2 = 1;
//         }
//         else if (cnt == 1)
//         {
//             FLAG_PROJ_TRIG_BTN2 = 0;
//         }

//         cnt = (cnt >= 1) ? (0) : (cnt+1) ;
//     }

//     if ((FLAG_PROJ_TRIG_BTN2 == 1) && 
//         (btn_get_tick() > BTN_PRESSED_LONG))
//     {         
//         btn_set_tick(0);

//     }
// }

// F13 EVB , P137/INTP0
// void Button_Process_in_IRQ(void)    
// {
//     FLAG_PROJ_TRIG_BTN1 = 1;
// }

void UARTx_Process(unsigned char rxbuf)
{    
    // unsigned char tmp = 0;

    if (rxbuf > 0x7F)
    {
        printf("invalid command\r\n");
    }
    else
    {
        printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {

            case 'X':
            case 'x':
                RL78_soft_reset(7);
                break;
            case 'Z':
            case 'z':
                RL78_soft_reset(1);
                break;
        }
    }
}


/*
    7:Internal reset by execution of illegal instruction
    1:Internal reset by illegal-memory access
*/
//perform sofware reset
void _reset_by_illegal_instruction(void)
{
    static const unsigned char illegal_Instruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegal_Instruction;
    dummy();
}
void _reset_by_illegal_memory_access(void)
{
    #if 1
    const unsigned char ILLEGAL_ACCESS_ON = 0x80;
    IAWCTL |= ILLEGAL_ACCESS_ON;            // switch IAWEN on (default off)
    *(__far volatile char *)0x00000 = 0x00; //write illegal address 0x00000(RESET VECTOR)
    #else
    signed char __far* a;                   // Create a far-Pointer
    IAWCTL |= _80_CGC_ILLEGAL_ACCESS_ON;    // switch IAWEN on (default off)
    a = (signed char __far*) 0x0000;        // Point to 0x000000 (FLASH-ROM area)
    *a = 0;
    #endif
}

void RL78_soft_reset(unsigned char flag)
{
    switch(flag)
    {
        case 7: // do not use under debug mode
            _reset_by_illegal_instruction();        
            break;
        case 1:
            _reset_by_illegal_memory_access();
            break;
    }
}


// retarget printf
int __far putchar(int c)
{
    // F13 , UART0
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SDR00L = (unsigned char)c;
    while(STIF0 == 0)
    {

    }
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    return c;
}

void hardware_init(void)
{
    // const unsigned char indicator[] = "hardware_init";
    EI();   //BSP_EI();
    R_UART0_Start();            // P15:TXD0, P16:RXD0
    R_TMR_RJ0_Start();  

    #if defined (ENABLE_BUTTON_DEBOUNCE)
    btn_init();
    #endif

    /*
        I2C slave  - IICA0
        23 : P62/SCL0
        24 : P63/SDA0      
    */

    // CRC_TEST();

    IICA0_slave_Init();    
    
    BUILD_TIME();
    printf("\r\n%s finish\r\n",__func__);
}
