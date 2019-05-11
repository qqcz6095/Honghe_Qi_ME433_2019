#include <xc.h>
#include "IMU.h"

#define  Read   0b11010111 
#define  Write  0b11010110
#define  WHO 0x0F
#define CTRL1_XL 0x10
#define SetA 0b10000010
#define CTRL2_G 0x11
#define SetG 0b10001000
#define CTRL3_C 0x12
#define Set3C 00000100
//void setEx(unsigned char cmd,unsigned char reg);

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
void i2c_master_setup(void) {
  I2C2BRG = 37;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2  50Mhz 
                                    // look up PGD for your PIC32 PGD=104ns
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

void setExpander(unsigned char cmd, unsigned char reg)
{
i2c_master_send(cmd);
i2c_master_send(reg);
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}
void I2C_read_multiple(unsigned char initial_address, unsigned char initial_reg, unsigned char * index, int length){
int i;
for (i=initial_address;i<length;i++)
{
       i2c_master_start();
       i2c_master_send(Write);
       i2c_master_send(initial_reg+i);
       i2c_master_stop();
       i2c_master_restart();
       i2c_master_send(Read);
       *index=i2c_master_recv();
       i2c_master_ack(1);
       i2c_master_stop();
       index++;  
    }
}
void imu_init(){
    // Accelerometer setup
       i2c_master_start();
       i2c_master_send(Write);
       i2c_master_send(CTRL1_XL);
       i2c_master_send(SetA);
       i2c_master_stop;
       
       //Gryo setup
       i2c_master_start();
       i2c_master_send(Write);
       i2c_master_send(CTRL2_G);
       i2c_master_send(SetG);
       i2c_master_stop;
}