/*
 Name:		Alarma.ino
 Created:	14/04/2018 22:55:29
 Author:	Emi
 */

 // the setup function runs once when you press reset or power the board

#include "TelegramCom.h"
#include "AlarmaC.h"
#include "WifiAlarma.h"
 //#include "Configuracion.h"
#include "LectorSD.h"
#include <RTC.h>
#include "Hora.h"
#include "RelojAlarma.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>

RTC rtc(DST_ON); //inicalizamos el Reloj
WiFiClientSecure client2;
RelojAlarma Reloj; //Crea un gestor del Reloj
				   //Configuracion config; //Guarda la configuracion
AlarmaC alcObj;
LectorSD objSD(chipSelect, &Reloj, &alcObj);
//UniversalTelegramBot bot(Config.leer("BotToken"), client2);
TelegramCom telCom;
WifiAlarma WifiObj;
UniversalTelegramBot bot(alcObj.leer("BotToken"), client2);


void setup() {

	// Open serial communications and wait for port to open:

	Serial.begin(9600);

	while (!Serial) {

		; // wait for serial port to connect. Needed for Leonardo only

	}

	WifiObj.inicializa();
	pinMode(PIRPin, INPUT);

	//----------Ponemos el reloj en Hora si ha perdido la corriente
	//rtc.setDateTime( 2018, 04, 12, 18, 06, 00 ); // Year, Month, Day, Hour24H, Minutes and Seconds
	Serial.println("La hora es: " + Reloj.ConvierteTiempo(rtc));


	for (int k = 0; k < Reloj.alarmas; k++) {
		Serial.println("Las alarmas son las siguientes:");
		Reloj.H[k].leer();
	}

	Reloj.ProximaAlarma(rtc);
	if (Reloj.ultima != -1) {
		Serial.println("La proxima alarma es");
		Serial.println("Alarma tipo:" + Reloj.H[Reloj.ultima].tipo_l() + " " + String(Reloj.H[Reloj.ultima].hora_l()) + ":" + String(Reloj.H[Reloj.ultima].minuto_l()));
		Serial.println("Dia: " + Reloj.H[Reloj.ultima].dias_leer());
	}
	else {
		Serial.println("No ha alarma detectada");
	}
}

