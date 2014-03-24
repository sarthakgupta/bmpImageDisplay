#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "ff.h"
#include "diskio.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/ELT240320TP.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

/*Variable required for SD Card R/W*/
FRESULT rc;						/* Result code */
FATFS fatfs;					/* File system object */
FIL fil;						/* File object */
DIR dir;						/* Directory object */
FILINFO fno;					/* File information object */
UINT i;
BYTE buff[10];					/* Buffer to store data read from SD Card*/
WORD bw,br;
unsigned char d1,d2,d3,d4,d5=44,d=49;/* Buffer to store data read from SD Card*/
//char str[11]="Hello World",dummy[3000];
unsigned long a=524;
int j,flag=0;
int a1=1;
//signed int width=0, height=0, addr=0, c_method=0;
//unsigned int pixel_val=0;
signed int width=0, height=0, _height=0, addr=0, c_method=0, bits_pp,size;
unsigned long pixel_val=0,dummy[50],tft_pix,remainder;
unsigned char pix1,pix2,pix3,_pix1,_pix2,_pix3;

/*Function to handle Systick interrupts*/
void SysTickHandler(void)
{
	SysTickIntDisable();
	a1++;
	if(a1==100)
	{
		a1=1;
		disk_timerproc();
	}

	SysTickIntEnable();
}

volatile unsigned int g_ulcount = 0;
unsigned long k1,k2,k3;
volatile unsigned int flag1=0,flag2=1;
void InitConsole(void)
{
    /* Enable GPIO port A which is used for UART0 pins. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Make the UART pins be peripheral controlled. */
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART for console I/O. */
    UARTStdioInit(0);
    UARTprintf("HELLO\n");
}
unsigned int pix[320];
unsigned int smile[] =
{
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xF7FF,
	0x9EDE,0x869D,0x9EDE,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xDFBF,0x6EFF,0x7F7F,0x5F9F,0x679F,0x575F,0x773F,0x6EBE,
	0xDF9F,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0x76DE,0x97BF,0x679F,0x679F,0x679F,0x5F9F,
	0x679F,0x4F3F,0x4F3F,0x875F,0xAF1E,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x773F,0x679F,0x679F,0x679F,
	0x679F,0x679F,0x679F,0x5F9F,0x5F9F,0x4F3F,0x3EBF,0x3EBF,
	0x5E3D,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x6EFF,0x679F,
	0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x679F,0x679F,0x679F,0x5F9F,
	0x4F3F,0x4F3F,0x3EBF,0x3EBF,0xA6DE,0xFFFF,0xFFFF,0xFFFF,
	0xDFBF,0x979F,0x679F,0x5F9F,0x032F,0x032F,0x5F9F,0x679F,
	0x679F,0x679F,0x5F9F,0x032F,0x032F,0x3EBF,0x46FF,0x6EBE,
	0xD75E,0xFFFF,0xFFFF,0x6EFF,0x679F,0x679F,0x032F,0x5F9F,
	0x5F9F,0x032F,0x679F,0x679F,0x679F,0x032F,0x5F9F,0x4F3F,
	0x032F,0x3EBF,0x3EBF,0x459B,0xFFFF,0xFFFF,0x875F,0x679F,
	0x679F,0x032F,0x5F9F,0x5F9F,0x032F,0x679F,0x679F,0x5F9F,
	0x032F,0x5F9F,0x4F3F,0x032F,0x3EBF,0x3EBF,0x5E3D,0xFFFF,
	0x9EFE,0x577F,0x679F,0x0476,0x5F9F,0x5F9F,0x5F9F,0x5F9F,
	0x679F,0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x0476,
	0x365F,0x361E,0x8DD9,0x869D,0x577F,0x0476,0x679F,0x5F9F,
	0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x5F9F,0x577F,0x5F9F,0x4F3F,
	0x4F3F,0x3EBF,0x3EBF,0x0476,0x361E,0x759A,0x9EDE,0x575F,
	0x577F,0x5F9F,0x0476,0x0476,0x2D7B,0x2D7B,0x361E,0x361E,
	0x361E,0x2D7B,0x2D7B,0x0476,0x0476,0x365F,0x365F,0x2DDD,
	0x8DD9,0xD77F,0x3EBF,0x577F,0x0476,0xFFFF,0xFFFF,0xEF7D,
	0xEF7D,0xE71C,0xE71C,0xE71C,0xEF7D,0xEF7D,0xFFFF,0xEF7D,
	0x0476,0x361E,0x2D7B,0xD71D,0xFFFF,0x76BF,0x0476,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xEF7D,0xEF7D,0xEF7D,0xFFFF,
	0xFFFF,0xFFFF,0xEF7D,0xEF7D,0x0476,0x4539,0xFFFF,0xFFFF,
	0xD77E,0x0476,0xFFFF,0xFFFF,0xFFFF,0xC618,0xFFFF,0xFFFF,
	0xC618,0xFFFF,0xFFFF,0xC618,0xFFFF,0xEF7D,0xEF7D,0x0476,
	0xA63A,0xFFFF,0xFFFF,0xFFFF,0x0476,0xFFFF,0xFFFF,0xC618,
	0xC618,0xC618,0xC618,0xC618,0xC618,0xC618,0xC618,0xC618,
	0xEF7D,0xEF7D,0x4CD8,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0x0476,0xFFFF,0xFFFF,0xC618,0xFFFF,0xFFFF,0xC618,0xFFFF,
	0xFFFF,0xC618,0xEF7D,0xEF7D,0x4539,0xF7DF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0476,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xEF7D,0xEF7D,0x4CD8,0xF7DF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0x0476,0x0476,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x4539,
	0xA63A,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0476,0x0476,0x0476,
	0x0476,0x0476,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF};

