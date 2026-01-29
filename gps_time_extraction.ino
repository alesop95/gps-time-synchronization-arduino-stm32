#include <SoftwareSerial.h>
#define ARDUINO_GPS_RX 9
#define ARDUINO_GPS_TX 8
#define GPS_BAUD_RATE 9600	 
#define SerialMonitor Serial

/*Crea un oggetto SoftwareSerial chiamato "gpsPort" con ARDUINO_GPS_TX per ricevere dati e ARDUINO_GPS_RX per la trasmissione*/
SoftwareSerial gpsPort(ARDUINO_GPS_TX, ARDUINO_GPS_RX);

//Inizializzazioni variabili
long tempo_arduino;
int index = 0;
int header_index, timer_index = 0;
bool header_flag,flag_timer = false;

char stringa[100];
char header[7];
char tempo[10];
char stringa_voluta[7] = {"$GPGGA,"};

void setup()
{
gpsPort.begin(GPS_BAUD_RATE);
SerialMonitor.begin(9600);
}

void loop()
{
tempo_arduino = millis();

if (gpsPort.available())

{
char str1 = gpsPort.read();

stringa[index] = str1;

if (flag_timer == true){
if (timer_index >= 10){
timer_index=0;
flag_timer = false;
SerialMonitor.print(tempo);
SerialMonitor.print(";");
SerialMonitor.print(tempo_arduino);
SerialMonitor.print(";");
}
else{
tempo[timer_index] = str1;
timer_index++;
}
}

if(str1 == '$'){
header_flag = true;
}


if (header_flag == true){
if (header_index <=5){
header[header_index] = str1;
header_index++;
}
else{
header_flag = false;
header_index=0;
}
if(strcmp(header, "$GPGGA") == 0){
flag_timer = true;
}
}
}
}
