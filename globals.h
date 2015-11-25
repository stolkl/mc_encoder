
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

//*****************************************************************************
//
// The clock rate for the SysTick interrupt.  All events in the application
// occur at some fraction of this clock rate.
//
//*****************************************************************************
#define CLOCK_RATE              300

//*****************************************************************************
//
// The speed of the processor.
//
//*****************************************************************************
extern unsigned long g_ulSystemClock;

#endif // __GLOBALS_H__
