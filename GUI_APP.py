import eel
import SerialCOM

#Global variables
RadioStations={
    "Stations":[
        {
            "Name":"RockFM",
            "Frequency":"100.6",
        }
    ],
}
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
    return e["Frequency"]

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

#print(eel.js_random()())

eel.start('index.html', 
          # mode='edge', 
          host='localhost',
          port='5001',
          size=(500,900),
          cmdline_args=[
              '--start-fullscreen', 
              #'--browser-startup-dialog'
            ],
        #   geometry={
        #       'size': (200, 100), 
        #       'position': (300, 50)
        #     },
          close_callback=Close_CB)
