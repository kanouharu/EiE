/*!**********************************************************************************************************************
@file timer.c                                                                
@brief Provide easy access to set up and run a Timer Counter (TC) Peripheral.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 

PUBLIC FUNCTIONS
- 
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
static fnCode_type Timer_StateMachine;            /*!< @brief The state machine function pointer */


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
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
  /* Enable counter if disabled using switches because I like switches */
  switch ((AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR) {
    case TC1_CCR_INIT:
      (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR ^= (u32)0x00000002;
    default:
      break;
      
  }
  
} /* end TimerStart() */

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
  if((AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR != TC1_CCR_INIT) {
    
      (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR = TC1_CCR_INIT;
  }
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
  return (u16)(AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CV;
} /* end TimerSet() */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerAssignCallBack(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_ )

@brief  Sets callback, presumably for TC interrupt.

Requires:
-
@param eTimerChannel_ holds a valid channel
@param fpUserCallback_ is a fn pointer

Promises:
- 

*/


void TimerAssignCallBack(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_ )
{
  
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
   
  /* Load the new timer value */
  
  //
  
} /* end TimerSet() */



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TimerInitialize(void)

@brief Initializes the State Machine and its variables.

Requires:
- NONE

Promises:
- 

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
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Timer_StateMachine = TimerSM_Error;
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
  Timer_StateMachine();

} /* end TimerRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
  

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
