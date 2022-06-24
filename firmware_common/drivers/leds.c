/*!**********************************************************************************************************************
@file leds.c                                                                
@brief LED driver and API

This driver provides on, off, toggle, blink and PWM functionality.
The basic on/off/toggle functionality is applied directly to the LEDs.
Blinking and PWMing of LEDs rely on the EIE operating system to provide timing at
regular 1ms calls to LedUpdate().

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 

PUBLIC FUNCTIONS

PROTECTED FUNCTIONS

***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Led"
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* New variables (all shall start with G_xxLed*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */

extern const PinConfigurationType G_asBspLedConfigurations[U8_TOTAL_LEDS]; /*!< @brief from board-specific file */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Led_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Led_StateMachine;                   /*!< @brief The state machine function pointer */
//static u32 Led_u32Timeout;                           /*!< @brief Timeout counter used across states */

 static LedControlType Led_asControl[U8_TOTAL_LEDS];    /*!< @brief Holds individual control parameters for LEDs */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedOn(LedNameType eLED_)

@brief Turn the specified LED on.  

This function automatically takes care of the active low vs. active high LEDs.
The function works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOn(BLUE);


Requires:
- Definitions in G_asBspLedConfigurations[eLED_] and Led_asControl[eLED_] are correct

@param eLED_ is a valid LED index

Promises:
- eLED_ is turned on 
- eLED_ is set to LED_NORMAL_MODE mode

*/
void LedOn(LedNameType eLED_)
{
  
  u32 *pu32OnAddress;

  /* Configure set and clear addresses */
  if(G_asBspLedConfigurations[eLED_].eActiveState == ACTIVE_HIGH)
  {
    /* Active high LEDs use SODR to turn on */
    pu32OnAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_SODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  else
  {
    /* Active low LEDs use CODR to turn on */
    pu32OnAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_CODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  
  /* Turn on the LED */
  *pu32OnAddress = G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;
  
  /* Always set the LED back to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;

} /* end LedOn() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedOff(LedNameType eLED_)

@brief Turn the specified LED off.

This function automatically takes care of the active low vs. active high LEDs.
It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOff(BLUE);


Requires:
- Definitions in G_asBspLedConfigurations[eLED_] and Led_asControl[eLED_] are correct

@param eLED_ is a valid LED index

Promises:
- eLED_ is turned off 
- eLED_ is set to LED_NORMAL_MODE mode

*/
void LedOff(LedNameType eLED_)
{
  u32 *pu32OffAddress;

  /* Configure set and clear addresses */
  if(G_asBspLedConfigurations[(u8)eLED_].eActiveState == ACTIVE_HIGH)
  {
    /* Active high LEDs use CODR to turn off */
    pu32OffAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_CODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  else
  {
    /* Active low LEDs use SODR to turn off */
    pu32OffAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_SODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  
  /* Clear the bit corresponding to eLED_ */
	*pu32OffAddress = G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;

  /* Always set the LED back to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;
  
} /* end LedOff() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedToggle(LedNameType eLED_)

@brief Toggles the specified LED from on to off or vise-versa.

This function automatically takes care of the active low vs. active high LEDs.
It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedToggle(BLUE);


Requires:
- Write access to PIOx_ODSR is enabled

@param eLED_ is a valid LED index

Promises:
- eLED_ is toggled 
- eLED_ is set to LED_NORMAL_MODE

*/
void LedToggle(LedNameType eLED_)
{
  u32* pu32Address = (u32*)(&(AT91C_BASE_PIOA->PIO_ODSR) + G_asBspLedConfigurations[eLED_].ePort);

  *pu32Address ^= G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;
  
  /* Set the LED to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;

} /* end LedToggle() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedBlink(LedNameType eLED_, LedRateType eBlinkRate_)

@brief Sets eLED_ to BLINK mode with the rate given.

BLINK mode requires the main loop to be running at 1ms period. If the main
loop timing is regularly off, the blinking timing may be affected although
unlikely to a noticeable degree.  

Example to blink the PURPLE LED at 1Hz:

LedBlink(PURPLE, LED_1HZ);


Requires:
@param eLED_ is a valid LED index
@param eBlinkRate_ is an allowed blinking rate from LedRateType

Promises:
- eLED_ is set to LED_BLINK_MODE at the blink rate specified

*/
void LedBlink(LedNameType eLED_, LedRateType eBlinkRate_){
  
  /* Always set the LED back to Blink mode */
	Led_asControl[(u8)eLED_].eMode = LED_BLINK_MODE;
  
  /* Set the LED blink rate */
	Led_asControl[(u8)eLED_].eRate = eBlinkRate_;
  
   /* Set the LED duty counter cycle */
	Led_asControl[(u8)eLED_].u16Count = eBlinkRate_;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedInitialize(void)

@brief Initialization of LED system parameters and visual LED check.


Requires:
- 

Promises:
- 

*/
void LedInitialize(void)
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    Led_StateMachine = LedSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Led_StateMachine = LedSM_Error;
  }
    
} /* end LedInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void LedRunActiveState(void)
{
  Led_StateMachine();

} /* end LedRunActiveState */



/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Idle(void)

@brief Loop through led configuration structs to check for blinking updates!
*/
static void LedSM_Idle(void)
{
  
   u32 *pu32Address;
  
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
 
    /* Check if LED is in BLINK_MODE mode */
    if (Led_asControl[i].eMode == LED_BLINK_MODE)
    {
      
    /* Toggle LED and reset cycle counter if cycle counter is 0 */
        if (Led_asControl[i].u16Count == 0)
        {
          
          u32* pu32Address = (u32*)(&(AT91C_BASE_PIOA->PIO_ODSR) + G_asBspLedConfigurations[i].ePort);
          *pu32Address ^= G_asBspLedConfigurations[(u8)i].u32BitPosition;
          Led_asControl[i].u16Count = Led_asControl[i].eRate;
  
        }
     /* Decrement counter */ 
      Led_asControl[i].u16Count--;
            
    }
      
  
  }
}/* end LedSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void LedSM_Error(void)          
{
  
} /* end LedSM_Error() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


