# -*- coding: GB2312 -*-
from ast import Str
import requests
import json
import time

class weatherreq(object):
    def __init__(self):
        self.city = ""
        self.weather = ""
        self.temp = ""
        self.time = ""
        
    def MatchWeather(self,weather):
        if int(weather) in range(0,4):
            weather = 's'
        elif int(weather) in range(4,9):
            weather = 'c'
        elif int(weather) == 9:
            weather = 'O'
        elif int(weather) in range(10,20):
            weather = 'R'
        elif int(weather) in range(26,30):
            weather = 'D'
        elif int(weather) ==30:
            weather = 'F'
        elif int(weather) ==31:
            weather = 'h'
        elif int(weather) in range(32,37):
            weather = 'W'
        elif int(weather) ==37:
            weather = 'C'
        elif int(weather) ==38:
            weather = 'H'
        return weather

    def fetchWeather(self):
        API="https://api.seniverse.com/v3/weather/now.json"
        result = requests.get(API, params={
            'key': 'SNeWQC8vEqNmNRmG0',
            'location': 'ip',
            'language': 'en',
            'unit': 'c'
        }, timeout=1)
        data = json.loads(result.text)
        return data

    def ret_weather(self):
        result = self.fetchWeather()
        self.city=result['results'][0]['location']['name']#输出查询到的城市名
        self.weather=self.MatchWeather(result['results'][0]['now']['code'])
        self.temp=result['results'][0]['now']['temperature']
        self.time=result['results'][0]['last_update']
        today=time.strftime("%w")
        ret = self.city +self.weather+self.temp+today
        
        return ret
    def ret_time(self):
        return time.ctime()
#wea = weatherreq()
#for i in range(0,50):
#    print(wea.fetchWeather())
#    time.sleep(1)
#tim = time.ctime().replace(" ","_")
#print(tim)

