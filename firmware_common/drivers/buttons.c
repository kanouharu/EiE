/*!**********************************************************************************************************************
@file buttons.c                                                                
@brief Button functions and state machine.  

The application handles all debouncing and button press / hold detection.
All buttons use interrupts to trigger the start and end
of the action.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- 

TYPES
- NONE


PUBLIC FUNCTIONS
- 

PROTECTED FUNCTIONS
- 

***********************************************************************************************************************/

#include "configuration.h"
#include "utilities.h"



/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Button"
***********************************************************************************************************************/
/* New variables */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                     /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                      /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                       /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                  /*!< @brief From main.c */

extern const PinConfigurationType G_asBspButtonConfigurations[U8_TOTAL_BUTTONS]; /*!< @brief from board-specific file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Button_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Button_pfnStateMachine;                  /*!< @brief The Button application state machine function pointer */

<<<<<<< HEAD
static ButtonStatusType Button_asStatus[U8_TOTAL_BUTTONS];
extern const PinConfigurationType G_asBspButtonConfigurations[U8_TOTAL_BUTTONS];


/************ EDIT BOARD-SPECIFIC GPIO DEFINITIONS ABOVE ***************/
=======
static ButtonStatusType Button_asStatus[U8_TOTAL_BUTTONS];  /*!< @brief Individual status parameters for buttons */
>>>>>>> tick_timer


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
/*!----------------------------------------------------------------------------------------------------------------------
@fn bool IsButtonPressed(ButtonNameType eButton_)

@brief Determine if a particular button is currently pressed at the momment in 
when the function is called.

/*!----------------------------------------------------------------------------------------------------------------------
@fn bool IsButtonPressed(ButtonNameType eButton_)

<<<<<<< HEAD
Requires:
  - Button_asStatus[eButton_] is a valid index
 
@param eButton_ is a valid button

Promises:
  - Returns TRUE if Button_asStatus[eButton_].eCurrentState is PRESSED
  - Otherwise, returns FALSE
*/

bool IsButtonPressed(ButtonNameType eButton_)
{

  /*Check button for pressed flag*/

    if(Button_asStatus[eButton_].eCurrentState == PRESSED){
      
      return(TRUE);
    }
    else {
      
      return(FALSE);     
      }
    


  

} /* end IsButtonPressed() */


  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonAcknowledged(ButtonNameType eButton_)

@brief Clears a specific button's bNewPressFlag

Requires:
  - Button_asStatus[eButton_] is a valid index
 
@param eButton_ is a valid button

Promises:
- Button_asStatus[eButton_].bNewPressFlag will be assigned the value of FALSE


*/

void ButtonAcknowledged(ButtonNameType eButton_)
{

  /*Clear specific button's bNewPressFlag*/

     Button_asStatus[eButton_].bNewPressFlag = FALSE;
   
} /* end ButtonAcknowledged() */

/*!----------------------------------------------------------------------------------------------------------------------

@fn bool WasButtonPressed(ButtonNameType eButton_)

@brief Determine if a particular button was pressed since this function was called


Requires:
  - Button_asStatus[eButton_] is a valid index
 
@param eButton_ is a valid button

Promises:
  - Returns TRUE if Button_asStatus[eButton_].bNewPressFlag is TRUE (Not Acknowledged)
  - Otherwise, returns FALSE
*/

bool WasButtonPressed(ButtonNameType eButton_)
{

  /*Check button was pressed*/

    if(Button_asStatus[eButton_].bNewPressFlag == TRUE){
      ButtonAcknowledged(eButton_);
      return(TRUE);
    }
    else {
      
      return(FALSE);     
      }
    


  

} /* end WasButtonPressed() */
/*!----------------------------------------------------------------------------------------------------------------------

@fn bool IsButtonHeld(ButtonNameType eButton_)

@brief Determine if a particular button has been held for a certain period of time

Requires:
  - Button_asStatus[eButton_] is a valid index
 
@param eButton_ is a valid button

Promises:
  - Returns TRUE if Button_asStatus[eButton_].bNewPressFlag is TRUE (Not Acknowledged)
  - Otherwise, returns FALSE
*/

