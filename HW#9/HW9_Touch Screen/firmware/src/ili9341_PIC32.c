#include "ili9341_PIC32.h"
#include <xc.h>
#include "app.h"

unsigned short y_read;
unsigned short y_convert;


short x_read;
int x_convert;
short z2_read;  
int z2_convert;
short z1_read;
int z1_convert;
short z1;
short z2;
#define CS LATBbits.LATB7
#define DC LATBbits.LATB15

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

void LCD_init() {
    int time = 0;
    
    CS = 0; // CS
   
    LCD_command(ILI9341_SWRESET);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 7200000) {} // 300ms

    LCD_command(0xEF);
  	LCD_data(0x03);
	LCD_data(0x80);
	LCD_data(0x02);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xCF);
  	LCD_data(0x00);
	LCD_data(0xC1);
	LCD_data(0x30);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xED);
  	LCD_data(0x64);
	LCD_data(0x03);
	LCD_data(0x12);
    LCD_data(0x81);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xE8);
  	LCD_data(0x85);
	LCD_data(0x00);
	LCD_data(0x78);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xCB);
  	LCD_data(0x39);
	LCD_data(0x2C);
	LCD_data(0x00);
    LCD_data(0x34);
    LCD_data(0x02);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xF7);
  	LCD_data(0x20);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xEA);
  	LCD_data(0x00);
	LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_PWCTR1);
  	LCD_data(0x23);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_PWCTR2);
  	LCD_data(0x10);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_VMCTR1 );
  	LCD_data(0x3e);
    LCD_data(0x28);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_VMCTR2);
  	LCD_data(0x86);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_MADCTL);
  	LCD_data(0x48);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
/*    
    LCD_command(ILI9341_VSCRSADD);
  	LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
 */   
    LCD_command(ILI9341_PIXFMT);
  	LCD_data(0x55);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_FRMCTR1);
  	LCD_data(0x00);
    LCD_data(0x18);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command( ILI9341_DFUNCTR);
  	LCD_data(0x08);
    LCD_data(0x82);
    LCD_data(0x27);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xF2);
  	LCD_data(0); // 1
    LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GAMMASET);
  	LCD_data(0x01);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GMCTRP1);
  	LCD_data(0x0F);
    LCD_data(0x31);
    LCD_data(0x2B);
    LCD_data(0x0C);
    LCD_data(0x0E);
    LCD_data(0x08);
    LCD_data(0x4E);
    LCD_data(0xF1);
    LCD_data(0x37);
    LCD_data(0x07);
    LCD_data(0x10);
    LCD_data(0x03);
    LCD_data(0x0E);
    LCD_data(0x09);
    LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GMCTRN1);
  	LCD_data(0x00);
    LCD_data(0x0E);
    LCD_data(0x14);
    LCD_data(0x03);
    LCD_data(0x11);
    LCD_data(0x07);
    LCD_data(0x31);
    LCD_data(0xC1);
    LCD_data(0x48);
    LCD_data(0x08);
    LCD_data(0x0F);
    LCD_data(0x0C);
    LCD_data(0x31);
    LCD_data(0x36);
    LCD_data(0x0F);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xB1);
  	LCD_data(0x00);
    LCD_data(0x10);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_SLPOUT);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_DISPON);
    
    CS = 1; // CS
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    
    CS = 0; // CS
    
    LCD_command(ILI9341_MADCTL);
    LCD_data(MADCTL_MX | MADCTL_BGR); // rotation
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    CS = 1; // CS
}

void SPI1_init() {
  SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
  RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
  TRISBbits.TRISB7 = 0; // CS is B7
  CS = 1; // CS starts high

  // DC pin
  TRISBbits.TRISB15 = 0;
  DC = 1;
  
  SPI1CON = 0; // turn off the spi module and reset it
  SPI1BUF; // clear the rx buffer by reading from it
  SPI1BRG = 2.5; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0; // clear the overflow bit
  SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1; // master operation
  SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    DC = 0; // DC
        spi_io(com);
    DC = 1; // DC
}

void LCD_data(unsigned char dat) {
    spi_io(dat);
}

void LCD_data16(unsigned short dat) {
    spi_io(dat>>8);
    spi_io(dat);
}

void LCD_setAddr(unsigned short x, unsigned short y, unsigned short w, unsigned short h) {
    LCD_command(ILI9341_CASET); // Column
    LCD_data16(x);
	LCD_data16(x+w-1);

	LCD_command(ILI9341_PASET); // Page
	LCD_data16(y);
	LCD_data16(y+h-1);

	LCD_command(ILI9341_RAMWR); // Into RAM
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary
    
    CS = 0; // CS
    
    LCD_setAddr(x,y,1,1);
    LCD_data16(color);
    
    CS = 1; // CS
}

