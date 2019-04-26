#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ili9341_PIC32.h"
#include <stdio.h>
//#include <ili9341.c>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF// no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define  Read   0b11010111 
#define  Write  0b11010110
#define  WHO 0x0F
#define CTRL1_XL 0x10
#define SetA 0b10000010
#define CTRL2_G 0x11
#define SetG 0b10001000
#define CTRL3_C 0x12
#define Set3C 00000100

#define O_T_L 0x20
#define O_T_H 0x21
#define OX_L_G 0x22
#define OX_H_G 0x23
#define OY_L_G 0x24
#define OY_H_G 0x25
#define OZ_L_G 0x26
#define OZ_H_G 0x27
#define OX_L_XL 0x28
#define OX_H_XL 0x29
#define OY_L_XL 0x2A
#define OY_H_XL 0x2B
#define OZ_L_XL 0x2C
#define OZ_H_XL 0x2D

#define centerx 110
#define centery 200
#define framelength 100
#define framewidth 6

char letter[100];
int k;
double f;

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; //A4 Output
    TRISBbits.TRISB4 = 1; //B4 Input 
    ANSELBbits.ANSB2 = 0; //B2 Output
    ANSELBbits.ANSB3 = 0; //B3 Output
    __builtin_enable_interrupts();
   
    i2c_master_setup(); //initial i2c
    SPI1_init(); // initial SPI
    LCD_init(); //Initial LCD
    LCD_clearScreen(ILI9341_BLACK); //set background
    while(1) {
        
	// use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
	// remember the core timer runs at half the sysclk
       
      /* if (_CP0_GET_COUNT()>1200000)
            {
                LATAbits.LATA4 =!LATAbits.LATA4;
                _CP0_SET_COUNT(0);
        }
            while (PORTBbits.RB4==0)
        {
            LATAbits.LATA4 =0;
        }
       
      //WHO AM I
      // LCD_clearScreen(ILI9341_GREEN);
       i2c_master_start(); //read GP7
        i2c_master_send(Write);//WHO ArM I
        i2c_master_send(WHO);
        i2c_master_stop(); 
       
        i2c_master_restart();//restart
        i2c_master_send(Read);
        r = i2c_master_recv(); 
        i2c_master_ack(1); // make the ack so the slave knows we got it
        i2c_master_stop(); // make the stop bit
     
       sprintf(letter," WHO AM I %d ",r);
       LCD_get(28, 32,letter,ILI9341_RED,ILI9341_WHITE);*/
       
        sprintf(letter," I L O V E   C A R ");
        LCD_get(28, 32,letter,ILI9341_RED,ILI9341_WHITE);
        
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
       
       //Accelerometer Read
       char AccelRead;
       i2c_master_start();
       i2c_master_send(Write);
       i2c_master_send(CTRL1_XL);
       i2c_master_stop();
       i2c_master_restart();
       i2c_master_send(Read);
       AccelRead=i2c_master_recv();
       i2c_master_ack(1);
       i2c_master_stop();
       
       sprintf(letter," Acceleration  % d ",AccelRead);
       LCD_get(28, 32,letter,ILI9341_RED,ILI9341_WHITE);
          
     //LCD_bar_right(centerx, centery, framewidth,framewidth,framewidth, ILI9341_CYAN, ILI9341_WHITE); //center    
     LCD_bar_right(centerx+framewidth, centery, framelength,framewidth, 10,ILI9341_CYAN, ILI9341_WHITE); //right 
     //LCD_bar_left(centerx, centery,framelength,framewidth,10,ILI9341_CYAN, ILI9341_WHITE); //left
     //LCD_bar_up(centerx, centery, framelength,framewidth,10, ILI9341_CYAN, ILI9341_WHITE); //up
     //LCD_bar_down(centerx, centery+framewidth, framelength,framewidth,10, ILI9341_CYAN, ILI9341_WHITE); //down
       }
       /* for ( k = 0; k < 100;  k++){
            _CP0_SET_COUNT(0);
        sprintf(letter," Hello World !  % d ",k);
        LCD_get(28, 32,letter,ILI9341_RED,ILI9341_WHITE);
        
        LCD_bar(28, 80,k,4,ILI9341_DARKGREEN, ILI9341_WHITE);
      
       f= 24000000.0/_CP0_GET_COUNT();
       sprintf(letter," FPS %.2f",f);
       LCD_get(28, 120,letter,ILI9341_DARKCYAN,ILI9341_WHITE);
       while (_CP0_GET_COUNT()<=2400000){;}
       
        }*/
    } 
        
        
       


