/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */


#ifndef _GUI_H    /* Guard against multiple inclusion */
#define _GUI_H

//#include "app.h"
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
//#include "configuration.h"
//#include "definitions.h"
#include "peripheral/uart/plib_uart6.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    /* ************************************************************************** */
    /** Descriptive Constant Name

      @Summary
        Brief one-line summary of the constant.
    
      @Description
        Full description, explaining the purpose and usage of the constant.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
      @Remarks
        Any additional remarks
     */
#define EXAMPLE_CONSTANT 0


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/*  A brief description of a section can be given directly below the section
    banner.
 */


// *****************************************************************************

/** Descriptive Data Type Name

  @Summary
    Brief one-line summary of the data type.

  @Description
    Full description, explaining the purpose and usage of the data type.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Remarks
    Any additional remarks
    <p>
    Describe enumeration elements and structure and union members above each 
    element or member.
 */
typedef enum {
    GUI_INIT,
            GUI_READ_CHAR,
            GUI_WAIT_READ_DONE,
            GUI_GET_CHAR,
            GUI_CALL_PROCESS_FCN,
            GUI_REPORT_ERROR,
            GUI_WAIT_CHAR,

}GUI_FSM_State;

typedef enum{
    //PROCESS_INIT,
            PROCESS_CMD_CHAR,
            PROCESS_WAIT_END_CHAR,
            PROCESS_GET_PARAMS,
            PROCESS_ERR_WAIT_END_CHAR,
            PROCESS_GET_REG,

}GUI_CMD_Process_State;

typedef struct{
    GUI_FSM_State GUI_State;
    GUI_CMD_Process_State GUI_CMD_State;
    uint8_t CMD_ErrorClass;
    uint8_t CMD_ErrorNo; 
    char crtCommand;
    bool newCommandStarted;
    bool newCommandReady;
    bool commandError;
    uint8_t paramsList[100];
    uint8_t paramsNo;
    char RxBuffer[1024];
    char TxBuffer[1024];
    
    uint8_t uiGetADCParams_State;
    uint8_t uiSetADCParams_State;
    uint8_t uiGetDigipotParams_State;
    uint8_t uiSetDigipotParams_State;
}GUI_Data;

extern GUI_Data gui;

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/*
  @Function
    void GUI_SFM(void); 

  @Summary
  Implements the Finite-State Machine for GUI interaction

  @Description
    This function reads one character at a time and calls the corresponding 
    * processing function
    <p>
    After result from the processing function, it will wait for another character.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    This function should be called at every MCU program cycle.

  @Parameters
    @param NoParameter No parameter is needed for this function

  @Returns
    List of the returned parameters
    <ul>
      <li>void   No parameter returned
    </ul>

  @Remarks
    No remarks available.

  @Example
    @code
        void main(void){
        *      while(true){
        *          System_Task();
        *          GUI_SFM();
        *      }
        * }
 */
void GUI_FSM(void);

uint16_t gui_cmd_process(char in_c);
/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GUI_H */

/* *****************************************************************************
 End of File
 */
