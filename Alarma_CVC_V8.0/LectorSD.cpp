// 
// 
// 

#include "LectorSD.h"
#include <SD.h>
#include "RelojAlarma.h"
#include "Hora.h"
#include "AlarmaC.h"


//LectorSD::LectorSD(int pinSelect, RelojAlarma *reloj, Configuracion *config)
LectorSD::LectorSD(int pinSelect, RelojAlarma *reloj, AlarmaC *alcObj)
{
	pinMode(chipSelect, OUTPUT);
	if (!SD.begin(chipSelect)) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	leerTarjeta(0, reloj, alcObj);
}






void LectorSD::iniciaSD(int chipSelect, RelojAlarma *reloj, AlarmaC *alcObj)
{
	pinMode(chipSelect, OUTPUT);
	if (!SD.begin(chipSelect)) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	leerTarjeta(0, reloj, alcObj);
}

//void LectorSD::leerTarjeta(int tipo, RelojAlarma *reloj, A *config) {
void LectorSD::leerTarjeta(int tipo, RelojAlarma *reloj, AlarmaC *alcObj) {
	int i;
	String cadenatemp;
	String cadena[15];
	reloj->alarmas = 0;
	reloj->ultima = -1;

	for (i = 0; i<15; i++) {
		cadena[i] = "";
	}

	/*cadena[0] = "H1=ON;18:35;1,2,3,4,5";
	cadena[1] = "H2=OFF;18:37;1,2,3,4,5";
	cadena[2] = "H3=ON;12:00;3";
	cadena[3] = "H4=OFF;15:00;3";
	cadena[4] = "H5=ONV;8:10;3";
	cadena[5] = "H6=OFV;11:59;3";*/

	//---------------------buscamos el archivo y lo leermos
	miFichero = SD.open("datos.txt");
	Serial.println("Leemos el fichero:" + String(miFichero));

	if (miFichero) {

		Serial.println("datos.txt:");

		// read from the file until there's nothing else in it:
		i = 0;
		while (miFichero.available()) {

			char caracter = miFichero.read();
			cadena[i] = cadena[i] + caracter;
			if ((caracter == 10) && (i <15)) {
				i++;
			}
		}
		//myFile.close();
		Serial.println("La tarjeta leida es");
		for (int j = 0; j <= i; j++) {
			Serial.println(cadena[j]);
		}
		// close the file:
	}
	else {

		// if the file didn't open, print an error:

		Serial.println("error opening datos.txt");

	}
	miFichero.close();
	//-------------una vez leido lo almacenamos en cadena--------

	for (int j = 0; j < 15; j++) {

		//Serial.println(cadena[j]);
		if (tipo == 0) {
			if (cadena[j].indexOf("BotToken") >= 0) {
				cadenatemp = cadena[j].substring(cadena[j].indexOf("=") + 1);
				alcObj->escribir(cadenatemp);
			}
			if (cadena[j].indexOf("BOTname") >= 0) {
				cadenatemp = cadena[j].substring(cadena[j].indexOf("=") + 1);
				alcObj->escribir(cadenatemp);
			}
			if (cadena[j].indexOf("BOTusername") >= 0) {
				cadenatemp = cadena[j].substring(cadena[j].indexOf("=") + 1);
				alcObj->escribir(cadenatemp);
			}
			if (cadena[j].indexOf("SSID") >= 0) {
				cadenatemp = cadena[j].substring(cadena[j].indexOf("=") + 1);
				alcObj->escribir(cadenatemp);
			}
			if (cadena[j].indexOf("PWD") >= 0) {
				cadenatemp = cadena[j].substring(cadena[j].indexOf("=") + 1);
				alcObj->escribir(cadenatemp);
			}
		}
		if ((tipo == 1) || (tipo == 0)) {
			if (cadena[j].indexOf("H1=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
			if (cadena[j].indexOf("H2=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
			if (cadena[j].indexOf("H3=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
			if (cadena[j].indexOf("H4=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
			if (cadena[j].indexOf("H5=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
			if (cadena[j].indexOf("H6=") >= 0) {
				reloj->H[reloj->alarmas].temp = cadena[j];
				reloj->H[reloj->alarmas].guardar();
				reloj->alarmas++;
				Serial.println("dentro de  alarmas" + cadena[j]);
			}
		}
	}
}

