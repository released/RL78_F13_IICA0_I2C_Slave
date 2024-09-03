/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_cg_userdefine.h"

#include "misc_config.h"
#include "custom_func.h"

#include "IICA0_slave_driver.h"

#include "CRC8_SAE_J1850_ZERO.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_8bit flag_IICA0_SLAVE_CTL;
#define FLAG_IICA0_SLAVE_TRIG_I2C_SEND                 		   	(flag_IICA0_SLAVE_CTL.bit0)
#define FLAG_IICA0_SLAVE_TRIG_I2C_RCV                       	(flag_IICA0_SLAVE_CTL.bit1)
#define FLAG_IICA0_SLAVE_I2C_REG_WR     				        (flag_IICA0_SLAVE_CTL.bit2)
#define FLAG_IICA0_SLAVE_I2C_REG_RD                		        (flag_IICA0_SLAVE_CTL.bit3)
#define FLAG_IICA0_SLAVE_I2C_START_RCV                       	(flag_IICA0_SLAVE_CTL.bit4)
#define FLAG_IICA0_SLAVE_I2C_MASTER_RDY_TO_RCV                  (flag_IICA0_SLAVE_CTL.bit5)
#define FLAG_IICA0_SLAVE_I2C_REVERSE6                           (flag_IICA0_SLAVE_CTL.bit6)
#define FLAG_IICA0_SLAVE_I2C_REVERSE7                           (flag_IICA0_SLAVE_CTL.bit7)

/*_____ D E F I N I T I O N S ______________________________________________*/

unsigned char g_tx_data[TX_BUFFER_LEN] = {0};
unsigned char g_rx_data[RX_BUFFER_LEN] = {0};
unsigned short g_iica0_cal_rx_len = 0;

unsigned char slv_decode_buffer1[PACKET_DATA_LEN] = {0};
unsigned char slv_decode_buffer2[PACKET_DATA_LEN*2] = {0};

unsigned char z_1 = 0x5A;
unsigned char z_2 = 0xA5;
unsigned char total_force = 0x7F;

typedef enum  
{
    TOUCH_NO_0 = 0,
    TOUCH_NO_1,
    TOUCH_NO_2,
}NUMBER_OF_TOUCH_t;


typedef enum  
{
    PACKET_WR_IDX_0 = 0,    // WR cmd , reg byte , 
    PACKET_WR_IDX_1,        // WR cmd , number of touch
}PACKET_IDX_t;


/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern volatile uint8_t   g_iica0_slave_status_flag;            /* iica0 slave flag */
extern volatile uint8_t * gp_iica0_rx_address;                  /* iica0 receive buffer address */
extern volatile unsigned short  g_iica0_rx_cnt;                 /* iica0 receive data length */
extern volatile unsigned short  g_iica0_rx_len;                 /* iica0 receive data count */
extern volatile uint8_t * gp_iica0_tx_address;                  /* iica0 send buffer address */
extern volatile unsigned short  g_iica0_tx_cnt;                 /* iica0 send data count */

extern volatile unsigned long counter_tau0_ch0_tick;
extern volatile unsigned long target_timeout_tick;


// for driver level
MD_STATUS drv_IICA0_slave_read(unsigned char * const rx_buf, unsigned short rx_num)
{
	MD_STATUS ret = MD_OK;

	R_IICA0_Slave_Receive(rx_buf, rx_num);

	if (ret != MD_OK)
    {
        printf("[%s error]0x%02X\r\n" , __func__ , ret);
		return ret;
	} 
    return ret;
}


MD_STATUS drv_IICA0_slave_write(unsigned char * const tx_buf, unsigned short tx_num)
{
	MD_STATUS ret = MD_OK;

    R_IICA0_Slave_Send(tx_buf , tx_num );

	if (ret != MD_OK)
    {
        printf("[%s error]0x%02X\r\n" , __func__ , ret);
		return ret;
	} 

    return ret;
}

bool drv_Is_IICA0_slave_bus_busy(void)
{
    return IICBSY0;
}

void drv_IICA0_slave_callback_error(unsigned char err)
{
    printf("IICA0 MD_STATUS:0x%02X\r\n",err);
}

void drv_set_IICA0_slave_receive_flag(bool flag)
{
    FLAG_IICA0_SLAVE_TRIG_I2C_RCV = flag;
}
bool drv_get_IICA0_slave_receive_flag(void)
{
    return FLAG_IICA0_SLAVE_TRIG_I2C_RCV;
}