bool IsButtonHeld(ButtonNameType eButton_)
{

  /*Check button is being held*/

    if(Button_asStatus[eButton_].eCurrentState == PRESSED &&
       IsTimeUp(&Button_asStatus[eButton_].u32DebounceTimeStart, U32_HOLD_TIME)){
       

      return(TRUE);
    }
    else {

      return(FALSE);     
      }
    


  

} /* end WasButtonPressed() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected Functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
Function: ButtonInitialize

Description:


Requires:
  - 
 
Promises:
  - 
=======
@brief Determine if a particular button is currently pressed at the moment in time when 
the function is called.

The button must still be pressed at the time of this inquiry for the function
to return TRUE.

Requires:
- Button_asStatus[eButton_] is a valid index

@param eButton_ is a valid button 
 
Promises:
- Returns TRUE if Button_asStatus[eButton_].eCurrentState is PRESSED 
- Otherwise returns FALSE

*/
bool IsButtonPressed(ButtonNameType eButton_)
{
  if( Button_asStatus[(u8)eButton_].eCurrentState == PRESSED)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }

} /* end IsButtonPressed() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn bool WasButtonPressed(ButtonNameType eButton_)

@brief Determines if a particular button was pressed since last time it was checked. 

This is effectively a latching function so that button presses are
not missed and are potentially available to multiple tasks.

The button may or may not still be pressed when this inquiry is made.  Mulitple
button presses are not tracked.  The user should call ButtonAcknowledge immediately
following this function to clear the state. If multiple tasks need the button 
information, only the last function should call ButtonAcknowledge.

Requires:
- Button_asStatus[eButton_] is a valid index

@param eButton_ is a valid button 
 
Promises:
- Returns TRUE if Button_asStatus[eButton_].bNewPressFlag is TRUE 
- Otherwise returns FALSE

*/
bool WasButtonPressed(ButtonNameType eButton_)
{
  if( Button_asStatus[(u8)eButton_].bNewPressFlag == TRUE)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }

} /* end WasButtonPressed() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonAcknowledge(ButtonNameType eButton_)

@brief Clears the "New Press" flag of a button so WasButtonPressed() no longer returns TRUE
unless a new button press occurs.

This function is generally always called after WasButtonPressed() returns TRUE unless 
it is known that other tasks may need the button pressed information.

Requires:
@param eButton_ is a valid button index
 
Promises:
- The flag at Button_asStatus[eButton_].bNewPressFlag is set to FALSE

*/
void ButtonAcknowledge(ButtonNameType eButton_)
{
  Button_asStatus[(u8)eButton_].bNewPressFlag = FALSE;

} /* end ButtonAcknowledge() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn bool IsButtonHeld(ButtonNameType eButton_, u32 u32ButtonHeldTime_)

@brief Queries to see if a button has been held for a certain time.  

The button must still be pressed when this function is called if it is to return TRUE.
This is a non-latching function.

Requires:
@param eButton_ is a valid button index
@param u32ButtonHeldTime is a time in ms 
 
Promises:
- Returns TRUE if eButton_ has been held longer than u32ButtonHeldTime_
- Otherwise returns FALSE

*/
bool IsButtonHeld(ButtonNameType eButton_, u32 u32ButtonHeldTime_)
{
 if( (Button_asStatus[(u8)eButton_].eCurrentState == PRESSED) && 
      IsTimeUp(&Button_asStatus[eButton_].u32DebounceTimeStart, u32ButtonHeldTime_ ) )
 {
   return(TRUE);
 }
 else
 {
   return(FALSE);
 }

} /* end IsButtonHeld() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonInitialize(void)

@brief Runs required initialization for the task.  

Should only be called once in main init section.


Requires:
- NONE
 
Promises:
- The Button task is configured
- Button interrupts are active on PIOA and PIOB
- Button task is set to ButtonSM_Idle

>>>>>>> tick_timer
*/
void ButtonInitialize(void)
{
  u32 u32Dummy;
  
  /* Setup default data for all of the buttons in the system */
  for(u8 i = 0; i < U8_TOTAL_BUTTONS; i++)
  {
    Button_asStatus[i].bNewPressFlag = FALSE;
    Button_asStatus[i].eCurrentState = RELEASED;
    Button_asStatus[i].eNewState     = RELEASED;
    Button_asStatus[i].u32TimeStamp  = 0;
  }

<<<<<<< HEAD
  /* Setup default data  for all of the buttons in the system*/
  
  for(u8 i = 0; i < U8_TOTAL_BUTTONS;i++)
  {
    Button_asStatus[i].bDebounceActive = FALSE;
    Button_asStatus[i].bNewPressFlag = FALSE;
    Button_asStatus[i].eCurrentState = RELEASED;
    Button_asStatus[i].eNewState  = RELEASED;
    Button_asStatus[i].u32DebounceTimeStart = 0;
    Button_asStatus[i].u32TimeStamp = 0;
  }
    
    
    /* Enable PIO interrupts */
    AT91C_BASE_PIOA->PIO_IER = GPIOA_BUTTONS;
    AT91C_BASE_PIOB->PIO_IER = GPIOB_BUTTONS;
    
    /* Dummy code to read the ISR registers and clear the flags */
    u32Dummy u32BitTrapA = AT91C_BASE_PIOA->PIO_ISR; 
    u32Dummy u32BitTrapB = AT91C_BASE_PIOB->PIO_ISR; 

    /* Configure the NVIC to ensure the PIOA and PIOB interrups are active */
    NVIC_ClearPendingIRQ(IRQn_PIOA);
    NVIC_ClearPendingIRQ(IRQn_PIOB);
    NVIC_EnableIRQ(IRQn_PIOA);
    NVIC_EnableIRQ(IRQn_PIOB);
    
    /* Init complete: set fuction pointer and application flag! */
    Button_pfnStateMachine = ButtonSM_Idle;



  
=======
  /* Enable PIO interrupts */
  AT91C_BASE_PIOA->PIO_IER = GPIOA_BUTTONS;
  AT91C_BASE_PIOB->PIO_IER = GPIOB_BUTTONS;
  
  /* Dummy code to read the ISR registers and clear the flags */
  u32Dummy  = AT91C_BASE_PIOA->PIO_ISR;
  u32Dummy |= AT91C_BASE_PIOB->PIO_ISR;

  /* Configure the NVIC to ensure the PIOA and PIOB interrupts are active */
  NVIC_ClearPendingIRQ(IRQn_PIOA);
  NVIC_ClearPendingIRQ(IRQn_PIOB);
  NVIC_EnableIRQ(IRQn_PIOA);
  NVIC_EnableIRQ(IRQn_PIOB);
    
  /* Init complete: set function pointer and application flag */
  Button_pfnStateMachine = ButtonSM_Idle;
>>>>>>> tick_timer

} /* end ButtonInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
<<<<<<< HEAD
Function ButtonRunActiveState()
=======
@fn ButtonRunActiveState()

@brief Selects and runs one iteration of the current state in the state machine.
>>>>>>> tick_timer

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void ButtonRunActiveState(void)
{
  Button_pfnStateMachine();

} /* end ButtonRunActiveState */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonStartDebounce(u32 u32BitPosition_, PortOffsetType ePort_)

