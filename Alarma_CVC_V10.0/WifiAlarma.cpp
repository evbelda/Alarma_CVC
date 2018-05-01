// 
// 
// 

#include "WifiAlarma.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "AlarmaC.h"



bool shouldSaveConfig = false;


void WifiAlarma::inicializa()
{
	//Serial.begin(9600);
	wifiManager.setAPCallback(configModeCallback); //Cuando la conexión al WiFi falla, se entra en el modo AP
	wifiManager.setConnectTimeout(10); // Se da un tiempo de 10 segundos para iniciar la conexion a la red WiFi
	wifiManager.autoConnect("Local_modulo " + N_Dispositivo); // Nombre de la red WiFi creada por la ESP8266-01

	if (!wifiManager.autoConnect()) {
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(1000);
	}

	// Conexion a la red wifi exitosa
	Serial.println("Conectado de forma exitosa a la red WiFi");

	//set config save notify callback
	wifiManager.setSaveConfigCallback(saveConfigCallback);

	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");
}

void configModeCallback(WiFiManager *myWiFiManager) {
	Serial.println("Accediendo al Modo de Configuración");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

//flag para guardar datos

void saveConfigCallback() {
	Serial.println("Debería grabar la configuracion");
	shouldSaveConfig = true;
}