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
	if (cadena == "SCode")
	{
		return secretCode;
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
	if (cadena == "SCode")
	{
		secretCode = String(cadena);
	}
}

String AlarmaC::activaAlarma(UniversalTelegramBot bot, RelojAlarma *reloj, RTC rtc)
{	
	String mensaje = "";
	if (modo_alarma == 1) {
		if (reloj->CompruebaAlarma(rtc)) {
			Serial.println("El tipo de alarma es: " + reloj->H[reloj->ultima].tipo_l());
			if (reloj->H[reloj->ultima].tipo_l() == "ON") 
			{
				Serial.println("Se activa la alarma en ON: " + reloj->H[reloj->ultima].tipo_l());
				claveBien = true;
				mensaje = "Activar";
				return mensaje;
			}
			else if (reloj->H[reloj->ultima].tipo_l() == "ONV") 
			{
				modo_survey = 1;
				claveBien = true;
				mensaje = "ActivarV";
				return mensaje;
			}
			if (reloj->H[reloj->ultima].tipo_l() == "OFF") 
			{
				envio = 1;
				claveBien = true;
				estado = 0;
				mensaje = "Desactivar";
				return mensaje;
			}
			if (reloj->H[reloj->ultima].tipo_l() == "OFV") 
			{
				envio = 1;
				claveBien = true;
				estado = 0;
				modo_survey = 0;
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
		claveBien = false;
		tiempoEnvio = millis();
	}
	else if (claveMal)
	{
		estado = 5;
		claveMal = false;
	}
}

String AlarmaC::enEstado1()
{
	String mensaje = "";
	if (millis()> tiempoEnvio + tiempoEsperaSonar) {
		estado = 2;
		valorRepAlarma = 0;
		mensaje = "Activado el";
			return mensaje;
	}
	delay(100);
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
		claveMal = false;
	}
	else if (estado != 0)
	{
		estado = 4;
		tiempo = 0;
	}
}

String AlarmaC::enEstado4()
{
	String mensaje = "";
	if (tiempo < millis() && (repeticion < maxRepeticion))
	{
		mensaje = "Sonando";
		tiempo = millis() + tiempoEntreCorreos;
		repeticion++;
		return mensaje;
	}

	if (claveBien)
	{
		estado = 0;
		claveBien = false;
	}
	else if (claveMal)
	{
		repeticion = 0;
		tiempo = 0;
		estado = 6;
		claveMal = false;
	}
}

void AlarmaC::enEstado5()
{
	if (claveBien)
	{
		estado = 1;
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