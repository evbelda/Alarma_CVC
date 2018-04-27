// 
// 
// 

#include "AlarmaC.h"
#include <UniversalTelegramBot.h>
#include "RelojAlarma.h"
#include "RTC.h"


String AlarmaC::leer(String cadena)
{
	if (cadena == "BotToken")
	{
		return BotToken;
	}
	if (cadena == "BotName")
	{
		return BotName;
	}
	if (cadena == "BotUsername")
	{
		return BotUsername;
	}
	if (cadena == "Ssid")
	{
		return Ssid;
	}
	if (cadena == "Pwd")
	{
		return Pwd;
	}
}

String AlarmaC::escribir(String cadena)
{
	if (cadena == "BotToken")
	{
		cadena.toCharArray(BotToken, cadena.length() + 1);
	}
	if (cadena == "BotName")
	{
		cadena.toCharArray(BotName, cadena.length() + 1);
	}
	if (cadena == "BotUsername")
	{
		cadena.toCharArray(BotUsername, cadena.length() + 1);
	}
	if (cadena == "Ssid")
	{
		cadena.toCharArray(Ssid, cadena.length() + 1);
	}
	if (cadena == "Pwd")
	{
		cadena.toCharArray(Pwd, cadena.length() + 1);
	}
}

String AlarmaC::activaAlarma(UniversalTelegramBot bot, RelojAlarma *reloj, RTC rtc)
{	
	String mensaje = "";
	if (modo_alarma == 1) {
		if (reloj->CompruebaAlarma(rtc)) {
			Serial.println("El tipo de alarma es: " + reloj->H[reloj->ultima].tipo_l());
			if (reloj->H[reloj->ultima].tipo_l() == "ON") {
				Serial.println("Se activa la alarma en ON: " + reloj->H[reloj->ultima].tipo_l());
				claveBien = true;
				mensaje = "Activar";
				return mensaje;
				//bot.sendMessage(telAlarm->chat_id, "Activar", "");
			}
			else if (reloj->H[reloj->ultima].tipo_l() == "ONV") {
				
				//bot.sendMessage(telAlarm->chat_id, "Activar", "");
				modo_survey = 1;
				claveBien = true;
				mensaje = "ActivarV";
				return mensaje;
				//bot.sendMessage(telAlarm->chat_id, "Activando modo vigia", "");
			}
			if (reloj->H[reloj->ultima].tipo_l() == "OFF") {
				//bot.sendMessage(telAlarm->chat_id, "Desactivar", "");
				envio = 1;
				claveBien = true;
				estado = 0;
				mensaje = "Desactivar";
				return mensaje;
				//bot.sendMessage(telAlarm->chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
			}
			if (reloj->H[reloj->ultima].tipo_l() == "OFV") {
				//bot.sendMessage(telAlarm->chat_id, "Desactivar", "");
				envio = 1;
				claveBien = true;
				estado = 0;
				modo_survey = 0;
				//bot.sendMessage(telAlarm->chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) + ".", "");
				//bot.sendMessage(telAlarm->chat_id, "Vigia desactivado dispositivo " + String(N_Dispositivo) + ".", "");
				mensaje = "DesactivarV";
				return mensaje;
			}
		}
	}
}

void AlarmaC::enEstado0()
{
	repeticion = 0;
	if (envio == 1) {
		claveBien = false;
		envio = 0;
	}
	if (claveBien)
	{
		estado = 1;
		//Serial.print("1");
		claveBien = false;
		tiempoEnvio = millis();
	}
	else if (claveMal)
	{
		estado = 5;
		//Serial.print("5");
		claveMal = false;
	}
}

void AlarmaC::enEstado3()
{
	valorRepAlarma = millis();
	if (claveBien)
	{
		estado = 0;
		claveBien = false;
	}
	else if (claveMal)
	{
		estado = 6;
		//Serial.print("6");
		claveMal = false;
	}
	else if (estado != 0)
	{
		estado = 4;
		tiempo = 0;
		//Serial.print("4");
	}
}

void AlarmaC::enEstado5()
{
	if (claveBien)
	{
		estado = 1;
		//Serial.print("1");
		claveBien = false;
	}
}

void AlarmaC::enEstado6()
{
	if (tiempo < millis() && (repeticion < maxRepeticion))
	{
		tiempo = millis() + 20000;
		repeticion++;
	}
	if (claveBien)
	{
		estado = 0;
		Serial.print("0");
		claveBien = false;
	}
}