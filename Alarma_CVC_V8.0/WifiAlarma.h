// WifiAlarma.h

#ifndef _WIFIALARMA_h
#define _WIFIALARMA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <ESP8266WiFi.h>
	#include <WiFiManager.h>
	#include "AlarmaC.h"
	//#include "Configuracion.h"
#else
	#include "WProgram.h"
#endif

void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();

class WifiAlarma {
public:
	WiFiManager wifiManager;
	//bool shouldSaveConfig = true;
	void inicializa();
	//void configModeCallback(WiFiManager *myWiFiManager);
	//void saveConfigCallback();
};


#endif

