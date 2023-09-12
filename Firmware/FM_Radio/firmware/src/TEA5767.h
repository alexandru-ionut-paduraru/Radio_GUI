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

#ifndef _TEA5767_H    /* Guard against multiple inclusion */
#define _TEA5767_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


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


    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */

    typedef union{
        uint8_t reg;
        struct{
            //most significant 6 bits of PLL
            uint8_t PLL_H:6;
            /**
             SM - Search mode:<br>
             1 - search mode active<br>
             0 - search mode inactive
            */
            bool SM:1; 
            /**
             MUTE - Mute:<br>
             1 - L and R are muted<br>
             0 - L and R are not muted
            */
            bool MUTE:1; 
        }; 
    }U_WriteReg_1;
    
    typedef union{
        uint8_t reg;
        struct{
            //LSB Byte of PLL
            uint8_t PLL_L;
        };
    }U_WriteReg_2;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             SM - Search mode:<br>
             1 - search mode active<br>
             0 - search mode inactive
            */
            bool SWP1:1; 
            /**
             MUTE - Left;
            */
            bool ML:1; 
            /**
             MUTE - Right;
            */
            bool MR:1; 
            /**
             Mono to Stereo:<br>
             1 - Forced mono<br>
             0 - Stereo on
            */
            bool MS:1; 
            /**
             HLSI - High/Low Side Injection:<br>
             1 - High Side LO injection<br>
             0 - Low side LO injection
            */
            bool HLSI:1; 
            /**
             SSL- Search Stop Level
            */
            uint8_t SSL:2; 
            /**
             SUD - Search UP/Down:<br>
             1 - Search UP<br>
             0 - Search Down
            */
            bool SUD:1; 
        }; 
    }U_WriteReg_3;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             SI - Search Indicator:<br>
             1 - SWPORT1 is output for the ready flag<br>
             0 - SWPORT1 is software programmable port 1
            */
            bool SI:1; 
            /**
             SNC - Stereo Noise Cancellation<br>
             1 - Stereo noise cancellation is on<br>
             0 - Stereo noise cancellation is off
            */
            bool SNC:1; 
            /**
             HCC - High Cut Control<br>
             1 - High Cat Control is on<br>
             0 - High Cut Control is OFF
            */
            bool HCC:1; 
            /**
             SMUTE - Soft Mute:<br>
             1 - Soft mute ON<br>
             0 - Soft mute OFF
            */
            bool SMUTE:1; 
            /**
             XTAL - Clock frequency
            */
            bool XTAL:1; 
            /**
             BL - Band Limits<br>
             1 - Japanese FM band<br>
             0 - US/Europe FM band
            */
            bool BL:1; 
            /**
             STBY - Standby<br>
             1 - in Standby mode<br>
             0 - not in Standby mode
            */
            bool STBY:1; 
            /**
             SWP2 - Software programmable port 2:<br>
             1 - port 2 is High<br>
             0 - port 2 is Low
            */
            bool SWP2:1; 
        }; 
    }U_WriteReg_4;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             Spare
            */
            uint8_t Spare:6; 
            /**
             DTC<br>
             1 - the de-emphasis time constant is 75 µs<br>
             0 - the de-emphasis time constant is 50 µs
            */
            bool DTC:1; 
            /**
             PLLREF<br>
             1 - the 6.5 MHz reference frequency for the PLL is enabled<br>
             0 - the 6.5 MHz reference frequency for the PLL is disabled
            */
            bool PLLREF:1; 
            
            
        }; 
    }U_WriteReg_5;
    
    typedef union{
        uint8_t data[5];
        struct{
            U_WriteReg_1 Reg1;
            //LSB byte of PLL
            U_WriteReg_2 Reg2;
            U_WriteReg_3 Reg3;
            U_WriteReg_4 Reg4;
            U_WriteReg_5 Reg5;
        };
    }U_WriteRegs;
    
    typedef union{
        uint8_t reg;
        struct{
            //most significant 6 bits of PLL
            uint8_t PLL_H:6;
            /**
             BLF - Band Limit Flag:<br>
             1 - Band Limit has been reached<br>
             0 - Band Limit has not been reached
            */
            bool BLF:1; 
            /**
             RF - Ready Flag:<br>
             1 - Station has been found / band limit reached<br>
             0 - no station has been found
            */
            bool RF:1; 
        }; 
    }U_ReadReg_1;
    
    typedef union{
        uint8_t reg;
        struct{
            //LSB Byte of PLL
            uint8_t PLL_L;
        };
    }U_ReadReg_2;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             IF - IF counter result
            */
            uint8_t IF:7; 
            /**
             STEREO - Stereo Indication<br>
             1 - Stereo Reception<br>
             0 - Mono Reception
            */
            bool STEREO:1; 
        }; 
    }U_ReadReg_3;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             Intentionally left 0
            */
            bool Spare:1; 
            /**
             CI - Chip Identification: these bits have to be set to logic 0
            */
            uint8_t CI:3; 
            /**
             LEV - Level ADC output
            */
            uint8_t LEV:4; 
        }; 
    }U_ReadReg_4;
    
    typedef union{
        uint8_t reg;
        struct{
            /**
             Spare
            */
            uint8_t Spare:8;
        }; 
    }U_ReadReg_5;
    
    typedef union{
        uint8_t data[5];
        struct{
            U_ReadReg_1 Reg1;
            //LSB byte of PLL
            U_ReadReg_2 Reg2;
            U_ReadReg_3 Reg3;
            U_ReadReg_4 Reg4;
            U_ReadReg_5 Reg5;
        };
    }U_ReadRegs;
    
    typedef struct{
        U_WriteRegs out;
        U_ReadRegs in;
        uint8_t state;
        //used outside the FSM to start the sequence
        bool newCommand;
        //previous value for new command variable
        bool newCommand_prev; 
        bool bussy;
        bool commandDone;
        uint16_t timeoutCounter;
    }S_TEA5767;
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



    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */

    // *****************************************************************************
    /**
      @Function
        int ExampleFunctionName ( int param1, int param2 ) 

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
 
    void TEA5767_FSM(S_TEA5767 *Radio);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
