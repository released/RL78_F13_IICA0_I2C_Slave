/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_cg_macrodriver.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

// #define DATA_LENGTH                                             (255U)  /* Tx/Rx data buffer length.        */

#define TX_DATA_LEN                                                 (8)
#define PACKET_DATA_LEN                                             (7)
#define TX_BUFFER_LEN                                               (32)
#define RX_BUFFER_LEN                                               (64)

#define REG_RD_0x00                                                 (0x00)
#define REG_WR_0x83                                                 (0x83)

#define ENABLE_I2C_SLAVE_RCV_LOG0
#define ENABLE_I2C_SLAVE_RCV_LOG1
#define ENABLE_I2C_SLAVE_RCV_LOG2

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

// MD_STATUS drv_IICA0_slave_read(unsigned char * const rx_buf, unsigned short rx_num);
// MD_STATUS drv_IICA0_slave_write(unsigned char * const tx_buf, unsigned short tx_num);

void drv_IICA0_slave_callback_error(unsigned char err);

void drv_set_IICA0_slave_bus_busy(bool flag);
bool drv_get_IICA0_slave_bus_busy(void);

void drv_set_IICA0_slave_receive_flag(bool flag);
bool drv_get_IICA0_slave_receive_flag(void);
void drv_set_IICA0_slave_send_flag(bool flag);
bool drv_get_IICA0_slave_send_flag(void);


// MD_STATUS IICA0_slave_read(unsigned char* rx_xfer_data,unsigned short rx_num);
// MD_STATUS IICA0_slave_write(unsigned char* tx_xfer_data,unsigned short tx_num);

void IICA0_slave_Init(void);
void IICA0_slave_decode(unsigned char* target_decode_data);

void iica0_custom_slavehandler(void);


