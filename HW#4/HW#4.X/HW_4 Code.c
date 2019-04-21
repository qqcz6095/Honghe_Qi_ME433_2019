#include"xc.h"           
#include"sys/attribs.h"  
#include "math.h"
#define CS LATAbits.LATA0 
#define CONFIA 0b01110000
#define CONFIB 0b11110000
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
#pragma config WDTPS = PS1 // use slowest wdt
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
#pragma config USERID = 0x5877 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module
void init_SPI();
void setvoltage(char,int);
char SPI1_I0(char write);

char SPI1_I0(char write){
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF){;}
    return SPI1BUF;
}
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

    init_SPI();
    __builtin_enable_interrupts();
    
    float VA,VB,i = 0.0;
    int tri_i = 0, slope = 1;
    float V_step = 10.23;
    
    while (1){
        VB = slope*V_step*tri_i;                           // 5Hz triangle 
        tri_i++;
        if (tri_i <= 100) {                         // uphill slope 
            slope = 1;
        }
        else if (tri_i <= 200) {                    // downhill slope
            slope = -1;
        }
        else {                                          // Reset 
            tri_i = 0;
        }
        setvoltage(CONFIB, VB);                       // Send voltage to B
        
        while (_CP0_GET_COUNT() < 24000) {;}            // 1ms  ((48M/2)*.001s)
          _CP0_SET_COUNT(0);
        VA = 512.0 + 512.0*sin(i*2*3.1415*10);      // 10Hz 
        i = i + .001;                           
        setvoltage(CONFIA, VA);                        //Send voltage to A
    }
    
    return 0;
}
void init_SPI(){
   // SDI1Rbits.SDI1R = 0b0100;
    TRISAbits.TRISA1 = 0;               // A1 output
    RPA1Rbits.RPA1R = 0b0011;           // Set A1 to SDO1 
    TRISAbits.TRISA0 = 0;               // A0 output 
    SS1Rbits.SS1R = 0b0011;             // Set A0 to SS1 
    CS = 1;
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 0x1;
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;   
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
    CS = 0;                                 // enable the ram
    CS = 1;                                 // finish the command
}

void setvoltage(char a,int v){
    char val = v >>6;
    char bit1 = a | val;
    char bit2 = v << 2;
    CS = 0;
    SPI1_I0(bit1);
    SPI1_I0(bit2);
    CS = 1;
}