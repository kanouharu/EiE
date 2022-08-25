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

Protected:
- 

***********************************************************************************************************************/

#include "configuration.h"

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


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Button_xx" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Button_pfnStateMachine;                  /*!< @brief The Button application state machine function pointer */

static ButtonStatusType Button_asStatus[U8_TOTAL_BUTTONS];
extern const PinConfigurationType G_asBspButtonConfigurations[U8_TOTAL_BUTTONS];


/************ EDIT BOARD-SPECIFIC GPIO DEFINITIONS ABOVE ***************/


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public Functions */
/*--------------------------------------------------------------------------------------------------------------------*/



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
*/
void ButtonInitialize(void)
{

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



  

} /* end ButtonInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
Function ButtonRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
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
  
  

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions

The button state machine monitors button activity and manages debouncing and
maintaining the global button states.
***********************************************************************************************************************/

/*!--------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Idle(void)

@brief Simply look for at least one button to enter debouncing state, then modify 
state machine function pointer to trigger the active state

*/
static void ButtonSM_Idle(void)                
{
  
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
   
   Button_pfnStateMachine = ButtonSM_Idle();   
   
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
      
    /* Check if debounce period is over */
      if( IsTimeUp(&Button_asStatus[i].u32DebounceTimeStart, U32_DEBOUNCE_TIME))
      {
        /* Active low */
        if(G_asBspButtonConfigurations[i].eActiveState == ACTIVE_LOW){
          
          
          /* Read PIO_PDSR to get the actual input signal(new button state) */
          
          
        }
        /* Active high */
        else 
        {
          
          
          /* Read PIO_PDSR to get the actual input signal(new button state) */

          
          
        }
        
        /* Update if the button state has changed */
        if(Button_asStatus[i].eNewState != Button_asStatus[i].eCurrentState)
        {
          /* If the new state is PRESSED, update the new press flag */
          if(Button_asStatus[i].eCurrentState == PRESSED)
          {
            
            
          //  
            
            
            
          }
          
          
          
        
        
        }
        
        /* Regardless of a good press or not, clear the debounce active flag and 
        re-enable the interrupts */
        
        
      
    } /* end if( IsTimeUp.....) */
    
   } /*   if(Button_asStatus[i].... */
   
   
  } /* end  for(u8 i = 0; i < U8_TOTAL_BUTTONS;i++) */
   
   
   
 }/* end ButtonSM_ButtonActive */

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void ButtonSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void ButtonSM_Error(void)          
{
  
} /* end ButtonSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