void LCD_clearScreen(unsigned short color) {
    int i;
    
    CS = 0; // CS
    
    LCD_setAddr(0,0,ILI9341_TFTWIDTH,ILI9341_TFTHEIGHT);
	for (i = 0;i < ILI9341_TFTWIDTH*ILI9341_TFTHEIGHT; i++){
		LCD_data16(color);
	}
    
    CS = 1; // CS
}

void LCD_letter(unsigned short x, unsigned short y, char letter, unsigned lettercolor, unsigned bgcolor){
    char rowloc = letter - 0x20;
    int no;
    int column;
    for (column=0; column <= 4;){
        char pix = ASCII [rowloc][column];
        for (no = 0;no <= 7;){
            
            if (x+column <= ILI9341_TFTWIDTH && y+no<= ILI9341_TFTHEIGHT){
                if (pix >> no & 1 == 1){
                    LCD_drawPixel (x+column,y+no,lettercolor);
                }
                    else{
                        LCD_drawPixel (x+column,y+no,bgcolor);
                }  
            }
            no++;
        }
        column++;
    }
}

void LCD_get(unsigned short x, unsigned short y, char *letter, unsigned lettercolor, unsigned bgcolor){ 
    int i = 0;
    while(letter[i] != '\0'){
        LCD_letter(x + i * 5,y,letter[i],lettercolor,bgcolor);
        i++;
    }
}

void LCD_button_plus(unsigned short x, unsigned short y, unsigned short L, unsigned short H,unsigned short BL,unsigned short BH, unsigned short buttonbg,unsigned short buttoncolor){
    int i,j,l,m,n,o;
        for(l = 0; l < L; l++){
        for(m = 0; m < H;m++){
            LCD_drawPixel(x + l, y + m, buttonbg);
            
        } //button background
    }
    for (n=0;n<BH;n++){
        for(o=0;o<BH;o++){
    LCD_drawPixel(x+(L-BH)/2+n,y+(H-BH)/2+o ,buttoncolor);
        }
    }// center pixel
    for(i = 0; i < ((BL-BH)/2);i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L+BH)/2+i,y +(H-BH)/2+j,buttoncolor);
             }
        } //button sign right
     for(i = 0; i < (BL-BH)/2;i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L-BH)/2-i,y +(H-BH)/2+j,buttoncolor);
             }
      }// button sign left
     for(i = 0; i < (BL-BH)/2;i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L+BH)/2-j,y +(H-BH)/2-i,buttoncolor);
             }
      }// button sign up
     for(i = 0; i < (BL-BH)/2;i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L-BH)/2+j,y +(H+BH)/2+i,buttoncolor);
             }
      }// button sign down*/
    }
void LCD_button_minus(unsigned short x, unsigned short y, unsigned short L, unsigned short H,unsigned short BL,unsigned short BH, unsigned short buttonbg,unsigned short buttoncolor){
    int i,j,l,m,n,o;
        for(l = 0; l < L; l++){
        for(m = 0; m < H;m++){
            LCD_drawPixel(x + l, y + m, buttonbg);
            
        } //button background
    }
    for (n=0;n<BH;n++){
        for(o=0;o<BH;o++){
    LCD_drawPixel(x+(L-BH)/2+n,y+(H-BH)/2+o ,buttoncolor);
        }
    }// center pixel
    for(i = 0; i < ((BL-BH)/2);i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L+BH)/2+i,y +(H-BH)/2+j,buttoncolor);
             }
        } //button sign right
     for(i = 0; i < (BL-BH)/2;i++){
             for(j = 0; j< BH;j++){
                LCD_drawPixel(x +(L-BH)/2-i,y +(H-BH)/2+j,buttoncolor);
             }
      }// button sign left
    }

void XPT2046_read(unsigned short *x, unsigned short *y, unsigned int *z1,unsigned int *z2, unsigned short *t){
 //this is trash command.   
unsigned short t_temp1;unsigned short t_temp2;   
spi_io(0b11010001);
t_temp1=spi_io(0);
t_temp2=spi_io(0);
*t=(t_temp1<<8|t_temp2)>>3;
// end of trash command. I do not know why I have to have this trash command, otherwise the reading sticks to 510.... Nick does not know why either. :(
unsigned short y_temp1;unsigned short y_temp2;
spi_io(0b10010001);
y_temp1=spi_io(0);
y_temp2=spi_io(0);
*y=(y_temp1<<8|y_temp2)>>3;

unsigned short x_temp1; unsigned short x_temp2;
spi_io(0b11010001);
x_temp1=spi_io(0);
x_temp2=spi_io(0);
*x=(x_temp1<<8|x_temp2)>>3;

 unsigned short z2_temp1; unsigned short  z2_temp2;
spi_io(0b11000001);
z2_temp1=spi_io(0);
z2_temp2=spi_io(0);
*z2=(z2_temp1<<8|z2_temp2)>>3;

unsigned short z1_temp1; unsigned short z1_temp2;
spi_io(0b10110001);
z1_temp1=spi_io(0);
z1_temp2=spi_io(0);
*z1=(z1_temp1<<8|z1_temp2)>>3;
}