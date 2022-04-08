#!/usr/bin/python3

import sys
import os
import socket
import requests
import threading
import time
import json

with open("coords") as f:
    weatherData = json.load(f)



def getWeather(lat, lng):
        response = requests.post(f"https://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lng}&appid={os.environ['WEATHER_KEY']}")
        if response.status_code != 200:
            return "unable to get weather!"
        tmp = []
        tmp.append(f"{response.json()['main']['temp'] - 272.15:.2f}°C")
        tmp.append(f"feels {response.json()['main']['feels_like'] - 272.15:.2f}°C")
        tmp.append(f"min {response.json()['main']['temp_min'] - 272.15:.2f}°C")
        tmp.append(f"max {response.json()['main']['temp_max'] - 272.15:.2f}°C")
        tmp.append(f"{response.json()['name']}")
        return tmp

def getWeaterWithCoord(lat, lng, user, scope, isPrivate):
    weather = getWeather(lat, lng)
    if weather == "unable to get weather!":
        sock.send(f"PRIVMSG {scope} :{user}: {weather}\r\n".encode())
    elif isPrivate:
        sock.send(f"PRIVMSG {scope} :{user}: {weather[4]}: {weather[0]}, {weather[1]}, {weather[2]}, {weather[3]}\r\n".encode())
    else:
        sock.send(f"PRIVMSG #Walou :{user}: {weather[4]}: {weather[0]}, {weather[1]}, {weather[2]}, {weather[3]}\r\n".encode())

def getCoordsData(postalCode, user, scope, isPrivate):
    response = []
    if postalCode in weatherData:
        for i in range(len(weatherData[postalCode])):
            data = weatherData[postalCode][i]
            rep_tmp = ""
            rep_tmp += data["Commune"] + ": "
            temp = getWeather(data['Latitude'], data['Longitude'])
            if temp == "unable to get weather!":
                rep_tmp += temp
            else:
                rep_tmp += f"{temp[0]}, {temp[1]}, {temp[2]}, {temp[3]}!"
            response.append(rep_tmp)
    else:
        response.append("Postal code not found")
    for i in range(len(response)):
        print(f"send>>> PRIVMSG {scope} :{user}: {response[i]}\r\n".encode())
        sock.send(f"PRIVMSG {scope} :{user}: {response[i]}\r\n".encode())

r = requests.post('https://api.intra.42.fr/oauth/token',
            data={'grant_type':     'client_credentials',
                  'client_id':      os.environ['CLIENT_ID'],
                  'client_secret':  os.environ['CLIENT_SECRET']})

access_token = r.json()['access_token']

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 6667))

def Bot():
    sock.send("NICK Walou\r\n".encode())
    sock.send("USER Walou 0 * :Walou\r\n".encode())
    sock.send("JOIN #Walou\r\n".encode())
    sock.send("PRIVMSG #Walou :Hello\r\n".encode())
    
    def getPos(usr):
        r = requests.get('https://api.intra.42.fr/v2/users/' + usr + '/locations', headers={'Authorization': 'Bearer ' + access_token})
        if len(r.json()) == 0:
            pos = " don't exits"
        else:
            pos = r.json()[0]['user']['location']
        return pos if pos != None else 'is unavailable'
    
    while True:
        line = sock.recv(1024).decode()
        if not line:
            break
        if line.find("\r\n") == -1:
            line = line + sock.recv(1024).decode()
        cmds = line.split()

        if cmds[0] == "PING":
            sock.send("PONG\r\n".encode())
            cmds = cmds[2:]
            if len(cmds) == 0:
                continue
        if len(cmds) > 4 and cmds[1] == "PRIVMSG":
            if cmds[3][0] == '!':
                user=cmds[0][1:].partition("!")[0]
                if cmds[3][1:] == "getpos":
                    pos = getPos(cmds[4])
                    print(f"user: {user} [{pos}]")
                    if cmds[2][:1] == '#':
                        sock.send(f"PRIVMSG #Walou :{user}: {cmds[4]} {pos}\r\n".encode())
                    else:
                        sock.send(f"PRIVMSG {user} :{user}: {cmds[4]} {pos}\r\n".encode())
                elif cmds[3][1:] == "weather":
                    isPrivate = False 
                    if not cmds[2][:1] == '#':
                        isPrivate = True 
                    scope = user if isPrivate else "#Walou"
                    if len(cmds) == 5:
                        print("postal code: " + cmds[4] + f" isdigit? {str(cmds[4]).isdigit()}")
                        print(f"user: {user}")
                        if str(cmds[4]).isdigit():
                            threading.Thread(target=getCoordsData, args=(cmds[4],user,scope,isPrivate)).start()
                        else:
                            sock.send(f"PRIVMSG {scope} :{user}: bad postal code\r\n".encode())
                    elif len(cmds) == 6:
                        if not cmds[4].replace('.','',1).isdigit():
                            sock.send(f"PRIVMSG {scope} :{user}: bad latitude\r\n".encode())
                        if not cmds[5].replace('.','',1).isdigit():
                            sock.send(f"PRIVMSG {scope} :{user}: bad longitude\r\n".encode())
                        else:
                            threading.Thread(target=getWeaterWithCoord, args=(cmds[4],cmds[5],user,scope,isPrivate)).start()
        print(line)
    

threading.Thread(target=Bot).start()

while True:
    line = sys.stdin.readline()
    if line == "exit\n":
       sock.send("QUIT\r\n".encode())
       time.sleep(1)
       sock.close()
       exit()
