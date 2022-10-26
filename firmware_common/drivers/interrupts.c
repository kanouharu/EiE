/*!*********************************************************************************************************************
@file interrupts.c                                                               
@brief Definitions for main system interrupts.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- 
PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- 


***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>ISR"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword)  */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variables names shall start with "ISR_<type>" and be declared as static.
***********************************************************************************************************************/



/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/




/*!--------------------------------------------------------------------------------------------------------------------
@fn void InterruptSetup(void)

@brief Disables and clears all NVIC interrupts and sets up interrupt priorities.


Requires:
- IRQn_Type enum is the sequentially ordered interrupt values starting at 0

Promises:
- Interrupt priorities are set
- All NVIC interrupts are disabled and all pending flags are cleared
*/


void InterruptSetup(void)
{
  static const u32 au32PriorityConfig[PRIORITY_REGISTERS] = {IPR0_INIT, IPR1_INIT,
    IPR2_INIT, IPR3_INIT, IPR4_INIT, IPR5_INIT, IPR6_INIT, IPR7_INIT};
                                                    
                                                    
/* Disable all interrupts and ensure pending bits are clear */
  for (u8 i = 0; i < SAM3U2_INTERRUPT_SOURCES; i++)
  {
    NVIC_DisableIRQ(  (IRQn_Type)i  );
    NVIC_ClearPendingIRQ( (IRQn_Type) i);
  }
  /* Set interrupt priorities*/
  
  for (u8 i = 0; i < PRIORITY_REGISTERS; i++)
  {
    ((u32*) (AT91C_BASE_NVIC->NVIC_IPR))[i] = au32PriorityConfig[i];
  }
} /* end InterruptSetup (void) */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
Interrupt Service Routine Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!-------------------------------------------------------------------------------------------------------------------
@fn ISR void PIOA_Irqhandler(void)

@brief Parses the PORTA GPIO interrupts and handles them appropriately.

Note that all PORTA GPIO interrupts are ORed and will trigger this handler,
so any expected interrupt that is enabled must be parsed out and handled.

Requires:
- The button IO bits match the interrupt flag locations

Promises:
- Buttons: sets the active button's deboucing flag, clears the interrupt
  and initializes the button's debounce timer.
*/

void PIOA_IrqHandler(void)
{
  /************ NO BREAKPOINTS BEFORE PIO_ISR READS, FOR THE LOVE OF ARM!
    A debuggering viewing this register will clear flags needed!! */

    u32 u32GPIOInterruptSources;
    u32 u32ButtonInterrupts;
    u32 u32CurrentButtonLocation;

   
    u32GPIOInterruptSources = AT91C_BASE_PIOA->PIO_ISR;

  /*********** BREAKPOINT-SAFE ZONE STARTS HERE.  <3 */

  /* Mask out irrelevant PIOA interrupt flags from signal */
   u32ButtonInterrupts = (u32GPIOInterruptSources & GPIOA_BUTTONS ); 
   
  /* Check if Button signal was pressent in flags */
   if (u32ButtonInterrupts)
   {
     
   /* Scan though the flags looking for those that are set. */
    u32 u32CurrentButtonLocation = 0x00000001;
    
    for(u8 i = 0; i < 31; i++)
    {
           
      /* If the bit is set, then start debouncing */
     
     if(u32ButtonInterrupts & u32CurrentButtonLocation)
     {
     ButtonStartDebounce(u32CurrentButtonLocation, PORTA);
    }
    
     //My First Practical Logical Bit Shift!!! Yay! So Cool! 
     u32CurrentButtonLocation <<= 1; 
     
    }
    
   }
  /* Clear the PIOA pending flag and exit */
  NVIC_ClearPendingIRQ(IRQn_PIOA);
} /* end PIOA_IrqHandler()  */

/*!-------------------------------------------------------------------------------------------------------------------
@fn ISR void PIOB_Irqhandler(void)

@brief Parses the PORTB GPIO interrups and handles them appropriately.

Note that all PORTB GPIO interrupts are ORed and will trigger this handler,
so any expected interrupt that is enabled must be parsed out and handled.

Requires:
- The button IO bits match the interrupt flag locations

Promises:
- Buttons: sets the active button's deboucing flag, clears the intterupt
  and initializes the button's debounce timer.
*/

void PIOB_IrqHandler(void)
{
  /************ NO BREAKPOINTS BEFORE PIO_ISR READS, FOR THE LOVE OF ARM!
    A debuggering viewing this register will clear flags needed!! */

    u32 u32GPIOInterruptSources;
    u32 u32ButtonInterrupts;
    u32 u32CurrentButtonLocation;

   
    u32GPIOInterruptSources = AT91C_BASE_PIOB->PIO_ISR;

  /*********** BREAKPOINT-SAFE ZONE STARTS HERE.  <3 */

  /* Mask out irrelevant PIOA interrupt flags from signal */
   u32ButtonInterrupts = (u32GPIOInterruptSources & GPIOB_BUTTONS ); 
   
  /* Check if Button signal was pressent in flags */
   if (u32ButtonInterrupts)
   {
     
   /* Scan though the flags looking for those that are set. */
    u32 u32CurrentButtonLocation = 0x00000001;
    
    for(u8 i = 0; i < 31; i++)
    {
           
      /* If the bit is set, then start debouncing */
     
     if(u32ButtonInterrupts & u32CurrentButtonLocation)
     {
     ButtonStartDebounce(u32CurrentButtonLocation, PORTB);
    }
    

    
    //My First Practical Logical Bit Shift!!! Yay! So Cool! 

     u32CurrentButtonLocation <<= 1; 
     
    }
    
   }

  /* Clear the PIOB pending flag and exit */
  NVIC_ClearPendingIRQ(IRQn_PIOB);
} /* end PIOB_IrqHandler()  */


/*!-------------------------------------------------------------------------------------------------------------------
@fn void SysTick_Handler(void)

@brief  Handler for SysTick timer, which should keep system time acurrately 
 
Requires:
- 

Promises:
- 
*/

void SysTick_Handler(void)
{
  /* Clear the sleep flag */
  G_u32SystemFlags &= ~_SYSTEM_SLEEPING;
  
  /* Update Timers */
  G_u32SystemTime1ms++;
  if( (G_u32SystemTime1ms % 1000) == 0)
  {
    G_u32SystemTime1s++;
  }
} /* end PIOB_IrqHandler()  */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
