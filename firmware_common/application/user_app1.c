/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;                 /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                         /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1TimerCallback(void)

@brief Toggles LED at Timer1 interrupt.

Requires:
- Automatically called from Timer Interrupt

Headcannon: 

- CYAN is summoned to or from MIDICITY

Promises:

- CYAN LED is toggled.

*/

 void UserApp1TimerCallback(void)
{
  
LedToggle(CYAN);

} /* end UserApp1TimerCallback */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ButtonTest(void)

@brief Crude PIO Interrupt and Button API test

Requires:
- WHITE,PURPLE, and CYAN leds aren't being used elsewhere;
 Interrupts are set up properly;
 Button Driver properly initialized.

Promises:
- You can play with some buttons, demonstraiting press and hold detection

*/
void ButtonTest(void)
{
   /* Crude Button Driver Testing */
  
  if( IsButtonPressed(BUTTON0)){
    
      LedOn(WHITE);
  }
  else {
    LedOff(WHITE);
  }
  
  if(IsButtonHeld(BUTTON0, 10)){
    LedOn(PURPLE);
     }
  if(IsButtonPressed(BUTTON3)){
    LedOn(CYAN);
    LedOff(PURPLE);
      }
   else{
    LedOff(CYAN);
      }
} /* end ButtonTest */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerTest(void)

@brief Code that seemed to cause hard-faults when added to UserApp1. Supposedly
    demonstrates that TC1 (Channel 1 of the single TCB0 perf) and related interrupt
    is configured and triggering the CYAN LED in sync with another LED blinking
    in the main loop. Abstracting this code away so I can easily comment out.

Requires:
- PURPLE and CYAN leds aren't being used elsewhere;
 Interrupts are set up properly;
 Button Driver properly initialized.

Promises:
- Hardfaulting around return from TimerStart, Watchdogbone, somewhere in
the ButtonDriver, and seemingly random other places (Due to TC-interrupt happening 
reguardless of what the main loop is doing). Lots of frustration.

Realization: Callback assignment never works as interrupt code isn't able to
see the function pointer. No implementation of the working interrupt module code
is avalible in any online respository, even the final project branches I've checked.
 Better to just ignore the problem as a solution is not needed to understand the
 fundamental ideas behind TCs and their uses. 
*/
void TimerTest(void)
{
   /*TC Driver Testing */
  
 /*Initalize LEDs and queue CYAN to blink */
  
  LedOff(CYAN);
  LedOff(PURPLE);
  LedBlink(PURPLE,LED_4HZ);
  
  /* Setup Timer1 to clock out 125ms periods. 125ms / 2,66666us = 46875 */
  
  
  //TimerAssignCallback(TIMER0_CHANNEL1, UserApp1TimerCallback);
 
  // TimerStop(TIMER0_CHANNEL1);
  TimerSet(TIMER0_CHANNEL1, 46875);
  TimerStart(TIMER0_CHANNEL1);
  
      
} /* end TimerTest */

/*!----------------------------------------------------------------------------------------------------------------------
@fn void BinaryClock(void)

@brief Abstraction of the Binary Clock project found in an archive of
the online supplementary materials for the EIE program

Requires:
- WHITE,PURPLE, and CYAN leds aren't being used elsewhere;
- GPIO configured

*/
void BinaryClock(void)
{


  static u16 u16BlinkCount = 0;
  static u8 u8BinaryCounter = 0;

  u16BlinkCount++;

/* All discrete LEDs to off 
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  */
  /* Backlight to white */  
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOn(LCD_BLUE);


  /* 500ms check and reset */
  if(u16BlinkCount == 500)
    {
      u16BlinkCount = 0;
      
      /* Binary counter check and reset at 16 */
      if ( ++u8BinaryCounter == 16)
      {
        
        u8BinaryCounter = 0;
      }
      
      //LedToggle(PURPLE);
    }
  
     /* Parse the current count to set the LEDs.  
      RED is bit 0, ORANGE is bit 1, 
      YELLOW is bit 2, GREEN is bit 3. */
    
    if(u8BinaryCounter & 0x01)
    {
      LedOn(RED);
    }
    else
    {
      LedOff(RED);
    }

    if(u8BinaryCounter & 0x02)
    {
      LedOn(ORANGE);
    }
    else
    {
      LedOff(ORANGE);
    }

    if(u8BinaryCounter & 0x04)
    {
      LedOn(YELLOW);
    }
    else
    {
      LedOff(YELLOW);
    }

    if(u8BinaryCounter & 0x08)
    {
      LedOn(GREEN);
    }
    else
    {
      LedOff(GREEN);
    }

} /* end BinaryClock */
/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.
 
Currently dumping ground for various crude tests of ever expanding functionality
of the EiE system.


Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{ 
 // TimerTest();

  /* If good initialization, set state to Idle */
  
  PWMAudioSetFrequency(BUZZER1, 1000);
  PWMAudioSetFrequency(BUZZER2, 200);

  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Idle(void)

@brief What does this function do?
*/
static void UserApp1SM_Idle(void)
{
  
 BinaryClock(); 

 /* BUZZER1 is on if BUTTON1 was pressed */
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    PWMAudioOn(BUZZER1);
  }
 
  /* BUZZER2 is on if BUTTON2 was pressed */
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    PWMAudioOn(BUZZER2);
  }
  
  /* Both buzzers off if BUTTON3 was pressed */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    PWMAudioOff(BUZZER1);
    PWMAudioOff(BUZZER2);
  }
} /* end UserApp1SM_Idle() */
    

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
