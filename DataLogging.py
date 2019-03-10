import serial
import datetime

currTime = str(datetime.datetime.now().strftime("%Y-%m-%d %H.%M.%S"))
filename = str(currTime + ".csv")

startFile = open(filename, 'w+')
startFile.write("Time [ms],CO2 [ppm],Temp [dC],Heatpad (ON/OFF)")

ser = serial.Serial('COM6', 9600)

while True:
	line = ser.readline().decode('utf-8')

	print(line) # Display outputs in console for viewing

	dataAppend = open(filename, 'a')
	dataAppend.write(line)