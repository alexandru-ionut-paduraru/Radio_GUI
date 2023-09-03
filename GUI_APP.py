import eel
import serial

#capture close command
def Close_CB(page_path, sockets_list):
    print(page_path)
    print(f'Other Websockets: {sockets_list}')
    #exit() #exit program execution

eel.init('web_files', allowed_extensions=['.js', '.html'])

#print(eel.js_random()())

eel.start('index.html', 
          # mode='edge', 
          host='localhost',
          port='5001',
          size=(500,700),
          cmdline_args=[
              '--start-fullscreen', 
              #'--browser-startup-dialog'
            ],
        #   geometry={
        #       'size': (200, 100), 
        #       'position': (300, 50)
        #     },
          close_callback=Close_CB)
