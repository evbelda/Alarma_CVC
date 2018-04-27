// 
// 
// 

#include "RelojAlarma.h"
#include "Hora.h"	
#include "RTC.h"

RelojAlarma::RelojAlarma()
{
	;
}

String RelojAlarma::ImprimeDia(int dia) {
	if (dia != -1){
		return daysOfTheWeek[dia];
	}
	else
		return "";
	
}

String RelojAlarma::ConvierteTiempo(RTC rtc) {
	String temporal = "";
	date = rtc.getData();
	temporal.concat(date.year);
	temporal.concat("/");
	temporal.concat(date.month);
	temporal.concat("/");
	temporal.concat(date.day);
	temporal.concat("(");
	temporal.concat(daysOfTheWeek[date.dayWeek - 1]);
	temporal.concat(")");
	temporal.concat(date.hour24h);
	temporal.concat(":");
	temporal.concat(date.minutes);
	temporal.concat(":");
	temporal.concat(date.seconds);
	return  String(temporal);
}

void RelojAlarma::ProximaAlarma(RTC rtc) {
	int hora_sel = 24 * 60 * 7;
	int hora_temp;
	int hora_actual;
	int dia_actual = 0;
	int ultima_llamada = 0;
	ultima = -1;
	//DateTime date = rtc.now();
	date = rtc.getData();
	dia_actual = date.dayWeek;
	Serial.println("Hora actual: dia: " + String(dia_actual) + " hora: " + String(date.hour24h) + " minuto: " + String(date.minutes));

	hora_actual = date.hour24h * 60 + date.minutes + dia_actual * 24 * 60;
	while ((ultima == -1) && (dia_actual<7)) {
		//Serial.println("Dia actual" + String(dia_actual) + "hora actual " + String(hora_actual) + " Alarmas: " + String(alarmas));
		for (int i = 0; i<alarmas; i++) {
			if (H[i].esdia(dia_actual)) {
				//Serial.println("Estamos dentro del dia:" + String(dia_actual));
				hora_temp = dia_actual * 24 * 60 + H[i].hora_l() * 60 + H[i].minuto_l();
				//Serial.println("Estamos dentro del dia:" + String(dia_actual));           
				if ((hora_actual < hora_temp) && (hora_temp <= hora_sel)) {
					ultima = i;
					hora_sel = hora_temp;
					//Serial.println("Dentro de la busqueda: " + String(hora_temp));
				}
				//Serial.println("Dentro de la busqueda: " + hora_temp);
			}
			else {
				hora_temp = 24 * 60 * 7;
			}
		}
		dia_actual++;
		if ((dia_actual == date.dayWeek - 2) && (ultima == -1) && (ultima_llamada == 0)) {
			ultima_llamada = 1;
			//hora_actual = (-1+dia_actual)*24*60+date.hour()*60+date.minute();
			//dia_actual = 0;
			//hora_actual = -1*24*60+date.hour()*60+date.minute();
		}
		else if (dia_actual == 7) {
			dia_actual = 0;
			hora_actual = (-1 + dia_actual) * 24 * 60 + date.hour24h * 60 + date.minutes;
		}
		else if (ultima_llamada == 1) {
			dia_actual = 7;
		}
		//Serial.println("Estamos en: " + String(ultima) + "-" + String(hora_sel) + "-hora act: " + String(hora_actual));
	}
}

boolean RelojAlarma::CompruebaAlarma(RTC rtc) {
	//Serial.println("La hora es: " + ConvierteTiempo(rtc));

	if (H[ultima].esdia(date.dayWeek) && (H[ultima].hora_l() == date.hour24h * 1) && (H[ultima].minuto_l() == date.minutes * 1)) {
		return true;
	}
	else {
		return false;
	}
}

