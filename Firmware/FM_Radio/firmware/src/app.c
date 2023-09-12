/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "peripheral/tmr/plib_tmr4.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


void TMR4_CB(uint32_t status, uintptr_t context);


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    TMR4_CallbackRegister(&TMR4_CB, 0);
    TMR4_Start();

    //Radio parameters initialization
    appData.FM_RADIO.out.Reg1.MUTE=false;
    appData.FM_RADIO.out.Reg1.SM=false;
    appData.FM_RADIO.out.Reg1.PLL_H=42;
    appData.FM_RADIO.out.Reg2.PLL_L=54;
    appData.FM_RADIO.out.Reg3.SUD=true; //search direction
    appData.FM_RADIO.out.Reg3.SSL=1;
    appData.FM_RADIO.out.Reg3.HLSI=false;
    appData.FM_RADIO.out.Reg3.MS=true; //force mono
    appData.FM_RADIO.out.Reg3.MR=false;
    appData.FM_RADIO.out.Reg3.ML=false;
    appData.FM_RADIO.out.Reg3.SWP1=false;
    appData.FM_RADIO.out.Reg4.SWP2=false;
    appData.FM_RADIO.out.Reg4.STBY=false;
    appData.FM_RADIO.out.Reg4.BL=false; //band limit - Europe/US
    appData.FM_RADIO.out.Reg4.XTAL=true;
    appData.FM_RADIO.out.Reg4.SMUTE=false;
    appData.FM_RADIO.out.Reg4.HCC=true;
    appData.FM_RADIO.out.Reg4.SNC=true;
    appData.FM_RADIO.out.Reg4.SI=true;
    appData.FM_RADIO.out.Reg5.PLLREF=false;
    appData.FM_RADIO.out.Reg5.DTC=true;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {

            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
    GUI_FSM();
    
    if(!appData.FM_RADIO.newCommand){
        uint16_t PLL=(appData.FM_RADIO.in.Reg1.PLL_H<<8)|appData.FM_RADIO.in.Reg2.PLL_L;
        if (appData.commands.CMD_toggleSerchMode){
            //Toggle Search Mode
            appData.commands.CMD_toggleSerchMode=0;
            appData.FM_RADIO.out.Reg1.SM=!appData.FM_RADIO.out.Reg1.SM;
        }
        if (appData.commands.CMD_searchUP){
            //Search UP
            appData.commands.CMD_searchUP=0;
            PLL+=12; //0.1 MHz
            appData.FM_RADIO.out.Reg2.PLL_L=PLL&0xFF;
            appData.FM_RADIO.out.Reg1.PLL_H=PLL>>8&0x3F;
            //appData.FM_RADIO.out.Reg1.SM=true;
            appData.FM_RADIO.out.Reg3.SUD=true; //search up
            appData.FM_RADIO.newCommand=true;
        }
        if (appData.commands.CMD_searchDOWN){
            //Search DOWN
            PLL-=12; //0.1 MHz
            appData.commands.CMD_searchDOWN=0;
            appData.FM_RADIO.out.Reg2.PLL_L=PLL&0xFF;
            appData.FM_RADIO.out.Reg1.PLL_H=PLL>>8&0x3F;
            //appData.FM_RADIO.out.Reg1.SM=true;
            appData.FM_RADIO.out.Reg3.SUD=false; //search down
            appData.FM_RADIO.newCommand=true;
        }
        
        if (appData.commands.CMD_radioON){
            //Radio On
            appData.commands.CMD_radioON=0;
            appData.FM_RADIO.out.Reg2.PLL_L=PLL&0xFF;
            appData.FM_RADIO.out.Reg1.PLL_H=PLL>>8&0x3F;
            appData.FM_RADIO.out.Reg1.SM=false;
            appData.FM_RADIO.out.Reg4.SMUTE=false;
            appData.FM_RADIO.out.Reg4.STBY=false;
            appData.FM_RADIO.newCommand=true;
        }
        
        if (appData.commands.CMD_radioOFF){
            //Radio Off
            appData.commands.CMD_radioOFF=0;
            appData.FM_RADIO.out.Reg2.PLL_L=PLL&0xFF;
            appData.FM_RADIO.out.Reg1.PLL_H=PLL>>8&0x3F;
            appData.FM_RADIO.out.Reg4.SMUTE=true;
            appData.FM_RADIO.out.Reg1.SM=false;
            appData.FM_RADIO.out.Reg4.STBY=true;
            appData.FM_RADIO.newCommand=true;
        }
        
        if (appData.commands.CMD_playFrequency){
            //Play the current frequency (previously sent to uC)
            appData.commands.CMD_playFrequency=0;
            appData.FM_RADIO.out.Reg4.SMUTE=false;
            appData.FM_RADIO.out.Reg1.SM=false;
            appData.FM_RADIO.out.Reg4.STBY=false;
            appData.FM_RADIO.newCommand=true;
        }
    }else{
        if (appData.FM_RADIO.commandDone){
            appData.FM_RADIO.newCommand=false;
        }
    }
    TEA5767_FSM(&appData.FM_RADIO);
}

void TMR4_CB(uint32_t status, uintptr_t context){
    if (appData.FM_RADIO.timeoutCounter>0){
        appData.FM_RADIO.timeoutCounter--;
    }
}
/*******************************************************************************
 End of File
 */