void drv_set_IICA0_slave_send_flag(bool flag)
{
    FLAG_IICA0_SLAVE_TRIG_I2C_SEND = flag;
}
bool drv_get_IICA0_slave_send_flag(void)
{
    return FLAG_IICA0_SLAVE_TRIG_I2C_SEND;
}

// for application

// MD_STATUS IICA0_slave_read(unsigned char* rx_xfer_data,unsigned short rx_num)
// {
// 	MD_STATUS ret = MD_OK;
//     // unsigned char tmp = 0;

//    // while (drv_get_IICA0_slave_send_flag() || drv_Is_IICA0_slave_bus_busy()){ ; } 	//Make sure bus is ready for xfer

// 	// drv_set_IICA0_slave_send_flag(1);
// 	// ret = drv_IICA0_slave_write(&tmp, 1);
// 	// if (ret != MD_OK)
//     // {
//     //     printf("[IICA0_slave_read error1]0x%02X\r\n" , ret);
// 	// 	return ret;
// 	// } 
// 	// while (drv_get_IICA0_slave_send_flag() || drv_Is_IICA0_slave_bus_busy()){ ; } 	//Wait until the xfer is complete

// 	drv_set_IICA0_slave_receive_flag(1);
// 	ret = drv_IICA0_slave_read(rx_xfer_data, rx_num);
// 	while (drv_get_IICA0_slave_receive_flag() || drv_Is_IICA0_slave_bus_busy()){ ; } 	//Wait until the xfer is complete
// 	if (ret != MD_OK)
//     {
//         printf("[IICA0_slave_read error2]0x%02X\r\n" , ret);
// 		return ret;
// 	} 

//     // delay_ms(5);

//     return ret;
// }

// MD_STATUS IICA0_slave_write(unsigned char* tx_xfer_data,unsigned short tx_num)
// {
// 	MD_STATUS ret = MD_OK;

//     if (tx_num > 128)
//     {
//         printf("[IICA0_slave_write]tx_num over range(%2d)\r\n",tx_num);

//         return MD_ARGERROR;
//     }

//    // while (drv_get_IICA0_slave_send_flag() || drv_Is_IICA0_slave_bus_busy()){ ; } 	//Make sure bus is ready for xfer

// 	drv_set_IICA0_slave_send_flag(1);
// 	ret = drv_IICA0_slave_write(tx_xfer_data , tx_num);   // reg byte + data byte , at least 2 bytes
// 	if (ret != MD_OK)
//     {
//         printf("[IICA0_slave_write error1]0x%02X\r\n" , ret);
// 		return ret;
// 	} 
// 	while (drv_get_IICA0_slave_send_flag() || drv_Is_IICA0_slave_bus_busy()){ ; } 	//Wait until the xfer is complete
   
//     // delay_ms(5);

//     return ret;
// }


unsigned char CRC_cal(unsigned char* target_cal_data,unsigned char target_cal_len)
{    
    unsigned char res = 0l;

    res = Crc8_saeJ1850Zero(&target_cal_data[0],target_cal_len);

    // printf("CRC cal = 0x%02X\r\n",res);

    return res;    
}

void touch_int_pin_trig(void)
{
    TOUCH_INT = 1;

    NOP();
    NOP();

    TOUCH_INT = 0;
}


void IICA0_slave_Init(void)
{
    
    TOUCH_INT = 0;

	g_iica0_rx_len = 0xFF;                  /* iica0 receive data length */
	g_iica0_rx_cnt = 0;                     /* iica0 receive data count */
    g_iica0_tx_cnt = 0xFF;                  /* iica0 send data count */

    FLAG_IICA0_SLAVE_I2C_REG_RD = 0;
    FLAG_IICA0_SLAVE_I2C_REG_WR = 0;
    FLAG_IICA0_SLAVE_I2C_MASTER_RDY_TO_RCV = 0;
}

