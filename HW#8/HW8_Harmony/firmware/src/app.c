/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "ili9341_PIC32_Harmony.h"
#include <stdio.h>

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
short ax_raw;
float ax_real;
short ay_raw;
float ay_real;
short az_raw;
float az_real;
float xl_r;
float xl_l;
float yl_u;
float yl_d;
short temp_raw;
float temp_real;

unsigned char data[13];
unsigned char *index=data;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
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
void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
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
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }
    
 
        case APP_STATE_SERVICE_TASKS:
        {
       
            
                 _CP0_SET_COUNT(0);  
       //WHO AM I
       i2c_master_start(); 
       i2c_master_send(Write);//WHO ArM I
       i2c_master_send(WHO);
       i2c_master_stop(); 
       
       i2c_master_restart();//restart
       i2c_master_send(Read);
       r = i2c_master_recv(); 
       i2c_master_ack(1); // make the ack so the slave knows we got it
       i2c_master_stop(); // make the stop bit
     
       //Who am I display
       sprintf(letter," WHO AM I %d ",r);
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
       I2C_read_multiple(0,0x20,index,14);      
       ax_raw=data[9]<<8|data[8];
       ax_real=4*9.8/65536*ax_raw;
       ay_raw=data[11]<<8|data[10];
       ay_real=-4*9.8/65536*ay_raw;
       az_raw=data[13]<<8|data[12];
       az_real=4*9.8/65536*az_raw;
       
       temp_raw=data[1]<<8|data[0];
       temp_real=temp_raw/65536*125.00-40+25;
       
       //acceleration value display
       sprintf(letter," A_X  % .02f ",ax_real);
       LCD_get(28, 40,letter,ILI9341_RED,ILI9341_WHITE);
       sprintf(letter," A_Y  % .02f ",ay_real);
       LCD_get(28, 50,letter,ILI9341_RED,ILI9341_WHITE);
       sprintf(letter," A_Z  % .02f ",az_real);
       LCD_get(28, 60,letter,ILI9341_RED,ILI9341_WHITE);
      // sprintf(letter," temp  % .02f ",temp_real);
      //LCD_get(28, 70,letter,ILI9341_RED,ILI9341_WHITE);
       //bar display
       if (ax_real>0){
           xl_r=framelength/2*ax_real/9.8;
           xl_l=0;
       }else{
           xl_l=-framelength/2*ax_real/9.8;
           xl_r=0;    
       }
       if (ay_real>0){
           yl_u=framelength/2*ay_real/9.8;
           yl_d=0;
       } else{
           yl_d=-framelength/2*ay_real/9.8;
           yl_u=0;
           }
     LCD_bar_right(centerx, centery, framewidth,framewidth,framewidth, ILI9341_CYAN, ILI9341_WHITE); //center    
     LCD_bar_right(centerx+framewidth, centery, framelength,framewidth,xl_r ,ILI9341_CYAN, ILI9341_WHITE); //right 
     LCD_bar_left(centerx, centery,framelength,framewidth,xl_l,ILI9341_CYAN, ILI9341_WHITE); //left
     LCD_bar_up(centerx, centery, framelength,framewidth,yl_u, ILI9341_CYAN, ILI9341_WHITE); //up
     LCD_bar_down(centerx, centery+framewidth, framelength,framewidth,yl_d, ILI9341_PINK, ILI9341_WHITE); //down
     
     //led flash
     while (_CP0_GET_COUNT()<=1200000){;} //20hz update
     LATAbits.LATA4 =!LATAbits.LATA4;    
     
        }
            break;
        }

        /* TODO: implement your application state machine.*/
        


}


 

/*******************************************************************************
 End of File
 */
