# Original script written by chatgpt with prompt
#  write me a python script to get the latitude, longitude, date/time and magnitude of earthquakes for the last 30 days
# Requires `pip3 install requests`

# Adapted by Phill Conrad to generate note list for Allolib

# To use, do:
# python3 tutorials/allolib-f23/get_earthquakes.py > tutorials/synthesis/bin/SineEnv-data/earthquakes.synthSequence


import requests
from pprint import pprint
from datetime import datetime, timedelta

def printEarthquakeAsComment(earthquake):
    properties = earthquake['properties']
    geometry = earthquake['geometry']
    latitude = geometry['coordinates'][1]
    longitude = geometry['coordinates'][0]
    date_time = datetime.utcfromtimestamp(properties['time'] / 1000.0).strftime('%Y-%m-%d %H:%M:%S')
    magnitude = properties['mag']
    title = properties['title']
    print(f"# Title: {title} Date/Time: {date_time}, Latitude: {latitude}, Longitude: {longitude}, Magnitude: {magnitude}")

def printEarthquakeAsNote(earthquake, times, magnitudes, longitudes):
    properties = earthquake['properties']
    geometry = earthquake['geometry']
    latitude = geometry['coordinates'][1]
    longitude = geometry['coordinates'][0]
    date_time = datetime.utcfromtimestamp(properties['time'] / 1000.0).strftime('%Y-%m-%d %H:%M:%S')
    magnitude = properties['mag']
    amplitude = scaleX(magnitude, magnitudes, 0.01, 0.1)
    scaled_time = scaleX(properties['time'], times, 1, 60)
    pitch = scaleX(magnitude, magnitudes, 110, 880)
    duration = scaleX(magnitude, magnitudes, 0.1, 2.0)
    pan = scaleX(longitude, longitudes, -1, 1)
    # @ time duration SineEnv amplitude frequency attackTime releaseTime pan x y 
    print(f"@ {scaled_time} {duration} SineEnv {amplitude} {pitch} 0.01 1.0 {pan} {longitude} {latitude} ")

def scaleX(x, xValues, minScale, maxScale):
    minX = min(xValues)
    maxX = max(xValues)
    return minScale + (maxScale - minScale) * (x - minX) / (maxX - minX)

# MAIN PROGRAM STARTS HERE

# Define the API endpoint and parameters
base_url = "https://earthquake.usgs.gov/fdsnws/event/1/query"
start_date = (datetime.now() - timedelta(days=30)).strftime("%Y-%m-%d")
end_date = datetime.now().strftime("%Y-%m-%d")
min_magnitude = 5.5  # Minimum earthquake magnitude

# Define the request parameters
parameters = {
    "format": "geojson",
    "starttime": start_date,
    "endtime": end_date,
    "minmagnitude": min_magnitude,
}

# Send a GET request to the USGS API
response = requests.get(base_url, params=parameters)

# Check if the request was successful
if response.status_code == 200:
    data = response.json()
    #pprint(data, indent=4)
    # Extract and print earthquake information
    times = list(map(lambda x: x['properties']['time'], data['features']));
    magnitudes = list(map(lambda x: x['properties']['mag'], data['features']));
    longitudes = list(map(lambda x: x['geometry']['coordinates'][0], data['features']));
    for earthquake in data['features']:
        printEarthquakeAsComment(earthquake)
        printEarthquakeAsNote(earthquake, times, magnitudes, longitudes)
else:
    print(f"# Error: Unable to retrieve earthquake data. Status code {response.status_code}")
