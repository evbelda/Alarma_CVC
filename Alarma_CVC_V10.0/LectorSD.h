// LectorSD.h

#ifndef _LECTORSD_h
#define _LECTORSD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <SD.h>
	#include "RelojAlarma.h"
	#include "Hora.h"	
	#include "AlarmaC.h"
#else
	#include "WProgram.h"
#endif

#define chipSelect D8

class LectorSD
{
private: 
	File miFichero;
public:
	LectorSD(int pinSelect, RelojAlarma *reloj, AlarmaC *alcObj);
	//LectorSD(int pinSelect, RelojAlarma *reloj);
	void iniciaSD(int pinSelect, RelojAlarma *reloj, AlarmaC *alcObj);
	//void iniciaSD(int pinSelect, RelojAlarma *reloj);
	void leerTarjeta(int tipo, RelojAlarma *reloj, AlarmaC *alcObj);
	//void leerTarjeta(int tipo, RelojAlarma *reloj);
};
#endif

