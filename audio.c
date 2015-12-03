#include "inc/hw_memmap.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/pwm.h"
#include "audio.h"
#include "globals.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "inc/lm3s8962.h"

//*****************************************************************************
//
// The current volume of the music/sound effects.
//
//*****************************************************************************
static unsigned char g_ucVolume = 50;

//*****************************************************************************
//
// A pointer to the song currently being played, if any.  The value of this
// variable is used to determine whether or not a song is being played.  Since
// each entry is a short, the maximum length of the song is 65536 / 300
// seconds, which is around 218 seconds.
//
//*****************************************************************************
static const unsigned short *g_pusMusic = 0;

//*****************************************************************************
//
// The number of bytes in the array describing the song currently being played.
//
//*****************************************************************************
static unsigned short g_usMusicLength;

//*****************************************************************************
//
// The count of clock ticks into the song being played.
//
//*****************************************************************************
static unsigned short g_usMusicCount;

//*****************************************************************************
//
// A pointer to the sound effect currently being played, if any.  The value of
// this variable is used to determine whether or nto a song is being played.
// Each entry of this array represents 1/300th of a second.
//
//*****************************************************************************
static const unsigned short *g_pusSoundEffect = 0;

//*****************************************************************************
//
// The number of entries in the array describing the sound effect currently
// being played.
//
//*****************************************************************************
static unsigned short g_usSoundLength;

//*****************************************************************************
//
// The cound of clock ticks into the sound effect being played.
//
//*****************************************************************************
static unsigned short g_usSoundCount;

//*****************************************************************************
//
// Mutes the audio output and sets the PWM generator to an inaudible frequency.
//
//*****************************************************************************
static void
AudioMute(void)
{
    //
    // Disable the PWM output.
    //
    PWMOutputState(PWM_BASE, PWM_OUT_1_BIT, false);
    PWMOutputInvert(PWM_BASE, PWM_OUT_1_BIT, false);

    //
    // Set the PWM frequency to 40 KHz, beyond the range of human hearing.
    //
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, g_ulSystemClock / (SILENCE * 8));
    PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
}

//*****************************************************************************
//
// Sets the volume of the music/sound effect playback.
//
//*****************************************************************************
void
AudioVolume(unsigned long ulPercent)
{
    //
    // If the volume is below two then simply mute the output.
    //
    if(ulPercent < 2)
    {
        PWMOutputState(PWM_BASE, PWM_OUT_1_BIT, false);
        PWMOutputInvert(PWM_BASE, PWM_OUT_1_BIT, false);
    }
    else
    {
        //
        // Set the PWM compare register based on the requested volume.  Since
        // this value is relative to zero, it is correct for any PWM frequency.
        //
        HWREG(PWM_BASE + PWM_GEN_0_OFFSET + PWM_O_X_CMPB) = ulPercent;
        PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);

        //
        // Turn on the output since it might have been muted previously.
        //
        PWMOutputState(PWM_BASE, PWM_OUT_1_BIT, true);
        PWMOutputInvert(PWM_BASE, PWM_OUT_1_BIT, true);
    }

    //
    // Save the volume for future use (such as un-muting).
    //
    g_ucVolume = ulPercent & 0xff;
}

//*****************************************************************************
//
// Adjusts the audio output up by the specified percentage.
//
//*****************************************************************************
void
AudioVolumeUp(unsigned long ulPercent)
{
    //
    // Increase the volume by the specified amount.
    //
    g_ucVolume += ulPercent;

    //
    // Do not let the volume go above 100%.
    //
    if(g_ucVolume > 100)
    {
        //
        // Set the volume to the maximum.
        //
        g_ucVolume = 100;
    }

    //
    // Set the actual volume if something is playing.
    //
    if(g_pusMusic || g_pusSoundEffect)
    {
        AudioVolume(g_ucVolume);
    }
}

//*****************************************************************************
//
// Adjusts the audio output down by the specified percentage.
//
//*****************************************************************************
void
AudioVolumeDown(unsigned long ulPercent)
{
    //
    // Do not let the volume go below 0%.
    //
    if(g_ucVolume < ulPercent)
    {
        //
        // Set the volume to the minimum.
        //
        g_ucVolume = 0;
    }
    else
    {
        //
        // Decrease the volume by the specified amount.
        //
        g_ucVolume -= ulPercent;
    }

    //
    // Set the actual volume if something is playing.
    //
    if(g_pusMusic || g_pusSoundEffect)
    {
        AudioVolume(g_ucVolume);
    }
}

//*****************************************************************************
//
// Returns the current volume level.
//
//*****************************************************************************
unsigned char
AudioVolumeGet(void)
{
    //
    // Return the current Audio Volume.
    //
    return(g_ucVolume);
}

void delay(unsigned long length)
{
	PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
	SysCtlDelay(length);
}

