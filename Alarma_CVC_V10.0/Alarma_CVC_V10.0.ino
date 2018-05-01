/*
 Name:		Alarma_CVC_V10.ino
 Created:	29/04/2018 18:18:30
 Author:	Emi
*/

#include <SD.h>
#include <RTC.h>
#include <WiFiManager.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include "WifiAlarma.h"
#include "RelojAlarma.h"
#include "LectorSD.h"
#include "TelegramCom.h"
#include "AlarmaC.h"

RTC rtc(DST_ON); //inicalizamos el Reloj
RelojAlarma Reloj; //Crea un gestor del Reloj
AlarmaC alcObj; //Guarda los parametros de configuracion del proceso de alarma
LectorSD objSD(chipSelect, &Reloj, &alcObj); //Lee la tarjeta SD y almacena los parametros de entrada
WifiAlarma WifiObj;
TelegramCom telCom; //Gestiona las comunicaciones con Telegram
WiFiClientSecure client2; //Necesario para crear el bot de Telegram
UniversalTelegramBot bot(alcObj.leer("BotToken"), client2); //crea el bot de Telegram

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	while (!Serial) {

		; // wait for serial port to connect. Needed for Leonardo only

	}
	WifiObj.inicializa();
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
	pinMode(PIRPin, INPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	String mensaje = "";
	while (alcObj.estado == 0)
	{
		if ((millis() > alcObj.tiempo_activacion + 61000) && (alcObj.activacion_alarma == 1)) {
			Reloj.ProximaAlarma(rtc);
			alcObj.activacion_alarma = 0;
		}
		mensaje = alcObj.activaAlarma(bot, &Reloj, rtc); //comprueba la proxima alarma que va a iniciar el proceso y cambia los parametros para pasar a estado1
		if (mensaje == "Activar")
		{
			bot.sendMessage(telCom.chat_id, "Activar", "");
			alcObj.tiempo_activacion = millis();
			alcObj.activacion_alarma = 1;
		}
		if (mensaje == "ActivarV")
		{
			bot.sendMessage(telCom.chat_id, "Activar", "");
			bot.sendMessage(telCom.chat_id, "Activando modo vigia", "");
			alcObj.tiempo_activacion = millis();
			alcObj.activacion_alarma = 1;
		}

		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);//Comunicacion con Telegram esperando algun mensaje
		alcObj.enEstado0();//Se gestionan los parametros para cambio a estado 1
		delay(1);
	}
	/*
	//// En el siguiente estado (estado '1'), la alarma acaba de activarse, pero no estar� operativa hasta que pasen 20 segundos ////
	En este estado, el programa realizar� de forma secuencial las siguientes acciones:
	- El programa espera 20 segundos.
	- Posteriormente, el programa pasa a estado '2'.
	- Notifica el cambio a trav�s de Telegram.
	*/
	while (alcObj.estado == 1)
	{
		mensaje = alcObj.enEstado1();
		if (mensaje == "Activado el")
		{
			bot.sendMessage(telCom.chat_id, " Activado el " + String(N_Dispositivo) + " .", "");
			telCom.vigia = 0;
		}
		delay(100);
	}
	/*
	//// En el siguiente estado (estado '2') la alarma est� operativa e inspecciona si un intruso ha entrado en casa ////
	La detecci�n consta de dos modos:
	- Modo normal: Si detecta un intruso con el PIR pasa a estado 3
	- Modo Survey: Detecta el numero de veces que pasa el intruso por delante y registra la hora
	En este estado, el programa realizar� de forma secuencial las siguientes acciones:
	- Verificar si el sensor pir han detectado alg�n intruso.
	- Realizar una segunda comprobaci�n para evitar errores de medici�n del pir
	- Notificar por Telegram que hay un posible intruso
	- Pasar a estado 3
	*/
	while (alcObj.estado == 2)
	{
		//Comprobacion para que busque la p´roxima aalrma automaticamente
		if ((millis() > alcObj.tiempo_activacion + 61000) && (alcObj.activacion_alarma == 1)) {
			Reloj.ProximaAlarma(rtc);
			alcObj.activacion_alarma = 0;
		}
		//Fin de comprobacion
		mensaje = alcObj.activaAlarma(bot, &Reloj, rtc); //Comprobacion si hay alguna alarma programada
		if (mensaje == "Desactivar")
		{
			bot.sendMessage(telCom.chat_id, "Desactivar", "");
			bot.sendMessage(telCom.chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
			alcObj.activacion_alarma = 1;
		}
		if (mensaje == "DesacivarV")
		{
			bot.sendMessage(telCom.chat_id, "Vigia desactivadado. Dispositivo " + String(N_Dispositivo) + ".", "");
			alcObj.activacion_alarma = 1;
		}


		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		if (alcObj.modo_survey == 0) {
			telCom.chequeoPIRNormal(bot, &alcObj);//Chequea el estado del PIR en estado normal (espera que alquien pase por delante
		}
		else {
			telCom.chequeoPIRSurvey(rtc, Reloj, &alcObj);//Cuenta el numero de veces que pasa alguien por delante
		}
	}
	/*
	//// En el siguiente estado (estado '3') la alarma acaba de detectar un intruso y pondr� en funcionamiento la sirena (si se pone) si en 20 segundos no se introduce el c�digo PIN ////
	En este estado y durante 20 segundos, el programa realizar� de forma secuencial las siguientes acciones:
	- Espera al codigo de desactivacion de Telegram (clave) para desactivar la alarma antes de que pase al estado 4 o 6
	- Verificar si el n�mero marcado es el correcto (claveBien o claveMal).
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
	}
	/*
	//// En el siguiente estado (estado '4') la sirena de la alarma (si se conecta) est� en funcionamiento y se necesita el c�digo PIN para detener la alarma ////
	En este estado, el programa realizar� de forma secuencial las siguientes acciones:
	- Comprobar si se ha marcado enviado alg�n comando por Telegram para desactivar la alarma
	- Verificar si el PIN que devuelve el comando "Desactivar xxxx" es correcto (claveBien o claveMal).
	*/

	while (alcObj.estado == 4)
	{
		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		mensaje = alcObj.enEstado4();
		if (mensaje == "Sonando")
		{
			bot.sendMessage(telCom.chat_id, "Que te est� sonando la alarma!!!! Estado 4. Dispositivo " + String(N_Dispositivo) + ".", "");
		}
		/*
		if (alcObj.tiempo < millis() && (alcObj.repeticion < alcObj.maxRepeticion))
		{
		bot.sendMessage(telCom.chat_id, "Que te est� sonando la alarma!!!! Estado 4. Dispositivo " + String(N_Dispositivo) + ".", "");
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
		}*/
	}
	/*
	//// En el siguiente estado (estado '5') el c�digo PIN est� bloqueado y se necesita el c�digo PUK para activar la alarma ////
	Este estado no se ha trabajado todavia. Nunca se llega a estado 5 pero deja para futuras versiones
	*/

	while (alcObj.estado == 5)
	{
		alcObj.enEstado5();
	}
	/*
	//// En el siguiente estado (estado '6') el c�digo PIN est� bloqueado y se necesita el c�digo PUK para desactivar la alarma ////
	En este estado, el programa realizar� de forma secuencial las siguientes acciones:
	- Comprobar si se ha enviado alg�n comando por Telegram.
	- Verificar si el n�mero marcado PUK es el correcto (claveBien).
	*/

	while (alcObj.estado == 6)
	{
		telCom.TelegramServidor(&Reloj, &alcObj, &objSD, &rtc, &bot);
		alcObj.enEstado6();
	}
}
