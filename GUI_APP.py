import eel
import SerialCOM
import json

#Global variables
RadioStations={
    "Stations":[
        {
            "Name":"RockFM",
            "Frequency":"100.6",
        }
    ],
}

InRegisterList=[0]*5
OutRegisterList=[0]*5
#capture close command
def Close_CB(page_path, sockets_list):
    print(page_path)
    print(f'Other Websockets: {sockets_list}')
    #exit() #exit program execution

eel.init('web_files', allowed_extensions=['.js', '.html'])

@eel.expose
def py_init():
    SerialCOM.check_com_ports()
    eel.update_connection_status(SerialCOM.ser.is_open, SerialCOM.InfoString)
    eel.set_com_list(SerialCOM.com_ports, SerialCOM.ser.is_open)
    py_loadList()

@eel.expose
def py_comListUpdate():
    SerialCOM.check_com_ports()
    eel.set_com_list(SerialCOM.com_ports, SerialCOM.ser.is_open)  

@eel.expose
def getRadioStations():
    #get data from file
    eel.stationsUpdate(RadioStations)
    
@eel.expose
def py_addStation(stationName, stationFreq):
    RadioStations["Stations"].append({
          "Name":stationName,
          "Frequency":str(stationFreq),
        }
      )
    eel.stationsUpdate(RadioStations)

@eel.expose
def py_deleteStation(stationNumber):
    RadioStations["Stations"].pop(stationNumber)
    eel.stationsUpdate(RadioStations)

@eel.expose
def py_updateStation_atIndex(stationNumber, stationName, stationFreq):
    RadioStations["Stations"][stationNumber]["Name"]=stationName
    RadioStations["Stations"][stationNumber]["Frequency"]=str(stationFreq)
    eel.stationsUpdate(RadioStations)

def sortGetName(e):
    return e["Name"]
def sortGetFreq(e):
    return float(e["Frequency"])

@eel.expose
def py_stationListSort(criteria, direction):
    #sort by
    if criteria=="byName":
        if direction == "asc":
            RadioStations["Stations"].sort(reverse=False, key=sortGetName)
        elif direction == "desc":
            RadioStations["Stations"].sort(reverse=True, key=sortGetName)
        else:
            pass
    elif criteria=="byFreq":
        if direction == "asc":
            RadioStations["Stations"].sort(reverse=False, key=sortGetFreq)
        elif direction == "desc":
            RadioStations["Stations"].sort(reverse=True, key=sortGetFreq)
        else:
            pass
    else:
        pass
    eel.stationsUpdate(RadioStations)

@eel.expose
def py_saveList():
    json_obj=json.dumps(RadioStations)
    with open("Stations.json", "w") as outFile:
        outFile.write(json_obj)
    eel.stationsUpdate(RadioStations)

@eel.expose
def py_loadList():
    with open("Stations.json", "r") as inFile:
        global RadioStations
        json_obj=json.load(inFile)
        RadioStations=json_obj
    eel.stationsUpdate(RadioStations)

@eel.expose
def py_getRadioStatus():
  
  if (SerialCOM.ser.is_open):
      #print(f'Getting Radio status bytes')
      if SerialCOM.GetToken():
        CMD_String="G\r"
        if SerialCOM.sendString(CMD_String)==-1:return -1
        [res, received_string]=SerialCOM.receiveString()
        if (res==-1): return -1 
        if received_string[0]=='X':
          print("Data update sequence - Command Error")
        else:
            received_string=received_string[1:] #take out first character
            #convert the content to a byte list
            try:
                data=[int(str_no) for str_no in received_string.split(",")]
                
                InRegisterList[0]=data[0]
                InRegisterList[1]=data[1]
                InRegisterList[2]=data[2]
                InRegisterList[3]=data[3]
                InRegisterList[4]=data[4]

                OutRegisterList[0]=data[5]
                OutRegisterList[1]=data[6]
                OutRegisterList[2]=data[7]
                OutRegisterList[3]=data[8]
                OutRegisterList[4]=data[9]

            except Exception as e:
                print(e)
                print("Get Inst Values - Error while converting received data")
                eel.printAlert("Error while converting received data")
        SerialCOM.ReleaseToken()
  eel.updateRadioStatus(InRegisterList, OutRegisterList)

@eel.expose
def py_toggleSerchMode():
    print(f'Sending Serch Mode Toggle - CMD-10')
    SerialCOM.setCommandReg(10, 1)

@eel.expose
def py_getNextStation():
    print(f'Sending Serch UP - CMD-11')
    SerialCOM.setCommandReg(11, 1)


@eel.expose
def py_getPrevStation():
    print(f'Sending Serch DOWN - CMD-12')
    SerialCOM.setCommandReg(12, 1)


@eel.expose
def py_radioOn():
  print(f'Sending Radio On - CMD-14')
  SerialCOM.setCommandReg(14, 1)

@eel.expose
def py_radioOff():
  print(f'Sending Radio Off - CMD-15')
  SerialCOM.setCommandReg(15, 1) 

@eel.expose
def py_stationPlay(index):
    print(f'Plaing Station {index} - {RadioStations["Stations"][index]["Name"]} - {RadioStations["Stations"][index]["Frequency"]}')
    #write PLL parameter
    PLL=int(4*(float(RadioStations["Stations"][index]["Frequency"])*1_000_000-225_000)/32768)
    print(f'PLL should be {PLL}')

    if (SerialCOM.ser.is_open):
      if SerialCOM.GetToken():
        CMD_String=f'w0,0,{(PLL>>8)&0x3F},{PLL&0xFF}\r'

        if SerialCOM.sendString(CMD_String)==-1:return -1
        [res, received_string]=SerialCOM.receiveString()
        if (res==-1): return -1 
        if received_string[0]=='X':
          print("Data update sequence - Command Error")
        else:
            received_string=received_string[1:] #take out first character
            #convert the content to a byte list
            try:
                pass
            except Exception as e:
                print(e)
                print("Write data error")
                eel.printAlert("Error while converting received data")
        SerialCOM.setCommandReg(13, 1)
        SerialCOM.ReleaseToken()
    else:
        eel.printAlert("Serial connection not opened")

@eel.expose
def py_setSSL(value):
    print(f'Setting SSL to {value}')
    
    if (SerialCOM.ser.is_open):
      if SerialCOM.GetToken():
        CMD_String=f'w0,1,{int(value)&0x03}\r'

        if SerialCOM.sendString(CMD_String)==-1:return -1
        [res, received_string]=SerialCOM.receiveString()
        if (res==-1): return -1 
        if received_string[0]=='X':
          print("Data update sequence - Command Error")
        else:
            received_string=received_string[1:] #take out first character
            #convert the content to a byte list
            try:
                pass
            except Exception as e:
                print(e)
                print("Write data error")
                eel.printAlert("Error while converting received data")
        SerialCOM.ReleaseToken()
    else:
        eel.printAlert("Serial connection not opened")

#print(eel.js_random()())

eel.start('index.html', 
          # mode='edge', 
          host='localhost',
          port='5001',
          size=(550,900),
          cmdline_args=[
              '--start-fullscreen', 
              #'--browser-startup-dialog'
            ],
        #   geometry={
        #       'size': (200, 100), 
        #       'position': (300, 50)
        #     },
          close_callback=Close_CB)