void IICA0_slave_encode(void)
{
    static unsigned char cnt = 0x10;
    unsigned char counter = 0x00;    
    unsigned char CRC = 0x00;
                    
    // prepare data
    g_iica0_tx_cnt = TX_DATA_LEN;

    // g_tx_data[counter++] = 0x00;  //reg
    g_tx_data[counter++] = 0x00;
    g_tx_data[counter++] = 0x00;
    g_tx_data[counter++] = z_1;  //z_1
    g_tx_data[counter++] = z_2;  //z_2
    g_tx_data[counter++] = total_force + cnt;  //total force
    g_tx_data[counter++] = 0x00;

    CRC = Crc8_saeJ1850Zero( (unsigned char*)  &g_tx_data[0],6);
    g_tx_data[counter++] = CRC;

    cnt += 0x10;    // test cnt

    gp_iica0_tx_address = (uint8_t*) g_tx_data; // assign addr to drvier
}

void IICA0_slave_decode(unsigned char* target_decode_data)
{
    const unsigned char len_base = 3;
    const unsigned char len = PACKET_DATA_LEN;
    unsigned char byte_2 = target_decode_data[1];
    unsigned char CRC_target = 0;
    unsigned char CRC_actual = 0;
    unsigned char flag_error = 0;
    unsigned char i = 0;

    // printf("\r\n----START----\r\n");
    // printf("byte1(reserve):0x%02X\r\n",byte_1);

    // CHECK CRC result , if NG , bypass this packet
    switch(byte_2)
    {
        case TOUCH_NO_0:
            /*
                data packet base 
                0x50 
                [0x83 0x00] 
                0x1D
            */

            CRC_target = CRC_cal(&target_decode_data[0], (len_base-1) + len*byte_2 );    // len : 2
            CRC_actual = target_decode_data[(len_base-1) + len*byte_2];  // index : 2
            if (CRC_target != CRC_actual)  
            {
                flag_error = 1;

                printf("CRC ERROR-0:0x%02X\r\n",CRC_target);
                dump_buffer(target_decode_data, (len_base) + len*byte_2);
            }

            break;

        case TOUCH_NO_1:
            /*
                0x50 
                [0x83 0x01]
                [0xFF 0x01 0xFF 0x03 0xFF 0x05 0x07] 
                0xDA
            */

            CRC_target = CRC_cal(&target_decode_data[0], (len_base-1) + len*byte_2 );    // len : 2+7
            CRC_actual = target_decode_data[(len_base-1) + len*byte_2];  // index : 2+7
            if (CRC_target != CRC_actual)
            {
                flag_error = 1;

                printf("CRC ERROR-1:0x%02X\r\n",CRC_target);
                dump_buffer(target_decode_data, (len_base) + len*byte_2);
            }

            break;

        case TOUCH_NO_2:
            /*
                0x50 
                [0x83 0x02] 
                [0x01 0x02 0x03 0x04 0x05 0x06 0x07] 
                [0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E] 
                0x1E
            */

            CRC_target = CRC_cal(&target_decode_data[0], (len_base-1) + len*byte_2 );    // len : 2+7*2
            CRC_actual = target_decode_data[(len_base-1) + len*byte_2];  // index : 2+7*2
            if (CRC_target != CRC_actual)
            {
                flag_error = 1;

                printf("CRC ERROR-2:0x%02X\r\n",CRC_target);
                dump_buffer(target_decode_data, (len_base) + len*byte_2);
            }

            break;
    }

    if (!flag_error)
    {
        // printf("byte2(sensor):0x%02X\r\n",byte_2);  
        switch(byte_2)
        {      
            case TOUCH_NO_0:
                #if defined (ENABLE_I2C_SLAVE_RCV_LOG0)
                printf("(0):\r\nEMPTY\r\n");
                #endif

                break;

            case TOUCH_NO_1:
                memcpy((void*) &slv_decode_buffer1[0], (void*) &target_decode_data[2], len);

                #if defined (ENABLE_I2C_SLAVE_RCV_LOG1)   // dump log
                printf("(1):\r\n");
                //dump_buffer(slv_decode_buffer1,len);

                for(i = 0 ; i < (len*byte_2) ; i++)
                {
                    printf("0x%02X,", slv_decode_buffer1[i] );
                }
                printf("\r\n");
                #endif

                break;
            case TOUCH_NO_2:
                memcpy((void*) &slv_decode_buffer2[0], (void*) &target_decode_data[2], len);
                memcpy((void*) &slv_decode_buffer2[7], (void*) &target_decode_data[9], len);
                
                #if defined (ENABLE_I2C_SLAVE_RCV_LOG2)   // dump log
                printf("(2):\r\n");
                //dump_buffer(slv_decode_buffer1,len);

                for(i = 0 ; i < (len*byte_2) ; i++)
                {
                    printf("0x%02X,", slv_decode_buffer2[i] );
                }
                printf("\r\n");

                #endif

                break;
        }
    }

    // printf("----EOL----\r\n");
}