<<<<<<< HEAD
@brief Called only from ISR: sets the "debounce active" flag and debounce start time

Requires:
- Only that the PIOA or PIOB ISR should call this function

@param u32BitPosition_ is a SINGLE bit for the button pin to start debouncing
@param ePort_ is the port on which the button is located  

Promises:
- If the indicated button is found in G_asBspButtonConfigurations, then the
 corresponding interrupt is disabled and debounce information is set in Button_as-
Status
*/

void ButtonStartDebounce(u32 u32BitPosition_, PortOffsetType ePort_)
{
  
  ButtonNameType eButton = NOBUTTON;
  
  /* Parse through to find the button */
  
  for (u8 i = 0; i < U8_TOTAL_BUTTONS;i++){
    
    if ((u32BitPosition_ == G_asBspButtonConfigurations[i].u32BitPosition ) &&
         (ePort_ == G_asBspButtonConfigurations[i].ePort))
      {
        
        eButton = (ButtonNameType)i;
        break;
      }
  }
  
    /* If the button has been found, disable the interrupt and update debounce status */
  if(eButton != NOBUTTON){
        /* Configure Interrupt Disable Address */
        u32 *pu32DisableAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IDR) + ePort_);
        
             
        /* Interrupt source flag for button */
        *pu32DisableAddress |= u32BitPosition_;
        
        /* Set debounce flag and debounce time stamp */
        Button_asStatus[(u8)eButton].bDebounceActive = TRUE;
        Button_asStatus[(u8)eButton].u32DebounceTimeStart = G_u32SystemTime1ms;
   
  }
  } /* end ButtonStartDebounce */
  