int main(void)
{
	/*Set the clocking to run at 80Mhz from the crystal of 16MHz using PLL*/
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	/* Set the clock for the GPIO Port A,B, E and F */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	/* Set the type of the GPIO Pin */
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

	/* Set the clock for the SSI Module0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    //Initialising UART
    InitConsole();

	/*Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 milliseconds interrupt.*/
	SysTickPeriodSet(SysCtlClockGet() / 10000);
	SysTickIntRegister(SysTickHandler);
	SysTickEnable();
	SysTickIntEnable();

	/* Set the type of the GPIO Pin of PORTF */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 );
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

	/* GPIO Pin2 on PORT F initialized to 0 */
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 , 0 );

	/* Configure GPIO pad with internal pull-up enabled */
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

	/* Enable global interupts */
	IntMasterEnable();

	UARTprintf("MOUNT initialize!!!\n");

	rc=f_mount(0, &fatfs);		// Register volume work area (never fails)

	if(rc!=FR_OK)
	{
		UARTprintf("1.rc=%d\nERROR MOUNT!!!",rc);
	}
    /* Reset and Initialise the LCD */
	TSLCDRst();
    TSLCDInit();
    setBitBL;

    /* TFT Home Display */
    TSLCDFillRect(0,TS_SIZE_X-1,0,TS_SIZE_Y-1,TS_COL_AQUA,TS_MODE_NORMAL);
    TSLCDFillRect(0,TS_SIZE_X-1,120,200,TS_COL_YELLOW,TS_MODE_NORMAL);

    TSLCDSetFontColor(TS_COL_RED);
   	TSLCDPrintStr(4,0,"Tiva Launchpad Based .BMP Image Display ",TS_MODE_NORMAL);
   	TSLCDPrintStr(5,0,"             TI-CEPD, NSIT              ",TS_MODE_NORMAL);
   	TSLCDShowPic(157,175,218,236,smile,TS_MODE_FULL);