void IICA0_slave_data_process(void)
{

    if (FLAG_IICA0_SLAVE_I2C_REG_WR)
    {
        if (drv_get_IICA0_slave_receive_flag())
        {
            if ( (g_rx_data[PACKET_WR_IDX_0] == REG_WR_0x83)  &&
                (g_rx_data[PACKET_WR_IDX_1]  <= TOUCH_NO_2) )
            {
                IICA0_slave_decode(g_rx_data);
            }
            else
            {
                printf("\r\n***corrupt packet****\r\n");
            }
            
            drv_set_IICA0_slave_receive_flag(0);
        }
    }

    if (FLAG_IICA0_SLAVE_I2C_REG_RD)
    {
        #if defined (ENALBE_SPECIFIC_FUNCTION)
        touch_int_pin_trig();            
        #endif
        
        IICA0_slave_encode();
    }
}

void IICA0_slave_check_reg(unsigned char d)
{                    
    switch(d)
    {
        case REG_RD_0x00:
            FLAG_IICA0_SLAVE_I2C_REG_RD= 1;
            break;
        case REG_WR_0x83:
            FLAG_IICA0_SLAVE_I2C_REG_WR = 1;
            break;
    }
}

/*
    [ONE format]
    [0x50] 
    [0x00 0x00 0x00]    // reserved
    [0x3C 0x00]         // z_1 , z_2
    [0x3C]              // total force
    [0x00]              // reserved
    [0x0B]              // CRC

    [TWO format]
    [0x50] 
    [0x00 0x00 0x00]    // reserved
    [0x7F 0xA3]         // z_1 , z_2
    [0x7F]              // total force
    [0x00]              // reserved
    [0x67]              // CRC

*/

