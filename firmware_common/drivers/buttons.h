/***********************************************************************************************************************
File: buttons.h                                                                
***********************************************************************************************************************/

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "configuration.h"

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/
/*!
@enum ButtonStateType
@brief Self-documenting button state type */

typedef enum {RELEASED, PRESSED} ButtonStateType;

/*!
@struct ButtonStatusType
@brief Required parameters for the task to track what each button is doing.
*/

typedef struct
{
  bool bDebounceActive;           /*!< True by ISR if a button interrupt occurs */
  bool bNewPressFlag;              /*!< TRUE if press not acknowledged */
  ButtonStateType eCurrentState;  /*!< Current state of the button */
  ButtonStateType eNewState;      /*!< New state of the button */
  u32 u32DebounceTimeStart;       /*!< Time loaded by ISR when interrupt occurs */
  u32 u32TimeStamp;               /*!< Time when button was pressed */

  
} ButtonStatusType;


/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/

static u32 U32_DEBOUNCE_TIME = 5;

static u32 U32_HOLD_TIME = 500;

/***********************************************************************************************************************
Function Declarations
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void ButtonInitialize(void);                        
void ButtonRunActiveState(void);
void ButtonStartDebounce(u32 u32BitPosition_, PortOffsetType ePort_);
bool IsButtonPressed(ButtonNameType eButton_);
void ButtonAcknowledged(ButtonNameType eButton_);
bool IsButtonPressed(ButtonNameType eButton_);
bool WasButtonPressed(ButtonNameType eButton_);
bool IsButtonHeld(ButtonNameType eButton_);



/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void ButtonSM_Idle(void);                
static void ButtonSM_Error(void);        


#endif /* __BUTTONS_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
