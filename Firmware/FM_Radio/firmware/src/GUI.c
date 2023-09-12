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
#include "GUI.h"
#include <string.h>
#include <stdio.h>
#include "app.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


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

GUI_Data gui;
extern APP_DATA appData;
/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

void GUI_FSM(void){
    //char readChar;
    //static bool readRequested=false;
    static char strToSend[1024];
    static char receivedString[1024];
    static uint16_t recLength=0; //length of the received string
    uint16_t index=0;
    char strAux[50];
    
    switch(gui.GUI_State){
        case GUI_INIT:
            //readRequested=false;
            gui.GUI_State=GUI_READ_CHAR;
            recLength=0;
            break;
        case GUI_READ_CHAR:
            UART6_Read(gui.RxBuffer, 1);
            gui.GUI_State=GUI_WAIT_CHAR;
            break;
        case GUI_WAIT_CHAR:
            if (UART6_ReadIsBusy()){
                //the FSM will remain in this state
            }else{
                if (UART6_ReadCountGet()>0){
                    //there is a new char to add the local incoming buffer
                    receivedString[recLength]=gui.RxBuffer[0]; //only one char is read at a time
                    recLength++;
                    if (gui.RxBuffer[0]=='\r'){
                        //buffer end
                        gui.GUI_State=GUI_CALL_PROCESS_FCN;
                    }else{
                        gui.GUI_State=GUI_READ_CHAR;
                    }
                }else{
                    gui.GUI_State=GUI_INIT;
                }
            }
            break;
            
//        case GUI_READ_CHAR:
//            if (!readRequested){
//                UART6_Read(gui.RxBuffer, 1);
//                readRequested=true;
//            }else{
//                if (UART6_ReadIsBusy()){
//                    gui.GUI_State=GUI_WAIT_READ_DONE;
//                    readRequested=false;
//                }else{
//                    if (UART6_ReadCountGet()>0){
//                        gui.GUI_State=GUI_GET_CHAR;
//                    }else{
//                        gui.GUI_State=GUI_INIT;
//                    }
//                }
//            }
//            break;
//            
//        case GUI_WAIT_READ_DONE:
//            if (!UART6_ReadIsBusy()){
//                if (UART6_ReadCountGet()>0){
//                    gui.GUI_State=GUI_GET_CHAR;
//                }else{
//                    gui.GUI_State=GUI_INIT;
//                }
//            }
//            break;
//            
//        case GUI_GET_CHAR:
//            gui.GUI_State=GUI_CALL_PROCESS_FCN;
//            break;
            
        case GUI_CALL_PROCESS_FCN:
            //gui_cmd_process(gui.RxBuffer[0]);
            for (index=0; index<recLength; index++){
                gui_cmd_process(receivedString[index]);
            }
            if (gui.newCommandReady){
                if(!gui.commandError){
                    switch(gui.crtCommand){
                        case 'i':
                            UART6_Write("iFM_RADIO\r", 10);
                            break;
                        case 'v':
                            UART6_Write("v00.1\r", 6);
                            break;
                        case 'g': //read status register - command
                            //update some of the bits
                            //appData.status.STAT_ADC_ScanDone=appData.ADC_scanCycleReady;
                            //get the value of the corresponding register
                            strcpy(strToSend, "g");
                            sprintf(strAux, "%u,", appData.status.STAT_Byte[1]);
                            strcat(strToSend, strAux);
                            sprintf(strAux, "%u\r", appData.status.STAT_Byte[0]);
                            strcat(strToSend, strAux);
                            UART6_Write(strToSend, strlen(strToSend));
                            break;
                        case 'c': //write command register -command
                            if (gui.paramsNo==2){
                                uint8_t CMD_bits = gui.paramsList[0];
                                if (CMD_bits<16){
                                    uint8_t CMD_byte=CMD_bits/8;
                                    uint8_t CMD_byte_bit =  CMD_bits%8;
                                    appData.commands.CMD_Byte[CMD_byte]=
                                            (appData.commands.CMD_Byte[CMD_byte]&~(1<<CMD_byte_bit))
                                            |((gui.paramsList[1]>0)<<CMD_byte_bit);
                                }
                                                                //send back the response
                                strcpy(strToSend, "c");
                                sprintf(strAux, "%u,", appData.commands.CMD_Byte[1]);
                                strcat(strToSend, strAux);
                                sprintf(strAux, "%u\r", appData.commands.CMD_Byte[0]);
                                strcat(strToSend, strAux);
                                UART6_Write(strToSend, strlen(strToSend));
                            }else{
                                //too many parameters. The case with missing 
                                //parameters is treated by the cmd process function
                                gui.CMD_ErrorClass=1; //command error
                                gui.CMD_ErrorNo=4; //too many parameters
                                gui.commandError=true;
                            }
                            break;
                        case 'r': //get register value
                            /*this command uses a table index 
                             * and a register index
                            */
                            if (gui.paramsNo==2){
                                uint8_t tableIndex=gui.paramsList[0];
                                uint8_t registerIndex=gui.paramsList[1];
                                //send back the response
                                strcpy(strToSend, "r");
                                sprintf(strAux, "%u,", tableIndex);
                                strcat(strToSend, strAux);
                                sprintf(strAux, "%u,", registerIndex);
                                strcat(strToSend, strAux);
                                switch (tableIndex){
                                    case 0: //Application related registers
                                        switch (registerIndex){
                                            case 0: //ADC Scan mask
//                                                strcat(strToSend, "2,");
//                                                sprintf(strAux, "%u,", (appData.ADC_ScanMask>>8)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u\r", appData.ADC_ScanMask&0xff);
//                                                strcat(strToSend, strAux);
//                                                break;
                                            default:
                                                gui.CMD_ErrorClass=2; //processing error
                                                gui.CMD_ErrorNo=2; //register does not exist
                                                gui.commandError=true;
                                                break;  
                                        }
                                        break;
                                    case 1: //ADC related registers
                                        switch (registerIndex){
                                            case 0: //prescalar
//                                                strcat(strToSend, "1,");
//                                                sprintf(strAux, "%u\r", ADC1.Params.PSC);
//                                                strcat(strToSend, strAux);
                                                break;
                                            case 11: //Channels to scan
//                                                strcat(strToSend, "2,");
//                                                sprintf(strAux, "%u,", (ADC1.Params.ChannelsToScan>>8)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u\r", ADC1.Params.ChannelsToScan&0xff);
//                                                strcat(strToSend, strAux);
                                                break;
                                            case 12: //Offset Value
//                                                strcat(strToSend, "4,");
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>24)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>16)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>8)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u\r", ADC1.Params.OffsetValue&0xff);
//                                                strcat(strToSend, strAux);
                                                break;
                                            default:
                                                gui.CMD_ErrorClass=2; //processing error
                                                gui.CMD_ErrorNo=2; //register does not exist
                                                gui.commandError=true;
                                                break;
                                        }
                                        break;
                                    default:
                                        gui.CMD_ErrorClass=2; //processing error
                                        gui.CMD_ErrorNo=1; //table does not exist
                                        gui.commandError=true;
                                        break;
                                }
                                if (!gui.commandError){
                                    UART6_Write(strToSend, strlen(strToSend));
                                }
                            }else{
                                //too many parameters. The case with missing 
                                //parameters is treated by the cmd process function
                                gui.CMD_ErrorClass=1; //command error
                                gui.CMD_ErrorNo=4; //too many parameters
                                gui.commandError=true;
                            }  
                            break;
                        case 'w': //sets register value
                            /*this command uses a table index 
                             * and a register index
                            */
                            if (gui.paramsNo==2 || true){
                                uint8_t tableIndex=gui.paramsList[0];
                                uint8_t registerIndex=gui.paramsList[1];
                                //send back the response
                                strcpy(strToSend, "w");
                                sprintf(strAux, "%u,", tableIndex);
                                strcat(strToSend, strAux);
                                sprintf(strAux, "%u,", registerIndex);
                                strcat(strToSend, strAux);
                                switch (tableIndex){
                                    case 0: //Application related registers
                                        switch (registerIndex){
                                            case 0: //PLL to write
                                                appData.FM_RADIO.out.Reg1.PLL_H=gui.paramsList[2]&0x3F;
                                                appData.FM_RADIO.out.Reg2.PLL_L=gui.paramsList[3];
                                                
                                                strcat(strToSend, "2,");
                                                sprintf(strAux, "%u,", appData.FM_RADIO.out.Reg1.PLL_H);
                                                strcat(strToSend, strAux);
                                                sprintf(strAux, "%u\r", appData.FM_RADIO.out.Reg2.PLL_L);
                                                strcat(strToSend, strAux);
                                                break;
                                            case 1: //SSL Search Stop Level
                                                appData.FM_RADIO.out.Reg3.SSL=gui.paramsList[2]&0x03;
                                                strcat(strToSend, "1,");
                                                sprintf(strAux, "%u\r", appData.FM_RADIO.out.Reg3.SSL);
                                                strcat(strToSend, strAux);
                                                break;
                                            default:
                                                gui.CMD_ErrorClass=2; //processing error
                                                gui.CMD_ErrorNo=2; //register does not exist
                                                gui.commandError=true;
                                                break;  
                                        }
                                        break;
                                    case 1: //ADC related registers
                                        switch (registerIndex){
                                            case 0: //prescalar
//                                                ADC1.Params.PSC=(uint8_t)(gui.paramsList[2]>=1);
//                                                strcat(strToSend, "1,");
//                                                sprintf(strAux, "%u\r", ADC1.Params.PSC);
//                                                strcat(strToSend, strAux);
//                                                break;
                                            
                                            case 12: //Offset Value
//                                                ADC1.Params.OffsetValue=(gui.paramsList[2]<<24)
//                                                        +(gui.paramsList[3]<<16)
//                                                        +(gui.paramsList[4]<<8)
//                                                        +gui.paramsList[5];
//                                                strcat(strToSend, "4,");
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>24)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>16)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u,", (ADC1.Params.OffsetValue>>8)&0xff);
//                                                strcat(strToSend, strAux);
//                                                sprintf(strAux, "%u\r", ADC1.Params.OffsetValue&0xff);
//                                                strcat(strToSend, strAux);
                                                break;
                                            default:
                                                gui.CMD_ErrorClass=2; //processing error
                                                gui.CMD_ErrorNo=2; //register does not exist
                                                gui.commandError=true;
                                                break;
                                        }
                                        break;
                                    default:
                                        gui.CMD_ErrorClass=2; //processing error
                                        gui.CMD_ErrorNo=1; //table does not exist
                                        gui.commandError=true;
                                        break;
                                }
                                if (!gui.commandError){
                                    UART6_Write(strToSend, strlen(strToSend));
                                }
                            }else{
                                //too many parameters. The case with missing 
                                //parameters is treated by the cmd process function
                                gui.CMD_ErrorClass=1; //command error
                                gui.CMD_ErrorNo=4; //too many parameters
                                gui.commandError=true;
                            }  
                            break;
                        case 'G': //get current values of all ADC channels
                            if (true){
                                //send back the response
                                strcpy(strToSend, "G");
                                sprintf(strAux, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u", 
                                        appData.FM_RADIO.in.data[0],
                                        appData.FM_RADIO.in.data[1],
                                        appData.FM_RADIO.in.data[2],
                                        appData.FM_RADIO.in.data[3],
                                        appData.FM_RADIO.in.data[4],
                                        appData.FM_RADIO.out.data[0],
                                        appData.FM_RADIO.out.data[1],
                                        appData.FM_RADIO.out.data[2],
                                        appData.FM_RADIO.out.data[3],
                                        appData.FM_RADIO.out.data[4]);
                                strcat(strToSend, strAux);
                                sprintf(strAux, "\r");
                                strcat(strToSend, strAux);
                                UART6_Write(strToSend, strlen(strToSend));
                            }else{
                                //too many parameters. The case with missing 
                                //parameters is treated by the cmd process function
                                gui.CMD_ErrorClass=1; //command error
                                gui.CMD_ErrorNo=4; //too many parameters
                                gui.commandError=true;
                            }
                            break;
                        default:
                            //not implemented
                            break;
                    }
                    if (gui.commandError){
                        gui.GUI_State=GUI_REPORT_ERROR;
                    }else{
                        gui.GUI_State=GUI_INIT;
                    }
                }else{
                    gui.GUI_State=GUI_REPORT_ERROR;
                }
            }else{
                gui.GUI_State=GUI_INIT;
            }
            
            break;
        case GUI_REPORT_ERROR:
            strcpy(strToSend, "X-");
            sprintf(strAux, "%u-", gui.CMD_ErrorClass);
            strcat(strToSend, strAux);
            sprintf(strAux, "%u\r", gui.CMD_ErrorNo);
            strcat(strToSend, strAux);
            UART6_Write(strToSend, strlen(strToSend));
            
            gui.GUI_State=GUI_INIT;
            break;
        default:
            gui.GUI_State=GUI_INIT;
            break;
    }
    
    /*Other state machines*/
    
}