while(1)
{

	while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0);
	SysCtlDelay(SysCtlClockGet()/100);
	flag1++;
	flag2=0;
	if(flag1==8)
		flag1=1;

	if(flag2==0)
	{
	//Opening test1.bmp
		if(flag1==1)
		{
			rc=f_open(&fil,"test1.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test1::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
	//Opening test2.bmp
		else if((flag1==2)&&(flag2==0))
		{
			rc=f_open(&fil,"test2.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test2::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
	//Opening test3.bmp
		else if((flag1==3)&&(flag2==0))
		{
			rc=f_open(&fil,"test3.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test3::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
	//Opening test4.bmp
		else if((flag1==4)&&(flag2==0))
		{
			rc=f_open(&fil,"test4.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test4::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
	//Opening test5.bmp
		else if((flag1==5)&&(flag2==0))
		{
			rc=f_open(&fil,"test5.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test4::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
	//Opening test6.bmp
		else if((flag1==6)&&(flag2==0))
		{
			rc=f_open(&fil,"test6.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test4::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}
//Opening test7.bmp
		else if((flag1==7)&&(flag2==0))
		{
			rc=f_open(&fil,"test7.bmp",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
			SysCtlDelay(SysCtlClockGet()/100);
//			UARTprintf("Test4::::\n");
			if(rc!=FR_OK)
			{
				UARTprintf("2.rc=%d\n ERROR OPEN!!!\n",rc);
			}
		}

		//Setting background colour
		TSLCDFillRect(0,TS_SIZE_X-1,0,TS_SIZE_Y-1,TS_COL_AQUA,TS_MODE_NORMAL);

		//Printing Image data
		rc=f_read(&fil,&dummy,10, &br);

		//Reading bmp offset address 10 or 0xA
		rc=f_read(&fil,&addr,4, &br);
//		UARTprintf("address::%04d\n",addr);

		rc=f_read(&fil,&dummy,4, &br);

		//Reading Width at address 18 or 0x12
		rc=f_read(&fil,&width,4, &br);
//		UARTprintf("width::%04d\n",width);

		//Reading height at address 22 or 0x16
		rc=f_read(&fil,&height,4, &br);
//		UARTprintf("height::%04d\n",height);

		rc=f_read(&fil,&dummy,4, &br);

		//Reading compression method of image at address 30 or 0x1E
		rc=f_read(&fil,&c_method,4, &br);
//		UARTprintf("Compression method::%04d\n",c_method);

		rc=f_read(&fil,&dummy,addr-34, &br);

		//Finding the number of bytes for padding
		remainder=(width*3)%4;
		if(remainder!=0)
		{
			remainder=4-remainder;
		}
//		UARTprintf("dummy bytes::%04d\n",remainder);

		//Setting the offset to display the image in centre
		_height=120+height/2;

		//Reading Bitmap data  at address::54
		for(j=1;j<=height;j++)
		{
			for(i=1;i<=width;i++)
				{
				rc=f_read(&fil,&pix1,1, &br);
				rc=f_read(&fil,&pix2,1, &br);
				rc=f_read(&fil,&pix3,1, &br);
				_pix1=pix1/8;
				_pix2=pix2/4;
				_pix3=pix3/8;
/*				UARTprintf("blue::%04d\n",pix1);
				UARTprintf("blue::%04d\n",_pix1<<11);
				UARTprintf("green::%06d\n",pix2);
				UARTprintf("green::%06d\n",_pix2<<5);
				UARTprintf("red::%04d\n",pix3);
				UARTprintf("red::%04d\n",_pix3);
*/
				tft_pix=(_pix1<<11)+(_pix2<<5)+_pix3;
//				UARTprintf("tft pix::%04d\n",tft_pix);
				pix[i-1]=tft_pix;
				}
			TSLCDShowPic(160-(width/2),159+(width/2),_height-j,_height-j,pix,TS_MODE_FULL);
			rc=f_read(&fil,&dummy,remainder, &br);
		}
		rc = f_close(&fil);
		flag2=1;
	}
}
	return 0;
}
