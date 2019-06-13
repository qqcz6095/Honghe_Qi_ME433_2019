#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
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
#pragma config PMDL1WAY = ON // allow multiple reconfigurations
#pragma config IOL1WAY = ON // allow multiple reconfigurations
#pragma config FUSBIDIO = OFF // USB pins controlled by USB module
#pragma config FVBUSONIO = OFF // USB BUSON controlled by USB module


//#include "ili9341.h"
//#include "i2c_master_noint.h"
//#include "ov7670.h"

/*#define DIR1 LATAbits.LATA10
#define DIR2 LATAbits.LATA7
#define USER PORTBbits.RB4



void startup() {
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // pin definitions
    ANSELA = 0;
    ANSELB = 0;
    TRISAbits.TRISA7 = 0; // DIR2
    DIR2 = 1;
    TRISAbits.TRISA10 = 0; // DIR1
    DIR1 = 1;
    TRISBbits.TRISB4 = 1; // USER
    
    // OC1 is B15, goes with DIR1
    
    // OC4 is A4, goes with DIR2
    
    // LCD uses SPI1: A0 is SDO, A1 is SDI, B5 is CST, B14 is SCK1, A9 is DC, B7 is CS
  //  SPI1_init();
   // LCD_init();
   // LCD_clearScreen(ILI9341_DARKGREEN);
    
    // Camera uses C0-7, C8 as OC2, A8 as INT3/PCLK, B13 as INT2/HREF, C9 as INT1/VSYNC, and I2C1
    //i2c_master_setup();
    //ov7670_setup();
    
    
    // B3 is available as SCL2, B2 is available as SDA2
    
    
    
    
}
*/
int main() {

    __builtin_disable_interrupts();

    //startup();
    TRISAbits.TRISA10 = 0;
    TRISBbits.TRISB15 = 0;
LATAbits.LATA10=0b0101;
RPB15Rbits.RPB15R=1;
    __builtin_enable_interrupts();
    
    int I = 0;
    char message[100];
    
       
    while(1) {
        
         
//Timer 2
T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
PR2 = 2399; // PR = PBCLK / N / desiredF - 1
TMR2 = 0; // initial TMR2 count is 0
OC4CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
OC4RS = 2399; // duty cycle
OC4R = 0; // initialize before turning OC1 on; afterward it is read-only
 T2CONbits.ON = 1;
 OC4CONbits.ON = 1; 
      //Timer 3
    T3CONbits.TCKPS = 0b100;
    PR3 = 14999; 
    TMR3 = 0;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    IPC3bits.T3IP = 2;
    IPC3bits.T3IS = 0;
     T3CONbits.ON = 1;
 
    //while (1){};

    }
}
void __ISR(_TIMER_3_VECTOR,IPL5SOFT)Timer3ISR(void){
    
      IFS0bits.T3IF = 0;
    LATAINV = 0b10000; //invert LED A4
    static int count = 0;
    static int direction = 1;
    count = count + direction;
    if (count > 100000) {//0.1 s
        direction = -1;
    }
    else if (count < 0){
        direction = 1;
    }
    OC4RS = (count * 2399)/100;
}