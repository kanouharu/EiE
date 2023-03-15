/*!**********************************************************************************************************************
@file timer.c                                                                
@brief Provide easy access to set up and run a Timer Counter (TC) Peripheral.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- TimerChannelType: TIMER0_CHANNEL0, TIMER0_CHANNEL1, TIMER0_CHANNEL2

PUBLIC FUNCTIONS
- void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)
- void TimerStart(TimerChannelType eTimerChannel_)
- void TimerStop(TimerChannelType eTimerChannel_)
- u16 TimerGetTime(TimerChannelType eTimerChannel_)
- void TimerAssignCallback(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_)
 
PROTECTED FUNCTIONS
- void TimerInitialize(void)
- void TimerRunActiveState(void)

**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Timer"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32TimerFlags;                      /*!< @brief Global Timer state flags */
volatile u32 G_u32TimerFlags;                      /*!< @brief Global Timer state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;            /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;             /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;              /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;         /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Timer_<type>" and be declared as static.
***********************************************************************************************************************/
//Discrepency in coding style between text and salvaged code
static fnCode_type Timer_StateMachine;          /*!< @brief The state machine function pointer */

static u32 Timer_u32Timer1IntCounter = 0;         /*!< @brief Track instances of The TC1 interrupt handler */
//Putting it in scope of interrupts.c is tricky. Don't use.
static fnCode_type Timer_fpTimer1Callback;            /*!< @brief The Timer ISR callback pointer */

//??
volatile u32 Timer_u32Timer1Counter = 0;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)

@brief  Sets the timer tick period (interrupt rate).

Requires:
-TimerStop SHOULD BE CALLED BEFORE, and TimerStart should be called after this
function to reset the counter and avoid gliches.

@param eTimerChannel_ holds a valid channel
@param u16TimerValue_ x in ticks

Promises:
- Updates register TC_RC value with u16TimerValue_

*/
void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)
{
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
  /* Load the new timer value */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_RC =
                                          (u32)(u16TimerValue_) & 0x0000FFFF;

} /* end TimerSet() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerStart(TimerChannelType eTimerChannel_)

@brief  Start the timer

Requires:
-

@param eTimerChannel_ holds a valid channel

Promises:
- Updates register TC_RC value with u16TimerValue_

*/
void TimerStart(TimerChannelType eTimerChannel_)
{
  /* Light-Duty Hardfault Diagnosis Local Variables*/
  u32Dummy CCR_Value = (TC1_CCR_INIT | AT91C_TC_CLKEN | AT91C_TC_SWTRG);
  CCR_Value ^= AT91C_TC_CLKDIS;
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
  /* Ensure clock is enabled and triggered */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR ^= AT91C_TC_CLKDIS;
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR |= (AT91C_TC_CLKEN | 
                                                         AT91C_TC_SWTRG);
   
   
   }/* end TimerStart() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerStop(TimerChannelType eTimerChannel_)

@brief  Stop the timer

Requires:
-
@param eTimerChannel_ holds a valid channel


Promises:
-

*/
void TimerStop(TimerChannelType eTimerChannel_)
{
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
    /* Disable counter if enabled */

  /* Ensure clock is enabled and triggered */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR |= AT91C_TC_CLKDIS;
  

} /* end TimerStop() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn u16 TimerGetTime(TimerChannelType eTimerChannel_)

@brief Returns timer counter value for a channel.

Requires:
-

@param eTimerChannel_ holds a valid channel


Promises:

*/
u16 TimerGetTime(TimerChannelType eTimerChannel_)
{
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
  /* Get the current timer value */
  return ((u16) ((AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CV & 0x0000FFFF));
} /* end TimerGetTime() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn  void TimerAssignCallBack(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_ )

@brief  Sets callback function pointer passed to Timer Counter ISR.

Requires:
- Requesite ISR to be written for channel
- Seperate, channel specific function pointer to be defined with global scope

@param eTimerChannel_ holds a valid channel
@param eTimerChannel_ holds a valid function pointer


Promises:

*/
void TimerAssignCallback(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_ )
{
  
   switch(eTimerChannel_)
  {
    case TIMER0_CHANNEL0:
    {
      break;
    }
    case TIMER0_CHANNEL1:
    {
      Timer_fpTimer1Callback = fpUserCallback_;
      break;
    }
    case TIMER0_CHANNEL2:
    {
      break;
    }
    default:
    {
      break;
    }
  }   
} /* end TimerAssignCallBack() */




/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

  /*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerDefaultCallBack(void)

@brief An empty function that the timer Callback points to by default. Expected 
that the user will set their own.

Requires:
Nothing. 
*/


static void TimerDefaultCallBack(void)
{
  

  
} /* end TimerDefaultCallBack() */

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TimerInitialize(void)

@brief Initializes the State Machine and its variables.

Requires:
- NONE

Promises:
- Timer 1 is configured per timer.h INIT settings

*/
void TimerInitialize(void)
{
  /* Load the block configuration regusters*/
  
  AT91C_BASE_TCB0->TCB_BMR = TCB_BMR_INIT;
  
  /* Load Channel 1 settings */
  
  AT91C_BASE_TC1->TC_CMR = TC1_CMR_INIT;
  AT91C_BASE_TC1->TC_RC = TC1_RC_INIT;
  AT91C_BASE_TC1->TC_IER  = TC1_IER_INIT;
  AT91C_BASE_TC1->TC_IDR  = TC1_IDR_INIT;    
  AT91C_BASE_TC1->TC_CCR  = TC1_CCR_INIT;

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    
    NVIC_ClearPendingIRQ(IRQn_TC1);
    NVIC_EnableIRQ(IRQn_TC1);
    Timer_StateMachine = TimerSM_Idle;
    

    Timer_fpTimer1Callback = TimerDefaultCallBack;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Timer_fpStateMachine = TimerSM_Error;
  }

} /* end TimerInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void TimerRunActiveState(void)
{
  Timer_fpStateMachine();

} /* end TimerRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void TC1_IrqHandler(void)

@brief Parses the TC1 interrupts and handles them appropriately.  

Note that all enabled TC1 interrupts are ORed and will trigger this handler, 
therefore any expected interrupt that is enabled must be parsed out 
and handled.

Requires:
- NONE

Promises:

If Channel1 RC interrupt:
- Timer Channel 1 is reset and automatically restarts counting
- AT91C_TC_CPCS is cleared
- Associated callback function is invoked
- IRQn_TC1 interrupt flag is cleared

*/
void TC1_IrqHandler(void)
{
  /* Check for RC compare interrupt - reading TC_SR clears the bit if set */
  if(AT91C_BASE_TC1->TC_SR & AT91C_TC_CPCS)
  {
    Timer_u32Timer1IntCounter++;
    Timer_fpTimer1Callback();
  }

  /* Clear the TC0 pending flag and exit */
  NVIC_ClearPendingIRQ(IRQn_TC1);
  
} /* end TC1_IrqHandler() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
  
/*!----------------------------------------------------------------------------------------------------------------------
@fn static void TimerDefaultCallback(void)

@brief An empty function that the Timer Callback points to.  Expected that the 
user will set their own.

Requires:
- NONE 

Promises:
- NONE 

*/
static void TimerDefaultCallback(void)
{
  
} /* end TimerDefaultCallback() */


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TimerSM_Idle(void)

@brief Wait for a message to be queued 
*/static void TimerSM_Idle(void)
{
   
} /* end TimerSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void TimerSM_Error(void)          
{
  
} /* end TimerSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
