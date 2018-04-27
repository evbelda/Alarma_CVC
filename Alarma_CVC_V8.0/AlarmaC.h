// AlarmaC.h

#ifndef _ALARMAC_h
#define _ALARMAC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <UniversalTelegramBot.h>
	#include "RelojAlarma.h"
	#include "RTC.h"
#else
	#include "WProgram.h"
#endif

#define N_Dispositivo 0
#define PIRPin D0
#define chipSelect D8

class AlarmaC
{
private:
	char BotToken[50] = "352056115:AAFRNjyshZNlKjcyAaDqzDpezn77YK6x0bM";    // your Bot Token;
	char BotName[50] = "AlarmaCasa";
	char BotUsername[50] = "@evbeldabot";
	char Ssid[50] = "MOVISTAR_63EA";
	//char Pwd[50] = "vB9v9VFSgdNzJe7HBSMw";
	char Pwd[50] = "";
public:
	String leer(String cadena);
	String escribir(String cadena);
	String activaAlarma(UniversalTelegramBot bot, RelojAlarma *reloj, RTC rtc);
	void enEstado0();
	void enEstado3();
	void enEstado5();
	void enEstado6();
	int estado;
	bool claveBien = false;
	bool claveMal = false;
	String secretCode = "1234";
	String secretCode2 = "12345678";
	int envio = 0;
	int  alarmaPIR = 0;
	int tiempo;
	int setVal = 0;
	long tiempoEnvio;
	long valorRepAlarma = 0;
	int repeticion = 0;
	int maxRepeticion = 3, tiempoAlarmaEncendida = 300000, tiempoDeteccion = 4500, tiempoEsperaSonar = 20000, tiempoEntreCorreos = 120000;
	int modo_survey = 0;
	int modo_alarma = 1;
};


#endif