=======
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonStartDebounce(u32 u32BitPosition_, PortOffsetType ePort_)
>>>>>>> tick_timer

@brief Called only from ISR: sets the "debounce active" flag and debounce start time  

Requires:
- Only the PIOA or PIOB ISR should call this function

@param u32BitPosition_ is a SINGLE bit and actual button location on which to start debouncing
@param ePort_ is the port on which the button is located

Promises:
- If the indicated button is found in G_asBspButtonConfigurations, then the
corresponding interrupt is disabled and debounce information is set in Button_asStatus

*/
void ButtonStartDebounce(u32 u32BitPosition_, PortOffsetType ePort_)
{
  ButtonNameType eButton = NOBUTTON;
  
  /* Parse through to find the button */
  for(u8 i = 0; i < U8_TOTAL_BUTTONS; i++)
  {
    if( (G_asBspButtonConfigurations[i].u32BitPosition == u32BitPosition_) &&
        (G_asBspButtonConfigurations[i].ePort == ePort_) )
    {
      eButton = (ButtonNameType)i;
      break;
    }
  }
  
  /* If the button has been found, disable the interrupt and update debounce status */
  if(eButton != NOBUTTON)
  {
    AT91C_BASE_PIOA->PIO_IDR |= u32BitPosition_;
    Button_asStatus[(u8)eButton].bDebounceActive = TRUE;
    Button_asStatus[(u8)eButton].u32DebounceTimeStart = G_u32SystemTime1ms;
  }
  
} /* end ButtonStartDebounce() */
 

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions

The button state machine monitors button activity and manages debouncing and
maintaining the global button states.
***********************************************************************************************************************/

<<<<<<< HEAD


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_ButtonActive(void)

@brief Monitor debouncing buttons, manage their flags upon registering a valid press or
release, and also re-enable interrupt.

Requires:
- 


Promises:
- 
*/

 static void ButtonSM_ButtonActive(void)
 {
   u32 *pu32PortAddress;
   u32 *pu32InterruptAddress;
   
   /* Assume no buttons are active and reset SM pointer to Idle state */
   
   Button_pfnStateMachine = ButtonSM_Idle;   
   
   /* check for buttons that are debouncing */
   
    for(u8 i = 0; i < U8_TOTAL_BUTTONS;i++)
    {
    /* Load address offsets for the current button */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) +
                             G_asBspButtonConfigurations[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) +
                             G_asBspButtonConfigurations[i].ePort);    
    
    /* Check if the current button is debouncing */
    if(Button_asStatus[i].bDebounceActive)
    {
      /* Keep SM pointer on Active */
      
         Button_pfnStateMachine = ButtonSM_ButtonActive;   
         

    /* Check if debounce period is over */
      if(IsTimeUp(&Button_asStatus[i].u32DebounceTimeStart, U32_DEBOUNCE_TIME))
      {
      
        /* Active low */
        if(G_asBspButtonConfigurations[i].eActiveState == ACTIVE_LOW){
          
          
          /* Read PIO_PDSR to get the actual input signal(new button state) */
          
          if( ~(*pu32PortAddress) & G_asBspButtonConfigurations[i].u32BitPosition)
          {
            Button_asStatus[i].eNewState = PRESSED;
          
          }
        
        /* Active high */
        else 
        {       
            Button_asStatus[i].eNewState = RELEASED;
        }
       
        
        }
        
        /* Update if the button state has changed */
        if(Button_asStatus[i].eNewState != Button_asStatus[i].eCurrentState)
        {
          Button_asStatus[i].eCurrentState = Button_asStatus[i].eNewState;
                                              
                                              
          /* If the new state is PRESSED, update the new press flag */
          if(Button_asStatus[i].eCurrentState == PRESSED)
          {
            
            
          Button_asStatus[i].bNewPressFlag = TRUE;
          Button_asStatus[i].u32TimeStamp = G_u32SystemTime1ms; 
                     
            
          }
          
          
          
        
        
        }
        
        /* Regardless of a good press or not, clear the debounce active flag and 
        re-enable the interrupts */
        
        Button_asStatus[i].bDebounceActive = FALSE;
        *pu32InterruptAddress = G_asBspButtonConfigurations[i].u32BitPosition;
      
        
    } /* end if( IsTimeUp.....) */
    
   } /*   if(Button_asStatus[i].... */
   
   
  } /* end  for(u8 i = 0; i < U8_TOTAL_BUTTONS;i++) */
   
   
   
 }/* end ButtonSM_ButtonActive */

