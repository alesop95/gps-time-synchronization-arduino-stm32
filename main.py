import serial	
def getMsFromStr(time_string):
ms = 0

ms += int(time_string[:2]) * 3600000
ms += int(time_string[2:4]) * 60000
ms += int(time_string[4:6]) * 1000
ms += int(time_string[7:10])

return ms		

ser = serial.Serial('COM3', 9600)	

initial_ms = 0	
initial_ard_time = 0	

output_file = open('output.csv', 'w+')	
output_file.write("sep=,\n")

try:
while True:
single_pair = ser.readline()	
	splitted_pair = single_pair.split(',')		
	left_side_ms = getMsFromStr(splitted_pair[0])	
        if(initial_ms == 0):
initial_ms = left_side_ms

if(initial_ard_time == 0):
initial_ard_time = int(splitted_pair[1])	
        computed_ard_time = int(splitted_pair[1]) - initial_ard_time
        final_res = str(left_side_ms - initial_ms) + ',' + 	str(computed_ard_time)
        output_file.write(final_res + "\n")	
        print final_res
except KeyboardInterrupt:
pass
output_file.close();
