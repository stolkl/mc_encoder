#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "drivers/rit128x96x4.h"
#include "audio.h"
#include "globals.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


//*****************************************************************************
//
// Flags that contain the current value of the interrupt indicator as displayed
// on the OLED display.
//
//*****************************************************************************
unsigned long g_ulFlags;

int clock_tenthsseconds=0;
int clock_seconds=0;
int clock_minutes=0;
int clock_hours=0;

int alarm_minutes=0;
int alarm_hours=0;

int alarm_set=0;

int ring_alarm=0;
int is_PM=0;
int is_alarm_PM=0;

static unsigned long ulLastStatus = 0;
static unsigned long ulLastSelStatus = 0;

char buffer[80];

//*****************************************************************************
//
// The counter of the number of consecutive times that the buttons have
// remained constant.
//
//*****************************************************************************
static unsigned long g_ulDebounceCounter;

//*****************************************************************************
//
// The speed of the processor clock, which is therefore the speed of the clock
// that is fed to the peripherals.
//
//*****************************************************************************
unsigned long g_ulSystemClock;
//*****************************************************************************
//
// This variable holds the last stable button status.
//
//*****************************************************************************
volatile unsigned long g_ulButtonStatus;

//*****************************************************************************
//
// This variable holds the Watch status.
//
//*****************************************************************************
volatile int g_watchStatus=0;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// Handles the SysTick timeout interrupt.
//
//*****************************************************************************
//void
//SysTickIntHandler(void)
//{
//    //
//    // Update the music/sound effects.
//    //
//    //AudioHandler();
//}

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void
Timer1IntHandler(void)
{
	
	//
	// Clear the timer interrupt.
	//
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	//
	// Toggle the flag for the second timer.
	//
	HWREGBITW(&g_ulFlags, 1) ^= 1;
}



///*int
//main(void)
//{
//	
//	 Set the clocking to run directly from the crystal.
//	
//	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
//	SYSCTL_XTAL_8MHZ);
//	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
//	
//	
//	
//     Get the system clock speed.
//    
//    g_ulSystemClock = SysCtlClockGet();
//
//	
//	 Initialize the OLED display and write status.
//	
//	RIT128x96x4Init(1000000);
//	RIT128x96x4StringDraw("", 24, 42, 15);
//
//	
//	 Enable the peripherals used by the application.
//	
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM); 
//	
//	
//	 Configure the GPIOs used to read the state of the on-board push buttons.
//	
//	/*GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,
//	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
//	GPIOPadConfigSet(GPIO_PORTE_BASE,
//	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
//	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 
//	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1); 
//	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
//	GPIO_PIN_TYPE_STD_WPU);*/
//	
//    
//     Configure the speaker
//    	
//	GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
//
//	
//	 Enable processor interrupts.
//	
//	IntMasterEnable();
//	
//	
//     Initialize the PWM for generating music and sound effects.
//    
//    AudioOn();
//
//	
//	 Configure the two 32-bit periodic timers.
//	
//	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
//	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 10);
//
//	
//	 Setup the interrupts for the timer timeouts.
//	
//	IntEnable(INT_TIMER1A);
//	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//
//	
//	 Enable the timers.
//	
//	TimerEnable(TIMER1_BASE, TIMER_A);
//	
//	
//     Configure SysTick to periodically interrupt.
//    
//    SysTickPeriodSet(g_ulSystemClock / CLOCK_RATE);
//    SysTickIntEnable();
//    SysTickEnable();
//    
//    while (1)
//    {
//    AudioPlaySong(g_pusFireEffect, sizeof(g_pusFireEffect));
//	SysCtlDelay(SysCtlClockGet());
//	AudioPlaySong(g_pusFireEffect, sizeof(g_pusFireEffect));
//	send('a');
//	send('d');
//	send('e');
//	send(' ');
//	send('i');
//	send('s');
//	send(' ');
//	send('g');
//	send('o');
//	send('o');
//	send('d');
//	send(' ');
//	send('b');
//	send('o');
//	send('y');
//	
//	delay(g_ulSystemClock);
//    }
//	
//
//	
//}*/