void loop() {
	String mensaje = "";
	while (alcObj.estado == 0)
	{
		/*if (alcObj.modo_alarma == 1) {
			//config.activaAlarma(bot, &telCom, &Reloj);
			if (Reloj.CompruebaAlarma(rtc)) {
				Serial.println("El tipo de alarma es: " + Reloj.H[Reloj.ultima].tipo_l());
				if (Reloj.H[Reloj.ultima].tipo_l() == "ON") {
					Serial.println("Se activa la alarma en ON: " + Reloj.H[Reloj.ultima].tipo_l());
					bot.sendMessage(telCom.chat_id, "Activar", "");
					alcObj.claveBien = true;

				}
				else if (Reloj.H[Reloj.ultima].tipo_l() == "ONV") {
					bot.sendMessage(telCom.chat_id, "Activar", "");
					alcObj.modo_survey = 1;
					alcObj.claveBien = true;
					bot.sendMessage(telCom.chat_id, "Activando modo vigia", "");
				}
			}
		}*/

		mensaje = alcObj.activaAlarma(bot, &Reloj, rtc);
		if (mensaje == "Activar")
		{
			bot.sendMessage(telCom.chat_id, "Activar", "");
		}
		if (mensaje == "ActivarV")
		{
			bot.sendMessage(telCom.chat_id, "Activar", "");
			bot.sendMessage(telCom.chat_id, "Activando modo vigia", "");
		}

		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		alcObj.enEstado0();
		/*alcObj.repeticion = 0;
		if (alcObj.envio == 1) {
			alcObj.claveBien = false;
			alcObj.envio = 0;
		}
		if (alcObj.claveBien)
		{
			alcObj.estado = 1;
			//Serial.print("1");
			alcObj.claveBien = false;
			alcObj.tiempoEnvio = millis();
		}
		else if (alcObj.claveMal)
		{
			alcObj.estado = 5;
			//Serial.print("5");
			alcObj.claveMal = false;
		}*/
		delay(1);
	}
	/*
	//// En el siguiente estado (estado '1'), la alarma acaba de activarse, pero no estará operativa hasta que pasen 20 segundos ////
	En este estado, el programa realizará de forma secuencial las siguientes acciones:
	- El programa espera 20 segundos.
	- Posteriormente, el programa pasa a estado '2'.
	- Notifica el cambio a través de Telegram.
	*/
	while (alcObj.estado == 1)
	{
		//config.enEstado1(&telCom, bot);

		if (millis()> alcObj.tiempoEnvio + alcObj.tiempoEsperaSonar) {
			alcObj.estado = 2;
			//Serial.print("2"); 
			alcObj.valorRepAlarma = 0;
			bot.sendMessage(telCom.chat_id, " Activado el " + String(N_Dispositivo) + " .", "");
			telCom.vigia = 0;
		}
		delay(100);
	}
	/*
	//// En el siguiente estado (estado '2') la alarma está operativa e inspecciona si un intruso ha entrado en casa ////
	La detección consta de dos modos:
	- Modo normal: Si detecta un intruso con el PIR pasa a estado 3
	- Modo Survey: Detecta el numero de veces que pasa el intruso por delante y registra la hora
	En este estado, el programa realizará de forma secuencial las siguientes acciones:
	- Verificar si el sensor pir han detectado algún intruso.
	- Realizar una segunda comprobación para evitar errores de medición del pir
	- Notificar por Telegram que hay un posible intruso
	- Pasar a estado 3
	*/
	while (alcObj.estado == 2)
	{
		mensaje = alcObj.activaAlarma(bot, &Reloj, rtc);
		if (mensaje == "Desactivar")
		{
			bot.sendMessage(telCom.chat_id, "Desactivar", "");
			bot.sendMessage(telCom.chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
		}
		if (mensaje == "DesacivarV")
		{
			bot.sendMessage(telCom.chat_id, "Vigia desactivadado. Dispositivo " + String(N_Dispositivo) + ".", "");

		}
		/*
		if (alcObj.modo_alarma == 1) {
			//alcObj.activaAlarma(bot, &telAlarma, &Reloj);
			
			
			if (Reloj.CompruebaAlarma(rtc)) {
				if (Reloj.H[Reloj.ultima].tipo_l() == "OFF") {
					bot.sendMessage(telCom.chat_id, "Desactivar", "");
					alcObj.envio = 1;
					alcObj.claveBien = true;
					alcObj.estado = 0;
					bot.sendMessage(telCom.chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
				}
				if (Reloj.H[Reloj.ultima].tipo_l() == "OFV") {
					bot.sendMessage(telCom.chat_id, "Desactivar", "");
					alcObj.envio = 1;
					alcObj.claveBien = true;
					alcObj.estado = 0;
					alcObj.modo_survey = 0;
					bot.sendMessage(telCom.chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
					bot.sendMessage(telCom.chat_id, "Vigia desactivado dispositivo " + String(N_Dispositivo) + ".", "");
				}
			}
		}*/

		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		if (alcObj.modo_survey == 0) {
			telCom.chequeoPIRNormal(bot, &alcObj);
		}
		else {
			telCom.chequeoPIRSurvey(rtc, Reloj, &alcObj);
		}
	}
	/*
	//// En el siguiente estado (estado '3') la alarma acaba de detectar un intruso y pondrá en funcionamiento la sirena (si se pone) si en 20 segundos no se introduce el código PIN ////
	En este estado y durante 20 segundos, el programa realizará de forma secuencial las siguientes acciones:
	- Espera al codigo de desactivacion de Telegram (clave) para desactivar la alarma antes de que pase al estado 4 o 6
	- Verificar si el número marcado es el correcto (claveBien o claveMal).
	- Si la claveMal esta a true pasa al estado 6 en el que hay que poner el PUK
	- Si la claveBien esta a true pasa al estado 0 y desactiva la alarma
	- Si no se pone ninguna clave pasa al estado 4
	*/
	while (alcObj.estado == 3)
	{
		alcObj.tiempo = millis() + alcObj.tiempoEsperaSonar;
		do
		{
			telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
			if (alcObj.claveBien || alcObj.claveMal)
			{
				break;
			}
		} while (alcObj.tiempo > millis());
		alcObj.enEstado3();
		/*alcObj.valorRepAlarma = millis();
		if (alcObj.claveBien)
		{
			alcObj.estado = 0;
			alcObj.claveBien = false;
		}
		else if (alcObj.claveMal)
		{
			alcObj.estado = 6;
			//Serial.print("6");
			alcObj.claveMal = false;
		}
		else if (alcObj.estado != 0)
		{
			alcObj.estado = 4;
			alcObj.tiempo = 0;
			//Serial.print("4");
		}*/
	}
	/*
	//// En el siguiente estado (estado '4') la sirena de la alarma (si se conecta) está en funcionamiento y se necesita el código PIN para detener la alarma ////
	En este estado, el programa realizará de forma secuencial las siguientes acciones:
	- Comprobar si se ha marcado enviado algún comando por Telegram para desactivar la alarma
	- Verificar si el PIN que devuelve el comando "Desactivar xxxx" es correcto (claveBien o claveMal).
	*/

	while (alcObj.estado == 4)
	{
		//Serial.println(String(tiempo)+"-" + String(millis())+ "repeticion:"+ String(repeticion));
		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		//config.enEstado4(&telAlarma, &bot);
		if (alcObj.tiempo < millis() && (alcObj.repeticion < alcObj.maxRepeticion))
		{
			bot.sendMessage(telCom.chat_id, "Que te está sonando la alarma!!!! Estado 4. Dispositivo " + String(N_Dispositivo) + ".", "");
			alcObj.tiempo = millis() + alcObj.tiempoEntreCorreos;
			alcObj.repeticion++;
		}

		if (alcObj.claveBien)
		{
			alcObj.estado = 0;
			alcObj.claveBien = false;
		}
		else if (alcObj.claveMal)
		{
			alcObj.repeticion = 0;
			alcObj.tiempo = 0;
			alcObj.estado = 6;
			alcObj.claveMal = false;
		}
	}
	/*
	//// En el siguiente estado (estado '5') el código PIN está bloqueado y se necesita el código PUK para activar la alarma ////
	Este estado no se ha trabajado todavia. Nunca se llega a estado 5 pero deja para futuras versiones
	*/

	while (alcObj.estado == 5)
	{
		//telegramServidor();
		alcObj.enEstado5();
		/*if (alcObj.claveBien)
		{
			alcObj.estado = 1;
			//Serial.print("1");
			alcObj.claveBien = false;
		}*/
	}
	/*
	//// En el siguiente estado (estado '6') el código PIN está bloqueado y se necesita el código PUK para desactivar la alarma ////
	En este estado, el programa realizará de forma secuencial las siguientes acciones:
	- Comprobar si se ha enviado algún comando por Telegram.
	- Verificar si el número marcado PUK es el correcto (claveBien).
	*/

	while (alcObj.estado == 6)
	{
		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		alcObj.enEstado6();
		/*if (alcObj.tiempo < millis() && (alcObj.repeticion < alcObj.maxRepeticion))
		{
			alcObj.tiempo = millis() + 20000;
			alcObj.repeticion++;
		}
		if (alcObj.claveBien)
		{
			alcObj.estado = 0;
			Serial.print("0");
			alcObj.claveBien = false;
		}*/
	}
}