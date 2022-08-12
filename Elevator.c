//Elevator Program:
// P0.16 - P0.19 are connected to decoder inputs, it makes one of the o/p LEDs 0 to 9 on
// P0.20-P0.23 are connected to *CLR pins of latches: make it '0' and then '1' to clear
// elevator keys: *Q outputs of latches connected to P1.16 TO P1.19

#include <lpc214x.h>
#include <stdio.h>
#define LED_OFF (IO0SET = 1U << 31)
#define LED_ON (IO0CLR = 1U << 31)
#define PLOCK 0x00000400
void alphadisp7SEG(char* buf);
void delay_ms(unsigned int j);
void elevator_run(void);
#define RS_ON (IO0SET = 1U << 20)
#define RS_OFF (IO0CLR = 1U << 20)
#define EN_ON (IO1SET = 1U << 25)
#define EN_OFF (IO1CLR = 1U << 25)
#define COL0 (IO1PIN & 1 <<19)
#define COL1 (IO1PIN & 1 <<18)
#define COL2 (IO1PIN & 1 <<17)
#define COL3 (IO1PIN & 1 <<16)
unsigned char rowsel=0,colsel=0;
void LCD_SendCmdSignals();
static void LCD_SendHigherNibble(unsigned char dataByte);
void LCD_SendDataSignals();


void keyboard();
static void delay_us(unsigned int count);
unsigned char lookup_table[4][4]={{'0', '1', '2','3'},
{'4', '5', '6','7'},
{'8', '9', 'a','b'},
{'c', 'd', 'e','f'}};
static void LCD_CmdWrite( unsigned char cmdByte)
{
LCD_SendHigherNibble(cmdByte);
LCD_SendCmdSignals();
cmdByte = cmdByte << 4;
LCD_SendHigherNibble(cmdByte);
LCD_SendCmdSignals();

}
static void LCD_DataWrite( unsigned char dataByte)
{
LCD_SendHigherNibble(dataByte);
LCD_SendDataSignals();
dataByte = dataByte << 4;
LCD_SendHigherNibble(dataByte);
LCD_SendDataSignals();

}
static void LCD_Reset(void)
{
/* LCD reset sequence for 4-bit mode*/
LCD_SendHigherNibble(0x30);


LCD_SendCmdSignals();
delay_ms(100);
LCD_SendHigherNibble(0x30);
LCD_SendCmdSignals();
delay_us(200);
LCD_SendHigherNibble(0x30);
LCD_SendCmdSignals();
delay_us(200);
LCD_SendHigherNibble(0x20);
LCD_SendCmdSignals();
delay_us(200);
}
static void LCD_SendHigherNibble(unsigned char dataByte)
{
//send the D7,6,5,D4(uppernibble) to P0.16 to P0.19
IO0CLR = 0X000F0000;IO0SET = ((dataByte >>4) & 0x0f) << 16;
}
static void LCD_SendCmdSignals(void)
{
RS_OFF;// RS - 1
EN_ON;delay_us(100);EN_OFF;// EN - 1 then 0
}
static void LCD_SendDataSignals(void)
{
RS_ON;// RS - 1
EN_ON;delay_us(100);EN_OFF;// EN - 1 then 0
}
static void LCD_Init(void)


{
delay_ms(100);
LCD_Reset();
LCD_CmdWrite(0x28u);//Initialize the LCD for 4-bit 5x7 matrix type
LCD_CmdWrite(0x0Eu);// Display ON cursor ON
LCD_CmdWrite(0x01u);//Clear the LCD
LCD_CmdWrite(0x80u);//go to First line First Position
}
void LCD_DisplayString(const char *ptr_string)
{
// Loop through the string and display char by char
while((*ptr_string)!=0)
LCD_DataWrite(*ptr_string++);
}

