
//*********************************************************
//  PIC18F14K50 UART ?? Program//        :  MPLABX XC8
//  ??????4Mhz   Baud Rate:9600BPS, 16-bit Baud Rate
//  ???????????????????
//      ??????
//        1 ? LED0?ON
//        2 ? LED1?ON
//        3 ? LED2?ON
//        4 ? LED3?ON
//     ???? ? ?LED OFF
//   ??????
//      UART Input   : RB5
//*********************************************************

#include    <xc.h>
#include    <stdio.h>


#define LED0      LATCbits.LATC0
#define LED1      LATCbits.LATC1
#define LED2      LATCbits.LATC2
#define LED3      LATCbits.LATC3

//-------------- ?????????? -----------------------
#pragma config FOSC = IRC        //  ??????
#pragma config USBDIV = OFF, CPUDIV = NOCLKDIV
#pragma config IESO  = OFF, FCMEN = OFF, PLLEN  = ON
#pragma config BORV  = 30,  BOREN = OFF, PWRTEN = OFF
#pragma config WDTPS = 32768, WDTEN = OFF
#pragma config MCLRE = OFF, HFOFST = OFF, XINST  = OFF
#pragma config BBSIZ = OFF, LVP    = OFF, STVREN = ON
#pragma config CP1  = OFF, CP0  = OFF, CPD  = OFF, CPB  = OFF
#pragma config WRT1 = OFF, WRT0 = OFF, WRTB = OFF, WRTC = OFF
#pragma config EBTR1 = OFF, EBTR0 = OFF, EBTRB  = OFF

#define _XTAL_FREQ 4000000


//*********************************************************
// ??
//*********************************************************
void uint2string(char *buf,uint16_t data, uint16_t flg);
char GetChar(void);
void PutString03(char *string,uint16_t data, uint16_t flg, uint16_t keta);
void PutString01(char *string,uint16_t flg);
void PutStringLF(void);
void PutString(char *string);
void Wait(unsigned int num);


//*********************************************************
// ??
//*********************************************************

char CharConv[16]={
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};


//*********************************************************
//
//*********************************************************
void main(void){
    
    char RxData;
    char i ;

    // ???????????
    // Internal Clock (8 MHz)
    // 
    // <OSCCON  0110 1010>
    // bit7 IDLEN=0 : 0 = SLEEP ?????????????????????
    // bit6-4 IRCF<2:0>: ???????????????
    //      111 = 16 MHz
    //     *110 = 8 MHz
    //      101 = 4 MHz
    //      100 = 2 MHz
    //      011 = 1 MHz(3)
    //      010 = 500 kHz
    //      001 = 250 kHz
    //      000 = 31 kHz(2)
    // bit3 OSTS=1: 1 = ?????CONFIG1 ?????FOSC<2:0> ????????????????
    // bit2 HFIOFS=0: 0 = HFINTOSC ????????????
    // bit1-0 SCS<1:0>: ?????????????
    //      1x = ??????? ????
        
    OSCCON = 0b01101010;

    // I/O : Digital Mode
    ANSEL  = 0b00000000 ;    // ANS3-7 ??????????
    ANSELH = 0b00000000 ;    // ANS8-10??????????

    // Configure Input / Output
    // TRISA ???(bit4,5)???????(= 1)?????PORTA ???????????
    // TRISB ???(bit4,5,6,7)???????(= 1)?????PORTA ???????????
    TRISA = 0b00100000; // RA5: Input
    TRISB = 0b00000010; // RX: Input

    // Configure TX / RX Pins
    // Setup EUSART
    // RCSTA: SPEN=1????????EUSART ???????TX/CK I/O ???
    // ??????????????TX/CK??????????????????
    // ??????????ANSEL?????????????I/O????????
    // 
    // RCSTA: SPEN=1 --EUSART ??????
    // TXSTA: TXEN(bit5) = 1, SYNC(bit4) = 0, BRGH(bit2)=1
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;
    
    // BAUDCON
    // BRG16(bit3)=0 
    BAUDCON = 0b00000000;
    
    // SYNC = 0?BRGH = 1?BRG16 = 0 FOSC = 8.000 MHz
    // 9600bps 
    
    SPBRG = 51;

         Wait(100) ;               // 1???????

    PutString01("*******************",1);
    PutString01("*** UART START ****",1);
    PutString01("*******************",1);

         
    
        
        
     while(1) {
     
        RxData = GetChar();
        
        if(RxData > 0){
            PutString03("Rxdata = ", RxData, 1, 16);
        }
        
        Wait(100) ;          // ???????????
     }
}

//-----------------------------------------------------------------------------
// ??????(num x 10ms)?????????????
//-----------------------------------------------------------------------------
void Wait(unsigned int num)
{
     int i ;

     // num?????????????
     for (i=0 ; i < num ; i++) {
          __delay_ms(10) ;     // 10ms??????????
     }
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PutString(char *string)
{
    char dummy;
    
    while( *string != (char)NULL ){
        while(TXIF==0){
            
        } 
        
        // ???????????    *1)
        TXREG = *string ;          // ????   
        dummy = TXREG;
        string ++;
    }
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PutStringLF(void)
{
    char dummy;
        while(TXIF==0){
            
        }     // ???????????    *1)
        TXREG = 0x0d ;          // ????   
        dummy = TXREG;
        
        while(TXIF==0) {
            
        }     // ???????????    *1)
        
        TXREG = 0x0a ;          // ????  
        dummy = TXREG;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PutString01(char *string,uint16_t flg)
{
    PutString(string);
    if(flg == 1)
        PutStringLF();
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void PutString03(char *string,uint16_t data, uint16_t flg, uint16_t keta)
{
    char buf[20];
    
    PutString(string);
    uint2string(buf, data, keta);
    PutString(buf);
    if(flg == 1)
        PutStringLF();
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
char GetChar(void)
{
    char rxdt;
    
    if (PIR1bits.RCIF){      // ????????
       rxdt = RCREG;
    }
    else{
        rxdt = 0;
    }
    
    return rxdt;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void uint2string(char *buf,uint16_t data, uint16_t flg)
{
    uint16_t dt;
    uint16_t i;
    uint16_t sw;
    
    sw = 0;
    
    dt = data;
    if( flg == 10){
        for(i=10000; i>0; i/=10){
            dt = dt/i;
            if(sw==0){
                if(dt != 0){
                    *buf = CharConv[dt];
                    sw=1;
                }
            }
            else{
                    *buf = CharConv[dt];            
            }
            dt = data - dt*i;
            buf ++;
        }
    }
    else{
        *buf = '0';
        buf ++;
        *buf = 'x';
        buf ++;

        dt = (data >> 12)& 0x0f;
        *buf = CharConv[dt];
        buf ++;
        
        dt = (data >> 8)& 0x0f;
        *buf = CharConv[dt];
        buf ++;
        
        dt = (data >> 4)& 0x0f;
        *buf = CharConv[dt];
        buf ++;
        
        dt = data & 0x0f;
        *buf = CharConv[dt];
        buf ++;
                
    }
    
    *buf =(char) NULL;
    
}




