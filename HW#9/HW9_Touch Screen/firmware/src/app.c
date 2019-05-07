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
#include "ili9341_PIC32.h"
#include <stdio.h>

#define buttonx 120
#define buttony1 80
#define buttony2 200
#define L 80.9
#define H 50
#define BL 30
#define BH 10

char letter[100];
int count;
char x_value;
char y_value;
char z_value;


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
    LCD_button_plus(buttonx,buttony1,L,H,BL,BH,ILI9341_GREEN, ILI9341_BLACK); // plus button
    LCD_button_minus(buttonx,buttony2,L,H,BL,BH, ILI9341_RED,ILI9341_BLACK); //minus button
    count=0;
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
  
                 CS_T=0;
              
//unsigned short x, y; int z1,z2; //XPT2046_read(&x, &y, &z1, &z2);
int z_raw;
unsigned short x;
unsigned short y;
unsigned short t;
int z1;
int z2;

XPT2046_read(&x, &y, &z1, &z2,&t);
z_raw=z1-z2+4095;

unsigned short x_actual;
unsigned short y_actual;
x_actual=((x-245)/15.125);
y_actual=((3900-y)/11.15);

       CS_T=1;
        sprintf(letter,"X Raw : %d      ",x); //print x value
       LCD_get(30,40,letter,ILI9341_RED,ILI9341_WHITE);  
       sprintf(letter,"Y Raw : %d      ",y); //print y value
       LCD_get(30,55,letter,ILI9341_RED,ILI9341_WHITE);
      sprintf(letter,"X : %d      ",x_actual); //print x value
       LCD_get(30,70,letter,ILI9341_RED,ILI9341_WHITE);   
       sprintf(letter,"Y : %d     ",y_actual); //print y value
       LCD_get(30,85,letter,ILI9341_RED,ILI9341_WHITE);
       sprintf(letter,"Z : %d   ",z_raw); // print z value
       LCD_get(30,100,letter,ILI9341_RED,ILI9341_WHITE);

       //&&buttony1<y_actual && y_actual<(buttony1+H)
       if (z_raw>800)
         { 
       if(buttonx<=x_actual && x_actual<=(buttonx+L) && buttony1<y_actual && y_actual<(buttony1+H)){
        count=count+1;
        }
       }
       if (z_raw>800){
        if(buttonx<=x_actual && x_actual<=(buttonx+L) && buttony2<y_actual && y_actual<(buttony2+H)){
        count=count-1;
        }
         }
       sprintf(letter,"C O U N T :%d",count);
       LCD_get(135, 45,letter,ILI9341_RED,ILI9341_WHITE); 
       
     //led flash
      //20hz update
         
     
        }
            break;
        }

        /* TODO: implement your application state machine.*/
        

}


 

/*******************************************************************************
 End of File
 */
