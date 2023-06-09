# -*- coding: GB2312 -*-
from socket import *
import threading
import weather_req
address="192.168.43.197"   
port=8266         
buffsize=1024        
s=socket(AF_INET, SOCK_STREAM)
s.connect((address,port))
#23Chongqing,CloudyTemp:27
req = weather_req.weatherreq()

def conn_socket(address,port):
    s=socket(AF_INET, SOCK_STREAM)
    s.connect((address,port))
    return s


def recieve_data():
    while True:
        recvdata=s.recv(buffsize).decode('utf-8')
        print(recvdata)
        if recvdata == "weather":
            send = req.ret_weather()
            print(send)
            s.send(send.encode())
        #if recvdata == "time":
        #    send = req.ret_time()
        #    s.send(send.encode())
        

t = threading.Thread(target=recieve_data)  
t.start()
        

def send_data(s):
    senddata=input('\n send：')
    #if senddata=='exit':
    #   break
    s.send(senddata.encode())
    #recvdata=s.recv(buffsize).decode('utf-8')
    #print(recvdata)

def close_socket(s):
    s.close()

while True:
    send_data(s)