/*!--------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Idle(void)

@brief Simply look for at least one button to enter debouncing state, then modify 
state machine function pointer to trigger the active state

=======
/*!-------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Idle(void)

@brief Look for at least one button to enter debouncing state
>>>>>>> tick_timer
*/
static void ButtonSM_Idle(void)                
{
  for(u8 i = 0; i < U8_TOTAL_BUTTONS; i++)
  {
    if(Button_asStatus[i].bDebounceActive)
    {
      Button_pfnStateMachine = ButtonSM_ButtonActive;
      break;
    }
  }
  
   /**/
  
  for(u8 i = 0; i < U8_TOTAL_BUTTONS;i++)
  {
   if(Button_asStatus[i].bDebounceActive == TRUE)
   {
     Button_pfnStateMachine = ButtonSM_ButtonActive;
     break;
   }
   
  }
  
} /* end ButtonSM_Idle(void) */
/*!-------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_ButtonActive(void)

@brief Process each button that is debouncing.

Time out the debounce period and set the "pressed" state if button action is confirmed.
Manage the hold timers.
*/
static void ButtonSM_ButtonActive(void)         
{
  u32 *pu32PortAddress;
  u32 *pu32InterruptAddress;

  /* Start by resetting back to Idle in case no buttons are active */
  Button_pfnStateMachine = ButtonSM_Idle;

  /* Check for buttons that are debouncing */
  for(u8 i = 0; i < U8_TOTAL_BUTTONS; i++)
  {
    /* Load address offsets for the current button */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) + G_asBspButtonConfigurations[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) + G_asBspButtonConfigurations[i].ePort);
    
    /* Check if the current button is debouncing */
    if( Button_asStatus[i].bDebounceActive )
    {
      /* Still have an active button */
      Button_pfnStateMachine = ButtonSM_ButtonActive;
      
      /* Check if debounce period is over */
      if( IsTimeUp(&Button_asStatus[i].u32DebounceTimeStart, U32_DEBOUNCE_TIME) )
      {
        /* Active low */
        if(G_asBspButtonConfigurations[i].eActiveState == ACTIVE_LOW)
        {
          /* Read PIO_PDSR to get the actual input signal (new button state) */
          if( ~(*pu32PortAddress) & G_asBspButtonConfigurations[i].u32BitPosition )
          {          
            Button_asStatus[i].eNewState = PRESSED;
          }
          else
          {
            Button_asStatus[i].eNewState = RELEASED;
          }
        }
        /* Active high */
        else
        {
          /* Read PIO_PDSR to get the actual input signal (new button state) */
          if( *pu32PortAddress & G_asBspButtonConfigurations[i].u32BitPosition )
          {          
            Button_asStatus[i].eNewState = PRESSED;
          }
          else
          {
            Button_asStatus[i].eNewState = RELEASED;
          }
        }
        
        /* Update if the button state has changed */
        if( Button_asStatus[i].eNewState != Button_asStatus[i].eCurrentState )
        {
          Button_asStatus[i].eCurrentState = Button_asStatus[i].eNewState;
          
          /* If the new state is PRESSED, update the new press flag */
          if(Button_asStatus[i].eCurrentState == PRESSED)
          {
            Button_asStatus[i].bNewPressFlag = TRUE;
            Button_asStatus[i].u32TimeStamp  = G_u32SystemTime1ms;
          }
        }

        /* Regardless of a good press or not, clear the debounce active flag and re-enable the interrupts */
        Button_asStatus[i].bDebounceActive = FALSE;
        *pu32InterruptAddress = G_asBspButtonConfigurations[i].u32BitPosition;
        
      } /* end if( IsTimeUp...) */
    } /* end if(Button_asStatus[i].bDebounceActive) */
  } /* end for (u8 i = 0; i < U8_TOTAL_BUTTONS; i++) */
  
} /* end ButtonSM_ButtonActive() */


#if 0
/*!-------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/

#if 0
static void ButtonSM_Error(void)          
{
  
} /* end ButtonSM_Error() */
#endif


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
