import sys
import serial
import serial.tools.list_ports
import time
import requests
import audioop
import json
import urllib.request
import cv2
import numpy as np
from PIL import Image
from pydub import AudioSegment

arduino_port = '/dev/ttyACM0'
avr_port = 'ttyACM1'
baudrate = 115200
web_cam_url = 'http://192.168.123.36:8080/shot.jpg'

rfid_raw = 0
rfid_id = 0
lcd_text = ""
lcd_change = 0
url_open_error = 0;


def get_text():
    audio_q = b''
    buff_size = 2048
    global rfid_id
    global rfid_raw
    rfid_id_his = rfid_id
    capture_num = 0

    ports = list(serial.tools.list_ports.comports())
    print(ports);

    for p in ports:
        print(p);
        if avr_port in p[1]: 
            com_port = p[0]
            break;
        else: com_port = ''
    try:
        port = serial.Serial(com_port,230400,timeout=None)
    except:
        return "Connection Error: Can't find Arduino serial port"
    millis = int(round(time.time() * 1000))
    while rfid_id != 0:
        rec_bytes = port.read(buff_size)
        audio_q+=rec_bytes
        if ser.in_waiting:
            if capture_num == 0:
                try:
                    imgResp = urllib.request.urlopen(web_cam_url)
                except:
                    print("URLError : Can't open the URL")
                else : 
                    imgNp = np.array(bytearray(imgResp.read()), dtype=np.uint8)
                    img = cv2.imdecode(imgNp, -1)
                    print("Webcam is Ready!")
                    file = "IPimage.jpg"
                    cv2.imwrite(file, img)
                    capture_num = 1
                    files = open("IPimage.jpg","rb")

                    upload = {'file':files}
                    res = requests.post('http://kclee.run-us-west1.goorm.io/image', files = upload)
            rfid_raw = ser.read()
            rfid_id = int.from_bytes(rfid_raw,byteorder="big")
            print("RFID has remoted")
        
    audio_q=audioop.mul(audio_q,1,3)

    sound = AudioSegment(
        data=audio_q,
        sample_width=1,
        frame_rate=20000,
        channels=1
    )
    sound.export("audio.mp3",format="mp3")
    files = open('audio.mp3', 'rb')
    
    upload = {'file':files}
    param = {'user':rfid_id_his}
    res = requests.post('http://kclee.run-us-west1.goorm.io/upload', files = upload, data=param)

    print(res.text)
    return res.text

ser = serial.Serial(arduino_port, baudrate)
while True:
    lcd_change = 0
    if ser.in_waiting:
        rfid_raw = ser.read()
        rfid_id = int.from_bytes(rfid_raw,byteorder="big")
        print(rfid_id)
        if(rfid_id != 0) :
            print("RFID has attached")
            lcd_text = get_text()
            lcd_change = 1

        
    if lcd_change == 1:
        lcd_text_encode = lcd_text.encode()
        print(lcd_text_encode)
        ser.write(lcd_text_encode)
