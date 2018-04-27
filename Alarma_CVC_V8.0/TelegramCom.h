// TelegramCom.h

#ifndef _TELEGRAMCOM_h
#define _TELEGRAMCOM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <WiFiClientSecure.h>
	#include <UniversalTelegrambot.h>
	#include "RelojAlarma.h"
	#include "AlarmaC.h"
	#include "LectorSD.h"
	#include "RTC.h"

#else
	#include "WProgram.h"
#endif


#endif

class TelegramCom
{
private:
	char BotToken[50] = "352056115:AAFRNjyshZNlKjcyAaDqzDpezn77YK6x0bM";    // your Bot Token;
	char BOTname[50] = "AlarmaCasa";
	char BOTusername[50] = "@evbeldabot";
	long Bot_mtbs = 4500; //mean time between scan messages
	long tiempoTel; // Variable para guardar tiempo.
	long Bot_envio = 0;
	int numNewMessages = 0;
public:
	void TelegramServidor(RelojAlarma *reloj, AlarmaC *alcObj, LectorSD *objSD, RTC *rtc, UniversalTelegramBot *bot);
	void chequeoPIRSurvey(RTC rtc, RelojAlarma reloj, AlarmaC *alcObj);
	void chequeoPIRNormal(UniversalTelegramBot bot, AlarmaC *alcObj);
	String chat_id = "";
	long vigia = 0;
	String vigia_tiempo;
};