static void delay_us(unsigned int count)
{
unsigned int j=0,i=0;
for(j=0;j<count;j++)
{
for(i=0;i<10;i++);
}
}
int main()
{
IO0DIR |= 1U << 31 | 0x00FF0000 ; // to set P0.16 to P0.23 as o/ps
IO1DIR |= 1U << 25; // to set P1.25 as o/p used for EN
IO0DIR |= 1U << 31 | 0XFF << 16 | 1U<<30 ; // to set P0.31 & P1.20 to P1.23 as outputs


IO1DIR |= 1 << 24; // to set P1.24 as output
//keyboard();
LCD_Reset();
LCD_Init();
delay_ms(100);
LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Elevator Operational");
LED_ON; // make D7 Led on .. just indicate the program is running
elevator_run();
while(1);
}
void elevator_run(void)
{
int i,val;
unsigned int counter;
IO1CLR = 1 << 24;// enable elevator section in the application board : 0 to enable
IO0CLR = 0X000F0000;//to set the elevator led for first floor
do{
IO0CLR = 0X00F00000;IO0SET = 0X00F00000; // clear all the latches *CLR
//waiting for floor key
do{
counter = (IO1PIN >> 16) & 0X0000000F ;// wait for any lift/elevator key press
}while(counter == 0x0F);
if(counter == 0x0e){ val=3;
alphadisp7SEG("flor1");
}
//1110 - floor 1 key pressed
else if(counter == 0x0d) { val=6;
alphadisp7SEG("flor2");


}//1101 - floor 2 key pressed
else if(counter == 0x0b) { val=8;
alphadisp7SEG("flor3");
}//1011 - floor 3 key pressed
else if(counter == 0x07) { val=10;
alphadisp7SEG("flor4");
}//0111- floor 4 k
//elevator movement-UP
for(i=0;i<val;i++)
{
IO0CLR = 0X000F0000;IO0SET = i << 16;
delay_ms(1000);
}
//elevator movement-DN
for(i=val-1;i>=0;i--)
{
IO0CLR = 0X000F0000;IO0SET = i << 16;
delay_ms(1000);
}
if(counter==0x07)
keyboard();
}while(1);
}
void SystemInit(void)
{
PLL0CON = 0x01;
PLL0CFG = 0x24;
PLL0FEED = 0xAA;


PLL0FEED = 0x55;
while( !( PLL0STAT & PLOCK ))
{
;
}
PLL0CON = 0x03;
PLL0FEED = 0xAA; // lock the PLL registers after setting the required PLL
PLL0FEED = 0x55;
}
void keyboard(){
SystemInit();
char buf[10] ;
LCD_Reset();
LCD_CmdWrite(0x80); LCD_DisplayString(" Emergency! ");
LCD_CmdWrite(0x94); LCD_DisplayString("Dial Number:");

//initialize UART0 port
IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 ; // to set as o/ps
IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
int i=0;
//make D7 Led on off for testing
LED_ON; delay_ms(500);LED_OFF;delay_ms(500);
do
{
while(1){

//check for keypress in row0,make row0 '0',row1=row2=row3='1'


rowsel=0;IO0SET = 0X000F0000;IO0CLR = 1 << 16;
if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
//check for keypress in row1,make row1 '0'
rowsel=1;IO0SET = 0X000F0000;IO0CLR = 1 << 17;
if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
//check for keypress in row2,make row2 '0'
rowsel=2;IO0SET = 0X000F0000;IO0CLR = 1 << 18;//make row2 '0'
if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
//check for keypress in row3,make row3 '0'
rowsel=3;IO0SET = 0X000F0000;IO0CLR = 1 << 19;//make row3 '0'
if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
};
delay_ms(50);//allow for key debouncing
while(COL0==0 || COL1==0 || COL2==0 || COL3==0);//wait for key release
delay_ms(50);//allow for key debouncing
IO0SET = 0X000F0000; //disable all the rows
//send to serial port(check on the terminal)
IO0SET = 0X0FF0000;//IO0CLR = lookup_table[rowsel][colsel] << 16;
buf[i] = lookup_table[rowsel][colsel];
buf[i+1]=0;
//buf[i]=lookup_table[rowsel][colsel];
i++;
LCD_Reset();
LCD_CmdWrite(0x80); LCD_DisplayString(" ");

LCD_CmdWrite(0x94); LCD_DisplayString(" ");
LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Dial : ");
LCD_CmdWrite(0x94); LCD_DisplayString(buf);
}
while(i<10);
LCD_Reset();
LCD_CmdWrite(0x80); LCD_DisplayString(" ");
LCD_CmdWrite(0x94); LCD_DisplayString(" ");
LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Ringing");
delay_ms(2000);

LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Ringing.");
delay_ms(2000);

LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Ringing..");
delay_ms(200);
LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Ringing...");
delay_ms(500);
LCD_Reset();LCD_CmdWrite(0x80); LCD_DisplayString("Elevator Operational");
delay_ms(3000);
}

void delay_ms(unsigned int j)
{
unsigned int x,i;
for(i=0;i<j;i++)
{
for(x=0; x<600; x++); /* loop to generate 1 milisecond delay with Cclk = 60MHz */

}
}

unsigned char getAlphaCode(unsigned char alphachar)
{
switch (alphachar)
{
// dp g f e d c b a - common anode: 0 segment on, 1 segment off
case 'f': return 0x8e;
case 'i': return 0xf9;
case 'r': return 0xce;
case 'e': return 0x86; // 1000 0110
case 'h': return 0x89;
case 'l': return 0xc7;
case 'p': return 0x8c;
case ' ': return 0xff;
case 'a' : return 0x88;
case 'g' : return 0x82;
case 'o' : return 0xc0;
case '1' : return 0xcf;
case '2' : return 0xa4;
case '3' : return 0xb0;
case '4' : return 0x99;//10011001
//simmilarly add for other digit/characters
default : return 0x99;
break;
}
return 0xff;

}

void alphadisp7SEG(char *buf)
{
unsigned char i,j;
//char seg7_data;

unsigned char seg7_data,temp=0;
for(i=0;i<5;i++) // because only 5 seven segment digits are present
{
seg7_data = getAlphaCode(*(buf+i)); //instead of this look up table can be used
//to shift the segment data(8bits)to the hardware (shift registers) using Data,Clock,Strobe
for(j=0;j<8;j++)
{
//get one bit of data for serial sending
temp = seg7_data & 0x80; // shift data from Most significan bit (D7)
if(temp == 0x80)
IOSET0 |= 1 << 19; //IOSET0 | 0x00080000;
else
IOCLR0 |= 1 << 19; //IOCLR0 | 0x00080000;
if(i==7&&j==7){
IOSET0 |= 1 << 19;
}
//send one clock pulse
IOSET0 |= 1 << 20; //IOSET0 | 0x00100000;
delay_ms(1); // nop();
IOCLR0 |= 1 << 20; //IOCLR0 | 0x00100000;
seg7_data = seg7_data << 1; // get next bit into D7 position

}

}
IOSET0 |= 1 << 30; //IOSET0 | 0x40000000;
delay_ms(1); //nop();
IOCLR0 |= 1 << 30; //IOCLR0 | 0x40000000;
}