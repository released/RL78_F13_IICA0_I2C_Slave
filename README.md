# RL78_F13_IICA0_I2C_Slave
 RL78_F13_IICA0_I2C_Slave

udpate @ 2024/09/03

1. use RL78 F13 MCU , to test below function 

- UART : P15/TXD0, P16/RXD0

- IICA0 MASTER : P62/SCL0 ,  P63/SDA0

	- use RL78_G16_IICA0_I2C_Master , for I2C master

- use P32 for GPIO input to sync with master if necessary, check define : ENALBE_SPECIFIC_FUNCTION

- use P61 for GPIO output to inform master if necessary, check define : ENALBE_SPECIFIC_FUNCTION

2. under I2C irq (check IICA0_slave_decode in iica0_custom_slavehandler) , will output data , which come from I2C master

check define : #define ENABLE_I2C_SLAVE_RCV_LOG0 , ENABLE_I2C_SLAVE_RCV_LOG1 , ENABLE_I2C_SLAVE_RCV_LOG2

	- format cmd 0 : 50h,83h,00h,crc
	
	- format cmd 1~5 : 50h,83h,01h,data x 7,crc
	
	- format cmd 6~10: 50h,83h,02h,data x 14,crc


3. when master send write reg : 0x83 , check flag : FLAG_IICA0_SLAVE_I2C_REG_WR

when master send read reg : 0x00 , will return data to I2C master , check flag : FLAG_IICA0_SLAVE_I2C_REG_RD

	- format : 00h,00h,5Ah,A5h,counter,00h,crc

4. below is log message when receive I2C data

![image](https://github.com/released/RL78_F13_IICA0_I2C_Slave/blob/main/log.jpg)

