import serial
import datetime

# Create string with format 2019-3-09_19.15.02.csv
currTime = str(datetime.datetime.now().strftime("%Y-%m-%d_%H.%M.%S"))
filename = str(currTime + ".csv")

# Create file with time stamp name and add column headers for easier analysis
startFile = open(filename, 'w+')
startFile.write("Time [ms],CO2 [ppm],Temp [dC],Heatpad (ON/OFF)")

# This COM port may need to be changed before the script works properly. Check Arduino IDE. For Windows it'll likely be
# COM6 or COM3. Also, make sure .ino baud rate and baud rate here agree.
ser = serial.Serial('COM6', 9600)

while True:
	line = ser.readline().decode('utf-8')

	print(line) # Display outputs in console for viewing

	# Append data to .csv file
	dataAppend = open(filename, 'a')
	dataAppend.write(line)