void dot(void)
{
	//
    // Unmute the audio volume.
    //
    AudioVolume(g_ucVolume);
    

    //
    //Turn LED on
    //
    GPIO_PORTF_DATA_R |= 0x01;

    //
    // Set the PWM frequency to the next frequency in the sound effect.
    //
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0,
                    (g_ulSystemClock /
                     (500 * 8)));
    PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
    delay(DOTLENGTH);

    //
    //Turn LED off
    //
	GPIO_PORTF_DATA_R &= ~(0x01);


    //
    // The sound effect is over, so mute the output.
    //
    AudioMute();
    delay(DOTLENGTH);
	
}

void dash(void)
{
	//
    // Unmute the audio volume.
    //
    AudioVolume(g_ucVolume);
    
    //
    //Turn LED ON
    //
    GPIO_PORTF_DATA_R |= 0x01;

    //
    // Set the PWM frequency to the next frequency in the sound effect.
    //
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0,
                    (g_ulSystemClock /
                     (500 * 8)));
    PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
    delay(DOTLENGTH*3);

    //
    //Turn LED OFF
    //
    GPIO_PORTF_DATA_R &= ~(0x01);


    //
    // The sound effect is over, so mute the output.
    //
    AudioMute();
    delay(DOTLENGTH);	
}

void letterBreak() {
    // The break between letters is 3x(dot length).
    // One dot length comes automatically at the end of
    // the previous dot or dash, so 2 more are needed.
    delay(2*DOTLENGTH);
}

void wordBreak() {
    // The break between words is 4x(dot length).
    // Three come for free since a letter just ended,
    // so 4 more are needed.
    delay(4*DOTLENGTH);
}

void send(char letter) {
    // Sends the given letter to the output
    switch (letter) {
        case 'A':
        case 'a': dot(); dash(); break;
        case 'B':
        case 'b': dash(); dot(); dot(); dot(); break;
        case 'C':
        case 'c': dash(); dot(); dash(); dot(); break;
        case 'D':
        case 'd': dash(); dot(); dot(); break;
        case 'E':
        case 'e': dot(); break;
        case 'F':
        case 'f': dot(); dot(); dash(); dot(); break;
        case 'G':
        case 'g': dash(); dash(); dot(); break;
        case 'H':
        case 'h': dot(); dot(); dot(); dot(); break;
        case 'I':
        case 'i': dot(); dot(); break;
        case 'J':
        case 'j': dot(); dash(); dash(); dash(); break;
        case 'K':
        case 'k': dash(); dot(); dash(); break;
        case 'L':
        case 'l': dot(); dash(); dot(); dot(); break;
        case 'M':
        case 'm': dash(); dash(); break;
        case 'N':
        case 'n': dash(); dot(); break;
        case 'O':
        case 'o': dash(); dash(); dash(); break;
        case 'P':
        case 'p': dot(); dash(); dash(); dot(); break;
        case 'Q':
        case 'q': dash(); dash(); dot(); dash(); break;
        case 'R':
        case 'r': dot(); dash(); dot(); break;
        case 'S':
        case 's': dot(); dot(); dot(); break;
        case 'T':
        case 't': dash(); break;
        case 'U':
        case 'u': dot(); dot(); dash(); break;
        case 'V':
        case 'v': dot(); dot(); dot(); dash(); break;
        case 'W':
        case 'w': dot(); dash(); dash(); break;
        case 'X':
        case 'x': dash(); dot(); dot(); dash(); break;
        case 'Y':
        case 'y': dash(); dot(); dash(); dash(); break;
        case 'Z':
        case 'z': dash(); dash(); dot(); dot(); break;
        case '0': dash(); dash(); dash(); dash(); dash(); break;
        case '1': dot(); dash(); dash(); dash(); dash(); break;
        case '2': dot(); dot(); dash(); dash(); dash(); break;
        case '3': dot(); dot(); dot(); dash(); dash(); break;
        case '4': dot(); dot(); dot(); dot(); dash(); break;
        case '5': dot(); dot(); dot(); dot(); dot(); break;
        case '6': dash(); dot(); dot(); dot(); dot(); break;
        case '7': dash(); dash(); dot(); dot(); dot(); break;
        case '8': dash(); dash(); dash(); dot(); dot(); break;
        case '9': dash(); dash(); dash(); dash(); dot(); break;
        case ',': dash(); dash(); dot(); dot(); dash(); dash(); break;
        case '.': dot(); dash(); dot(); dash(); dot(); dash(); break;
        case '?': dot(); dot(); dash(); dash(); dot(); dot(); break;
        case ';': dash(); dot(); dash(); dot(); dash(); dot(); break;
        case '\'': dash(); dash(); dash(); dot(); dot(); dot(); break;
        case '-': dash(); dot(); dot(); dot(); dot(); dash(); break;
        case '/': dash(); dot(); dot(); dash(); dot(); break;
        case '(':
        case ')': dash(); dot(); dash(); dash(); dot(); dash(); break;
        case '_': dot(); dot(); dash(); dash(); dot(); dash(); break;
        case ' ': wordBreak(); break;
    }
    letterBreak();
}

