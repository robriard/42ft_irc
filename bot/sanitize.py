#!/usr/bin/python3

import json
import sys
import os

def read_coords(file_name):
    with open(file_name) as f:
        return json.load(f)

def get_coords_raw():
    res = read_coords("coords_raw")
    dic = {}
    for i in range(len(res)):
        gps = res[i]["coordonnees_gps"].partition(", ")
        if res[i]["Code_postal"] in dic:
            found = False 
            for j in dic[res[i]["Code_postal"]]:
                if j["Commune"] == res[i]["Nom_commune"]:
                    found = True
                    break
            if not found:
                dic[res[i]["Code_postal"]].append({"Commune": res[i]["Nom_commune"], "Latitude": gps[0], "Longitude": gps[2]})
        else:
            dic[res[i]["Code_postal"]] = [{"Commune": res[i]["Nom_commune"], "Latitude": gps[0], "Longitude": gps[2]}]
    print(json.dumps(dic, indent=2))

#get_coords_raw()

def get_lat_long(code_postal):
    res = read_coords("coords")
    if code_postal in res:
        for i in range(len(res[code_postal])):
            print(res[code_postal][i])

get_lat_long(str(sys.argv[1]))
