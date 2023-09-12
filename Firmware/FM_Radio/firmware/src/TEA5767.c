/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

#include "TEA5767.h"
#include"config/default/peripheral/i2c/master/plib_i2c1_master.h"
#include "app.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

/** 
  @Function
    int ExampleLocalFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Description
    Full description, explaining the purpose and usage of the function.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    List and describe any required preconditions. If there are no preconditions,
    enter "None."

  @Parameters
    @param param1 Describe the first parameter to the function.
    
    @param param2 Describe the second parameter to the function.

  @Returns
    List (if feasible) and describe the return values of the function.
    <ul>
      <li>1   Indicates an error occurred
      <li>0   Indicates an error did not occur
    </ul>

  @Remarks
    Describe any special behavior not described above.
    <p>
    Any additional remarks.

  @Example
    @code
    if(ExampleFunctionName(1, 2) == 0)
    {
        return 3;
    }
 */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

void TEA5767_FSM(S_TEA5767 *Radio){
    if (Radio->newCommand && !Radio->newCommand_prev){
        //Rising edge of newCommand flag
        Radio->state=0; //reinitialize the state machine
    }
    switch (Radio->state){
        case 0: //init state
            Radio->commandDone=false;
            Radio->bussy=false;
            Radio->state=10;
            break;
        case 10: //wait for new command
            if (Radio->newCommand){
                Radio->bussy=true;
                Radio->state=20;
            }
            break;
        case 20: //write data to the radio module
            if (!I2C1_IsBusy()){
                I2C1_Write(0x60, Radio->out.data, 5);
                Radio->timeoutCounter=200; //200ms
                Radio->state=30;
            }
            break;
        case 30: //Wait timeoutCounter to become 0
            if (Radio->timeoutCounter==0){
                Radio->state=40;
            }
            break;
        case 40: //Read Status registers
            if (!I2C1_IsBusy()){
                I2C1_Read(0x60, Radio->in.data, 5);
                Radio->state=50;
            }
            break;
        case 50: //set command done flag and wait for new command to be cleared
            Radio->commandDone=true;
            if (Radio->newCommand==false){
                Radio->bussy=false;
                Radio->commandDone=false;
                Radio->state=100;
            }
            break;
        case 100: //check Radio Signal Status
            if (!I2C1_IsBusy()){
                if(!Radio->in.Reg1.RF){
                    //Ready Flag (RF) is OFF. Read data until the RF becomes ON
                    Radio->state=110;
                }else{
                    Radio->state=0; //Ready flag is ON
                }
            }
            break;
        case 110: //read data from FM Radio Module
            if (!I2C1_IsBusy()){
                I2C1_Read(0x60, Radio->in.data, 5);
                Radio->timeoutCounter=500; //.5 sec
                Radio->state=120;
            }
            break;
        case 120:
            if (Radio->timeoutCounter==0){
                Radio->state=100; //Check RF flag again.
            }
            break;
        default:
            Radio->state=0;
    }
    Radio->newCommand_prev=Radio->newCommand;
}


/* *****************************************************************************
 End of File
 */
