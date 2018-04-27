// 
// 
// 

#include "TelegramCom.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegrambot.h>
#include "RelojAlarma.h"
#include "AlarmaC.h"
#include "LectorSD.h"
#include "RTC.h"

void TelegramCom::TelegramServidor(RelojAlarma *reloj, AlarmaC *alcObj, LectorSD *objSD, RTC *rtc, UniversalTelegramBot *bot) {

	int Orden_Dispositivo = 0; // variable sirve para indicar si este dispositivo debe responder en el canal de Telegram
	char cadenaMensaje[50]; //cadena temporal para subtividir los mensajes de Telegram
	String mensaje;
	//Serial.println("Estado: " + String(estado) + "-Chat_id: " + chat_id + " numMensajes: " + String(numNewMessages));

	if (tiempoTel < millis()) {
		numNewMessages = bot->getUpdates(bot->last_message_received + 1);
		while (numNewMessages) {
			for (int i = 0; i<numNewMessages; i++) {
				Orden_Dispositivo = 0;
				Serial.println("Mensaje nuevo");
				bot->messages[i].text.toCharArray(cadenaMensaje, 50);
				Serial.println(cadenaMensaje);
				char * item = strtok(cadenaMensaje, " ");
				if (String(item) == "Estado")
				{
					item = strtok(NULL, " ");
					while (item != NULL)
					{
						Orden_Dispositivo = 1;
						if (String(item) == String(N_Dispositivo)) {
							bot->sendMessage(bot->messages[i].chat_id, "El estado es: " + String(alcObj->estado) + ".Dispositivo " + String(N_Dispositivo) + ".", "");
						}
						item = strtok(NULL, " ");
					}
					if (Orden_Dispositivo == 0) {
						bot->sendMessage(bot->messages[i].chat_id, "El estado es: " + String(alcObj->estado) + ". Dispositivo " + String(N_Dispositivo) + ".", "");
					}
				}
				if (String(item) == "Activar")
				{
					item = strtok(NULL, " ");
					while (item != NULL)
					{
						Orden_Dispositivo = 1;
						if (String(item) == String(N_Dispositivo)) {
							Serial.println("Activando el " + N_Dispositivo);
							if (alcObj->estado == 0) {
								alcObj->claveBien = true;
							}
							else if (alcObj->estado == 5) {
								alcObj->claveBien = true;
								alcObj->claveMal = false;
								//Serial.println("Dentro de poner el puk estado 5 > 3");
							}
							//envio = 1;
							alcObj->estado = 1;
							bot->sendMessage(bot->messages[i].chat_id, "Se esta activando. Espera 20 segundos... Dispositivo " + String(N_Dispositivo) + ".", "");
						}
						item = strtok(NULL, " ");
					}
					if (Orden_Dispositivo == 0) {
						if (alcObj->estado == 0) {
							alcObj->claveBien = true;
						}
						else if (alcObj->estado == 5) {
							alcObj->claveBien = true;
							alcObj->claveMal = false;
							//Serial.println("Dentro de poner el puk estado 5 > 3");
						}
						//envio = 1;
						alcObj->estado = 1;
						bot->sendMessage(bot->messages[i].chat_id, "Se esta activando. Espera 20 segundos... Dispositivo " + String(N_Dispositivo) + ".", "");
					}
				}
				if (String(item) == "Desactivar")
				{
					Serial.println("Dentro de desactivar");
					item = strtok(NULL, " ");
					if (alcObj->estado == 2) {
						if (String(item) == alcObj->secretCode) {
							item = strtok(NULL, " ");
							while (item != NULL)
							{
								Orden_Dispositivo = 1;
								if (String(item) == String(N_Dispositivo)) {
									Serial.println("Dentro de desactivar disp");
									alcObj->envio = 1;
									alcObj->claveBien = true;
									alcObj->estado = 0;
									bot->sendMessage(bot->messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
								}
								item = strtok(NULL, " ");
							}
							if (Orden_Dispositivo == 0) {
								Serial.println("Dentro de desactivar disp");
								alcObj->envio = 1;
								alcObj->claveBien = true;
								alcObj->estado = 0;
								bot->sendMessage(bot->messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
							}
						}

					}
					else {
						//if ((config->estado == 3 || config->estado == 4) && (N_Desactivacion<3)) {
						if ((alcObj->estado == 3 || alcObj->estado == 4)) {
							if (String(item) == alcObj->secretCode) {
								Serial.println("Desactivar alarma");
								alcObj->envio = 1;
								alcObj->claveBien = true;
								alcObj->estado = 0;
								bot->sendMessage(bot->messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
								//N_Desactivacion = 0;
							}
						}
						else {
							alcObj->estado = 6;
							if (String(item) == alcObj->secretCode2) {
								Serial.println("Desactivar alarma");
								alcObj->envio = 1;
								alcObj->claveBien = true;
								alcObj->estado = 0;
								bot->sendMessage(bot->messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
								//N_Desactivacion = 0;
							}
						}

					}
				}
				if (String(item) == "Vigia")
				{
					item = strtok(NULL, " ");
					if (String(item) == "activar") {
						alcObj->modo_survey = 1;
						bot->sendMessage(bot->messages[i].chat_id, "Vigia activado dispositivo " + String(N_Dispositivo) + ": " + String(vigia), "");
					}
					else if (String(item) == "desactivar") {
						alcObj->modo_survey = 0;
						bot->sendMessage(bot->messages[i].chat_id, "Vigia desactivado dispositivo " + String(N_Dispositivo) + ".", "");
					}
					else if (String(item) == "reset") {
						vigia = 0;
						bot->sendMessage(bot->messages[i].chat_id, "Vigia reiniciado dispositivo " + String(N_Dispositivo) + ": " + String(vigia), "");
					}
					else if (String(item) == "estado")
						bot->sendMessage(bot->messages[i].chat_id, "Vigia dispositivo " + String(N_Dispositivo) + ": " + String(vigia) + "." + vigia_tiempo, "");
				}
				if (String(item) == "Horas")
				{
					item = strtok(NULL, " ");
					if (String(item) == "listado") {
						bot->sendMessage(bot->messages[i].chat_id, "Las alarmas existentes son:", "");
						for (int j = 0; j< reloj->alarmas; j++) {
							bot->sendMessage(bot->messages[i].chat_id, reloj->H[j].tipo_l() + " hora: " + String(reloj->H[j].hora_l()) + ":" + String(reloj->H[j].minuto_l()) + " dias: " + String(reloj->H[j].dias_leer()), "");
						}
					}
					else if (String(item) == "activar") {
						objSD->leerTarjeta(1, reloj, alcObj);
						reloj->ProximaAlarma(rtc);
						alcObj->modo_alarma = 1;
						bot->sendMessage(bot->messages[i].chat_id, "Se han activado las alarmas automaticas.", "");
					}
					else if (String(item) == "leer") {
						objSD->leerTarjeta(1, reloj, alcObj);
						reloj->ProximaAlarma(rtc);
						//modo_alarma = 1;
						bot->sendMessage(bot->messages[i].chat_id, "Releyendo alarma de tarjeta.", "");
					}
					else if (String(item) == "desactivar") {
						alcObj->modo_alarma = 0;
						bot->sendMessage(bot->messages[i].chat_id, "Se han desactivado la automatizaci�n de la alarma.", "");
					}
					else if (String(item) == "estado") {
						if (alcObj->modo_alarma == 1) {
							bot->sendMessage(bot->messages[i].chat_id, "Proxima hora: tipo " + reloj->H[reloj->ultima].tipo_l() + " hora: " + String(reloj->H[reloj->ultima].hora_l()) + ":" + String(reloj->H[reloj->ultima].minuto_l()) + " dias: " + String(reloj->H[reloj->ultima].dias_leer()), "");
							bot->sendMessage(chat_id, "La hora actual es: " + reloj->ConvierteTiempo(rtc), "");
						}
						else {
							bot->sendMessage(bot->messages[i].chat_id, "La automatizacion de la alarma est� desactivada.", "");
						}
					}
				}
				chat_id = bot->messages[i].chat_id;
				//Serial.println("El chat id es: " + String(chat_id));                    
			}
			numNewMessages = bot->getUpdates(bot->last_message_received + 1);

		}
		tiempoTel = millis() + Bot_mtbs;
	}
	else
		tiempoTel = millis();
}

void TelegramCom::chequeoPIRNormal(UniversalTelegramBot bot, AlarmaC *alcObj)
{
	if (digitalRead(PIRPin) == HIGH) {
		if (alcObj->valorRepAlarma != 1) {
			alcObj->tiempoEnvio = millis();
			Serial.println("No lo cambies por dios");
			alcObj->valorRepAlarma = 1;
		}
		Serial.println("Dentro de pir1: " + String(alcObj->valorRepAlarma) + "-" + String(alcObj->tiempoEnvio));
	}
	if (millis()>alcObj->tiempoEnvio + alcObj->tiempoDeteccion) {
		Serial.println("Dentro de pir2: " + String(alcObj->valorRepAlarma) + "-" + String(alcObj->tiempoEnvio));
		if ((digitalRead(PIRPin) == HIGH && alcObj->valorRepAlarma == 1))
		{
			alcObj->estado = 3;
			Serial.println("Dentro de pir3");
			bot.sendMessage(chat_id, "Que tienes a alguien en casa!!!! Estado 3. Dispositivo " + String(N_Dispositivo) + " .", "");
			delay(1000);
		}
		else
			alcObj->valorRepAlarma = 0;
	}
}


void TelegramCom::chequeoPIRSurvey(RTC rtc, RelojAlarma reloj, AlarmaC *alcObj)
{
	if (digitalRead(PIRPin) == HIGH) {
		if (alcObj->valorRepAlarma != 1) {
			alcObj->tiempoEnvio = millis();
			alcObj->valorRepAlarma = 1;
		}
	}
	if (millis()>alcObj->tiempoEnvio + alcObj->tiempoDeteccion) {
		if ((digitalRead(PIRPin) == HIGH && alcObj->valorRepAlarma == 1))
		{
			vigia_tiempo = reloj.ConvierteTiempo(rtc);
			vigia++;
			delay(100);
		}
		else
			alcObj->valorRepAlarma = 0;
	}
}

