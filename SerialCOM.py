from cmath import inf
import serial
import serial.tools.list_ports
import eel

com_ports=[] #this will be filled-in by check_com_ports function
ser=serial.Serial() #serial variable that handles serial communication
Token=0 #used to handle concurency over serial connection
newConnection=False #new connection event
InfoString="" #identification info for the board

#before using read and write functions over serial port
#function GetToken has to be called
def GetToken(): 
    global Token
    if Token==0:
        Token=1
        return True
    else:
        return False

#When read/write sequence is done, the token should be released
def ReleaseToken():
    global Token
    Token=0

def check_com_ports():
    global com_ports
    com_ports=[]
    available_ports=serial.tools.list_ports.comports()
    for port, desc, hwid in sorted(available_ports):
        com_ports.append(port)


@eel.expose
def py_com_connect(PortNo):
    print("Attempt to conect to {}".format(PortNo))
    global ser
    global newConnection
    global InfoString
    ser.port=PortNo
    ser.baudrate="115200"
    ser.timeout=5 #five seconds
    ser.rtscts=False
    ser.dsrdtr=False
    connection_error=False

    InfoString="" #used to transfer the 
    try:
        ser.open()
        
        if ser.is_open: 
            CMD_String="i\r"
            print("Port opened")
            ReleaseToken()
            #get the board ID and virmware version
            print("--Get Board Identifier")
            while(not GetToken()): #execution locked in loop until Token is free
                pass
            print("---Send Command")
            if sendString(CMD_String)==-1: return -1

            print("---Waiting response")
            [res, received_string]=receiveString()
            if (res==-1): 
                eel.printAlert("Authentication Error")
                return -1   
            
            if received_string[0]!="X": #no error is reported
                InfoString=received_string[1:]
            else:
                connection_error=True
                InfoString=""
            

            print("--Getting firmware version")
            CMD_String="v\r"
            #get the firmware version
            print("---Sending command")
            if sendString(CMD_String)==-1: return -1
            
            print("---Waiting response")
            [res, received_string]=receiveString()
            if (res==-1): 
                eel.printAlert("Authentication Error")
                return -1  

            if received_string[0]!="X": #no error is reported
                InfoString+=" - v"+received_string[1:]
            else:
                connection_error=True
                InfoString=""
            
            print("---Reset all command flags")
            for index in range(0, 16):
                #print(f"reset command flag {index}")
                if setCommandReg(index, 0) == -1:
                    print("Error resetting command flags")
                    eel.printAlert("Error resetting command flags")
                    py_com_disconnect()
                    return -1

            ReleaseToken()

            #ask for parameters from the connected device
            newConnection=True #new connection event occured
    except Exception as e:
        print("Connection error occurred")
        eel.printAlert(f"Connection Error\n{e}")
        connection_error=True
    if not connection_error: #no error is reported
        eel.update_connection_status(ser.is_open and not connection_error, InfoString)
    else:
        py_com_disconnect()
        return -1

@eel.expose
def py_com_disconnect():
    try:
        ser.close()
        if not ser.is_open:
            print("Serial port successfully disconnected")
            ReleaseToken()
    except:
        print("Error when closing serial port")
    eel.update_connection_status(ser.is_open, "")

################################################################################
##               AUXILIARY COMMUNICATION FUNCTIONS
##
################################################################################


def setCommandReg(bit, value):
    """
    FUNCTION
    ret_val = setCommandReg(cmd_val)

    INPUT PARAMETERS
    cmd_val - 16 bit number representing the value that command register should take

    OUTPUT PARAMETERS
    ret_val - 1 - Writing successfull
            - -1 - Writing error

    DESCRIPTION
    This function sets the command word value inside the microcontroller. 
    """
    if value!=0: value=1
    if bit<0: bit=0
    if bit>31: bit=31
    CMD_String="c{},{}\r".format(bit, value) 
    if (ser.is_open):
        #Send command to uC
        if sendString(CMD_String)==-1:return -1

        [res, received_string]=receiveString()
        if (res==-1): return -1   

        if received_string[0]=="c": #uC responded to c command
            return 1 
        else:
            return -1
    else:
        return -1

def getStatusReg(bit):
    """
    FUNCTION
    ret_val = getStatusReg()

    INPUT PARAMETERS
    bit - bit number

    OUTPUT PARAMETERS
    ret_val - val > 0 - Current value of the status register
            - -1 - Communication Error
            
    DESCRIPTION
    This function gets the status word value from the microcontroller. 
    """
    CMD_String="g\r" 
    if (ser.is_open):
        #Send command to uC
        if sendString(CMD_String)==-1:return -1

        [res, received_string]=receiveString()
        if (res==-1): return -1  

        if received_string[0]!="X": #the message does not contain an error code
            received_string=received_string[1:] #take out first and last character
            status_value=0
            for number in received_string.split(","):
                status_value=(status_value<<8)|int(number)
            return 1 if status_value&(1<<bit) else 0
        else:
            return -1
    else:
        return -1

def sendString(inString):
    """Sends data over Serial connection"""
    try:
        ser.flush()
        ser.write(inString.encode('UTF-8'))
        return 1 #function completed successfully
    except Exception as e:
        print(e)
        print("Serial data write error")
        eel.printAlert("Serial data write error")
        py_com_disconnect()
        return -1 #Error while sending data

def receiveString():
    """Tries to receive a string while a '\r' char is received"""
    received_char=''
    received_string=''
    retry_no=1
    while (received_char!="\r" and retry_no>0):
        try:
            char=b''
            char=ser.read(1)
            received_char=char.decode('UTF-8')
            if ("0"<=received_char<="9") \
                    or ("A"<=received_char<="Z") \
                    or ("a"<=received_char<="z") \
                    or received_char=="," or received_char=="-": 
                received_string+=received_char
            if char==b'': retry_no-=1
        except:
            print("Error while decoding received chars ", end='')
            print(char)
            py_com_disconnect()
            return [-1, ""]
    if retry_no==0:
        py_com_disconnect()
        return [-1, ""]
    else:
        return [1, received_string]