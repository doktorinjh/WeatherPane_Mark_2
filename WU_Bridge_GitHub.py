import urllib2
import json

#Get json data from Weather Underground by using IP address
f = urllib2.urlopen('http://api.wunderground.com/api/***KEY***/conditions/hourly/q/autoip.json')
json_string = f.read()
parsed_json = json.loads(json_string)

#Current Location and Observation Time
location = parsed_json['current_observation']['display_location']['full']
time = parsed_json['current_observation']['observation_time']
station = parsed_json['current_observation']['station_id']

#Current Conditions
current_temp = parsed_json['current_observation']['temp_f']
current_weather = parsed_json['current_observation']['weather']
current_precip = parsed_json['hourly_forecast'][0]['pop']

#Write output to python text file
#f = open( 'C:\Users\User\Desktop\WU_Test.txt', 'w' ) #Used for testing
f = open( '/WeatherPaneScripts/WU_Variables.py', 'w' ) #Direct to files
f.write("import sys" + '\n' )
f.write(""'\n')
f.write("sys.path.insert(0, '/usr/lib/python2.7/bridge/')" + '\n' )
f.write(""'\n')
f.write("from bridgeclient import BridgeClient as bridgeclient" + '\n' )
f.write(""'\n')
f.write("value = bridgeclient()" + '\n' )
f.write(""'\n')
f.write("value.put(\"location\",'{}')".format((location)) + '\n')
f.write("value.put(\"time\",'{}')".format((time)) + '\n')
f.write("value.put(\"station\",'{}')".format((station)) + '\n')
f.write(""'\n')
f.write("value.put(\"current_temp\",'{}')".format((current_temp)) + '\n') 
f.write("value.put(\"current_weather\",'{}')".format((current_weather)) + '\n')
f.write("value.put(\"current_precip\",'{}')".format((current_precip)) + '\n')
f.close()