uint16_t gui_cmd_process(char in_c){
    uint16_t returnVal=0;
    static uint8_t crtParamValue=0;
    static bool formingNewParam=false;
    switch (gui.GUI_CMD_State){
//        case PROCESS_INIT:
//            gui.GUI_CMD_State=PROCESS_CMD_CHAR;
//            gui.CMD_ErrorClass=0;
//            gui.CMD_ErrorNo=0;
//            gui.crtCommand=0; //no command
//            gui.newCommandReady=false;
//            gui.newCommandStarted=false;
//            break;
        case PROCESS_CMD_CHAR:
            gui.CMD_ErrorClass=0;
            gui.CMD_ErrorNo=0;
            gui.crtCommand=0; //no command
            gui.newCommandReady=false;
            gui.newCommandStarted=false;
            gui.commandError=false;
            gui.paramsNo=0;
            crtParamValue=0;
            formingNewParam=false;
            switch (in_c){
                case 'i':
                    gui.crtCommand='i';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_WAIT_END_CHAR;
                    break;
                case 'v':
                    gui.crtCommand='v';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_WAIT_END_CHAR;
                    break;
                case 'g':
                    gui.crtCommand='g';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_WAIT_END_CHAR;
                    break;
                case 'c': //write command register
                    gui.crtCommand='c';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_GET_PARAMS;
                    break;
                case 'r': //write command register
                    gui.crtCommand='r';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_GET_PARAMS;
                    break;
                case 'w': //write command register
                    gui.crtCommand='w';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_GET_PARAMS;
                    break;
                case 'G': //Get ADC channels
                    gui.crtCommand='G';
                    gui.newCommandStarted=true;
                    gui.GUI_CMD_State=PROCESS_WAIT_END_CHAR;
                    break;
                default:
                    gui.CMD_ErrorClass=1; //command error
                    gui.CMD_ErrorNo=1; //unimplemented command
                    gui.commandError=true;
                    gui.GUI_CMD_State=PROCESS_ERR_WAIT_END_CHAR;
                    break;
            }
            break;
        case PROCESS_GET_PARAMS: //form the parameters list of the current command
            //check if the current char is number (0<=in_c<=9)
            if ((in_c>='0')&&(in_c<='9')){
                formingNewParam=true;
                if (((crtParamValue*10)<=240)||
                        ((crtParamValue*10==250)&&(in_c<='5'))){
                    crtParamValue=crtParamValue*10+(uint8_t)(in_c-'0');
                }else{
                    //parameter value out of range
                    gui.CMD_ErrorClass=1; //command error
                    gui.CMD_ErrorNo=5; //parameter out of range
                    gui.commandError=true;
                    gui.GUI_CMD_State=PROCESS_ERR_WAIT_END_CHAR;
                }
            }else{
                if (in_c==','){
                    if (formingNewParam){
                        gui.paramsList[gui.paramsNo] = crtParamValue;
                        crtParamValue=0;
                        formingNewParam=false;
                        gui.paramsNo++;
                        if (gui.paramsNo>=100){
                            // too many parameters in the list
                            gui.CMD_ErrorClass=1; //command error
                            gui.CMD_ErrorNo=4; //unimplemented command
                            gui.commandError=true;
                            gui.GUI_CMD_State=PROCESS_ERR_WAIT_END_CHAR;
                        }
                    }else{
                        //missing parameter
                        gui.CMD_ErrorClass=1; //command error
                        gui.CMD_ErrorNo=6; //missing parameter
                        gui.commandError=true;
                        gui.GUI_CMD_State=PROCESS_ERR_WAIT_END_CHAR;
                    }
                }else{
                    if (in_c=='\r'){
                        if (formingNewParam){
                            gui.paramsList[gui.paramsNo] = crtParamValue;
                            gui.paramsNo++;
                            //end of transmission
                            gui.newCommandReady=true;
                            gui.newCommandStarted=false;
                            formingNewParam=false;
                            gui.GUI_CMD_State=PROCESS_CMD_CHAR;
                        }else{
                            //parameter missing
                            gui.CMD_ErrorClass=1; //command error
                            gui.CMD_ErrorNo=6; //missing parameter
                            gui.commandError=true;
                            gui.newCommandReady=true;
                            gui.newCommandStarted=false;
                            gui.GUI_CMD_State=PROCESS_CMD_CHAR;
                        }
                    }else{
                       // command format error
                        gui.CMD_ErrorClass=1; //command error
                        gui.CMD_ErrorNo=2; //unimplemented command
                        gui.commandError=true;
                        gui.GUI_CMD_State=PROCESS_ERR_WAIT_END_CHAR;
                    }   
                }
            }
            break;
        case PROCESS_GET_REG:
            gui.GUI_CMD_State=PROCESS_CMD_CHAR;
            break;
        case PROCESS_WAIT_END_CHAR:
            if (gui.newCommandStarted && in_c=='\r'){
                gui.newCommandReady=true;
                gui.newCommandStarted=false;
                gui.GUI_CMD_State=PROCESS_CMD_CHAR;
            }
            
        break;
        case PROCESS_ERR_WAIT_END_CHAR:
            if (in_c=='\r'){
                gui.newCommandReady=true;
                gui.newCommandStarted=false;
                gui.commandError=true;
                gui.GUI_CMD_State=PROCESS_CMD_CHAR;
            }
            break;
        default:
            gui.GUI_CMD_State=PROCESS_CMD_CHAR;
            break;
    }
    if (gui.CMD_ErrorClass!=0 || gui.CMD_ErrorNo!=0){
        returnVal=1; //there is an error.
    }
    return returnVal;
}



/* *****************************************************************************
 End of File
 */