void iica0_custom_slavehandler(void)
{
    // unsigned char i = 0;

    /* Control for stop condition */
    if (1U == SPD0)
    {    
        /* Detect stop condition    */
        SPIE0 = 0U;                                     //Enable/disable generation of interrupt request when stop condition is detected 
        // WTIM0 = 1U;                                     //After input of nine clocks, the clock is set to low level and wait is set for master device.
        // WREL0 = 1U;                                     //release I2C bus wait
        
        IICA0_slave_data_process();       
        IICA0_slave_Init();
        g_iica0_slave_status_flag = 1U;

        // printf("STO\r\n");
    }
    else
    {
        //for I2C repeated start (restart)
        if (1U == STD0)                                 //start or restart condition
        {    
            g_iica0_slave_status_flag = 0U;             //Reset slave status flag 
            // printf("REPEAT\r\n");
        }

        if ((g_iica0_slave_status_flag & _80_IICA_ADDRESS_COMPLETE) == 0U)
        {

            //Address match , I2C slave transmit requested
            if (1U == COI0)                             //check selected as slave or not
            { 
                //selected by slave address
                SPIE0 = 1U;                             //Enable/disable generation of interrupt request when stop condition is detected 
                g_iica0_slave_status_flag |= _80_IICA_ADDRESS_COMPLETE;
                
                // printf("ADDR\r\n");
                
                //I2C slave transmit requested
                if (1U == TRC0)                         //check if transmit or not
                {                    
                    //data transmit timing
                    WTIM0 = 1U;                         //After input of nine clocks, the clock is set to low level and wait is set for master device.

                    // printf("TX\r\n");

                    if (g_iica0_tx_cnt > 0U)            //check remaining transmit data
                    {                        
                        // printf("TX-1\r\n");
                        //start to transmit next data in buffer
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                    }
                    else
                    {                   
                        // printf("TX-2\r\n");     
                        drv_set_IICA0_slave_send_flag(1);//r_iica0_callback_slave_sendend();

                        //Send dummy data?
                        IICA0 = 0;

                        //release clock stretching
                        WREL0 = 1U;                     //release I2C bus wait
                    }
                }
                else    //I2C slave receive requested
                {
                    // receive data portion of Master Write Slave Read is started from here

                    FLAG_IICA0_SLAVE_I2C_START_RCV = 1;
                    // printf("RX\r\n");

                    g_iica0_rx_cnt = 0;
                    g_iica0_rx_len = 0xFF;
                   
                    
                    //data receive start
                    ACKE0 = 1U;                         //enable ACK response for receive data
                    WTIM0 = 0U;                         //After input of eight clocks, the clock is set to low level and wait is set for master device.
                    WREL0 = 1U;                         //start to receive
                }
            }
            else
            {
                printf("!COI0\r\n");
                // r_iica0_callback_slave_error(MD_ERROR); 
            }
        }
        else
        {                     
              
            //data transfer timing , I2C slave transmit requested
            if (1U == TRC0)
            {                
                //data transmit timing , ACK was not detected
                if ((0U == ACKD0) && (g_iica0_tx_cnt != 0U))
                {
                    // r_iica0_callback_slave_error(MD_NACK);
                    
                    //release clock stretching
                    WREL0 = 1U;                         //release I2C bus wait

                    // printf("TX-3\r\n");                    
                }
                else
                {
                    
                    #if defined (ENALBE_SPECIFIC_FUNCTION)   // wait until pull low then start transmit data
                    // while (!TOUCH_SYNC);
                    if (TOUCH_SYNC == LOW)  
                    {
                        FLAG_IICA0_SLAVE_I2C_MASTER_RDY_TO_RCV = 1;
                    }
                    #endif

                    if (g_iica0_tx_cnt > 0U)
                    {
                        // printf("TX-4\r\n");   
                        //continue to transmit  
                        #if 1   // master ready to receive
                        if (FLAG_IICA0_SLAVE_I2C_MASTER_RDY_TO_RCV)
                        {                  
                            IICA0 = *gp_iica0_tx_address;
                            gp_iica0_tx_address++;
                            g_iica0_tx_cnt--;
                        }

                        #else    
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                        #endif
                    }
                    else
                    {             
                        // printf("TX-5\r\n");              
                        drv_set_IICA0_slave_send_flag(1);//r_iica0_callback_slave_sendend();

                        // WREL0 = 1U;

                        //exit from communication mode
                        LREL0 = 1U;
                    }
                }
            }
            else    //I2C slave receive requested
            {
                if (g_iica0_rx_cnt < g_iica0_rx_len)
                {                    
                    g_rx_data[g_iica0_rx_cnt] = IICA0;

                    // first byte will consider as REG byte
                    // raw data
                    // printf("%2d,%3d,%02Xh\r\n" ,g_iica0_rx_cnt,g_iica0_rx_len,g_rx_data[g_iica0_rx_cnt]);
                    
                    IICA0_slave_check_reg(g_rx_data[0]);

                    if (g_iica0_rx_cnt == (g_iica0_cal_rx_len-1) &&
                        FLAG_IICA0_SLAVE_I2C_START_RCV )
                    {
                        WTIM0 = 1U;                     //After input of nine clocks, the clock is set to low level and wait is set for master device.
                        WREL0 = 1U;                     //release I2C bus wait
                        drv_set_IICA0_slave_receive_flag(1);//r_iica0_callback_slave_receiveend();

                        FLAG_IICA0_SLAVE_I2C_START_RCV = 0;

                        // for (i = 0 ; i < g_iica0_cal_rx_len ; i++)
                        // {
                        //     // printf("meet len(%d):0x%02X\r\n" ,g_iica0_rx_cnt  ,g_rx_data[g_iica0_rx_cnt]);
                        //     printf("%2d,%02Xh," ,g_iica0_rx_cnt,g_rx_data[g_iica0_rx_cnt]);
                        // }
                        // printf("RDY\r\n");

                    }
                    else
                    {
                        //continue to receive next data
                        WREL0 = 1U;                     //release I2C bus wait

                        // hard code to get data length
                        if ( (g_rx_data[PACKET_WR_IDX_0] == REG_WR_0x83)  &&
                            (g_rx_data[PACKET_WR_IDX_1]  <= TOUCH_NO_2) )   // BYTE2 , Number of Touch
                        {
                            g_iica0_rx_len = 3 + PACKET_DATA_LEN*g_rx_data[PACKET_WR_IDX_1];
                            g_iica0_cal_rx_len = g_iica0_rx_len;
                            // printf("len:%d\r\n" ,g_iica0_rx_len);
                        }
                    }    

                    g_iica0_rx_cnt++;

                }
                else
                {
                    WREL0 = 1U;
                    // printf("RDY2\r\n");
                }
            }
        }
    }
}