//*****************************************************************************
//
// Provides periodic updates to the PWM output in order to produce a sound
// effect or play a song.
//
//*****************************************************************************
void
AudioHandler(void)
{
    unsigned long ulLoop;

    //
    // See if a song is being played.
    //
    if(g_pusMusic)
    {
        //
        // Increment the music counter.
        //
        g_usMusicCount++;

        //
        // Find the correct position within the array describing the song.
        //
        for(ulLoop = 0; ulLoop < g_usMusicLength; ulLoop += 2)
        {
            if(g_pusMusic[ulLoop] > g_usMusicCount)
            {
                break;
            }
        }

        //
        // See if the end of the song has been reached.
        //
        if(ulLoop == g_usMusicLength)
        {
            //
            // The song is over, so mute the output.
            //
            AudioMute();

            //
            // Indicate that there is no longer a song being played.
            //
            g_pusMusic = 0;
        }
        else
        {
            //
            // Set the PWM frequency to the requested frequency.
            //
            PWMGenPeriodSet(PWM_BASE, PWM_GEN_0,
                            (g_ulSystemClock / (g_pusMusic[ulLoop - 1] * 8)));
            PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
        }
    }

    //
    // Otherwise, see if a sound effect is being played.
    //
    else if(g_pusSoundEffect)
    {
        //
        // See if the end of the sound effect has been reached.
        //
        if(g_usSoundCount == g_usSoundLength)
        {
            //
            // The sound effect is over, so mute the output.
            //
            AudioMute();

            //
            // Indicate that there is no longer a sound effect being played.
            //
            g_pusSoundEffect = 0;
        }
        else
        {
            //
            // Set the PWM frequency to the next frequency in the sound effect.
            //
            PWMGenPeriodSet(PWM_BASE, PWM_GEN_0,
                            (g_ulSystemClock /
                             (g_pusSoundEffect[g_usSoundCount] * 8)));
            PWMSyncUpdate(PWM_BASE, PWM_GEN_0_BIT);
        }

        //
        // Increment the sound effect counter.
        //
        g_usSoundCount++;
    }
}

//*****************************************************************************
//
// Turns off audio playback.
//
//*****************************************************************************
void
AudioOff(void)
{
    //
    // Mute the output.
    //
    AudioMute();

    //
    // Cancel any song or sound effect playback that may be in progress.
    //
    g_pusMusic = 0;
    g_pusSoundEffect = 0;
}

//*****************************************************************************
//
// Configures the PWM module for producing audio.
//
//*****************************************************************************
void
AudioOn(void)
{
    //
    // Turn off the PWM generator 0 outputs.
    //
    PWMOutputState(PWM_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, false);
    PWMGenDisable(PWM_BASE, PWM_GEN_0);

    //
    // Configure the PWM generator.  Up/down counting mode is used simply to
    // gain an additional bit of range and resolution.
    //
    PWMGenConfigure(PWM_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_SYNC);

    //
    // Mute the audio output.
    //
    AudioMute();

    //
    // Enable the generator.
    //
    PWMGenEnable(PWM_BASE, PWM_GEN_0);
}

//*****************************************************************************
//
// Starts playback of a song.
//
//*****************************************************************************
void
AudioPlaySong(const unsigned short *pusSong, unsigned long ulLength)
{
    //
    // Stop the playback of any previous song or sound effect.
    //
    g_pusMusic = 0;
    g_pusSoundEffect = 0;

    //
    // Save the length of the song and start the song counter at zero.
    //
    g_usMusicLength = ulLength;
    g_usMusicCount = 0;

    //
    // Save the pointer to the song data.  At this point, the interrupt handler
    // could be called and commence the actual playback.
    //
    g_pusMusic = pusSong;

    //
    // Unmute the audio volume.
    //
    AudioVolume(g_ucVolume);
}

//*****************************************************************************
//
// Starts playback of a sound effect.
//
//*****************************************************************************
void
AudioPlaySound(const unsigned short *pusSound, unsigned long ulLength)
{
    //
    // Stop the playback of any previous song or sound effect.
    //
    g_pusMusic = 0;
    g_pusSoundEffect = 0;

    //
    // Save the length of the sound effect and start the sound effect counter
    // at zero.
    //
    g_usSoundLength = ulLength;
    g_usSoundCount = 0;

    //
    // Save the pointer to the sound effect data.  At this point, the interrupt
    // handler could be called and commence the actual playback.
    //
    g_pusSoundEffect = pusSound;

    //
    // Unmute the audio volume.
    //
    AudioVolume(g_ucVolume);
}
