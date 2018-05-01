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
		Serial.println("Dia actual" + String(dia_actual) + "hora actual " + String(hora_actual) + " Alarmas: " + String(alarmas)+"-"+String(ultima_llamada)+"--"+String(ultima));
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
		/*if ((dia_actual == date.dayWeek - 2) && (ultima == -1) && (ultima_llamada == 0)) {
			ultima_llamada = 1;
			//hora_actual = (-1+dia_actual)*24*60+date.hour()*60+date.minute();
			//dia_actual = 0;
			//hora_actual = -1*24*60+date.hour()*60+date.minute();
		}*/
		/*else*/
		if ((dia_actual == 7) && (ultima_llamada == 0)){
			dia_actual = 0;
			hora_actual = (-1 + dia_actual) * 24 * 60 + date.hour24h * 60 + date.minutes;
			ultima_llamada = 1;
		}
		else if ((ultima_llamada == 1) && (dia_actual == 7)) {
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

int RelojAlarma::CambiaHora(RTC *rtc, char * cadena)
{
	int longit = 0;
	String dia, mes, anho;
	String hora, minuto;
	char *p, *i;
	Data fecha;

	Serial.println("lo que viene: " + String(cadena) + " " + String(strlen(cadena)));
	cadena = strtok(NULL, " ");
	Serial.println("la primera parte: " + String(cadena) + " " + String(strlen(cadena)));
	if ((cadena[2] == '/') && (cadena[5] == '/') && (cadena[10] == '\0'))
	{
		p = strtok_r(cadena, "/", &i);
		if ((strlen(p) != 2) || !(isDigit(p[0])) || !(isDigit(p[1]))) {
			longit = 1;
		}
		dia = String(p);
		Serial.println("frase: " + String(p) + " " + String(strlen(p)));
		p = strtok_r(NULL, "/", &i);
		if ((strlen(p) != 2) || !(isDigit(p[0])) || !(isDigit(p[1]))) {
			longit = 1;
		}
		mes = String(p);
		Serial.println("frase2: " + String(p) + " " + String(strlen(p)));
		p = strtok_r(NULL, " ", &i);
		if ((strlen(p) != 4) || !(isDigit(p[0])) || !(isDigit(p[1])) || !(isDigit(p[2])) || !(isDigit(p[3]))) {
			longit = 1;
		}
		anho = String(p);
		Serial.println("frase3: " + String(p) + " " + String(strlen(p)));
		cadena = strtok(NULL, " ");
		Serial.println("Segunda parte de la frase:" + String(cadena));
		if ((cadena[2] == ':') && (cadena[5] == '\0'))
		{
			p = strtok_r(cadena, ":", &i);
			if ((strlen(p) != 2) || !(isDigit(p[0])) || !(isDigit(p[1]))) {
				longit = 1;
			}
			hora = String(p);
			Serial.println("frase4: " + String(p) + " " + String(strlen(p)));
			p = strtok_r(NULL, ":", &i);
			if ((strlen(p) != 2) || !(isDigit(p[0])) || !(isDigit(p[1]))) {
				longit = 1;
			}
			minuto = String(p);
			Serial.println("frase5: " + String(p) + " " + String(strlen(p)));
			if (longit == 0) {
				Serial.println("Fecha correcta");
				fecha.year = strtol(anho.c_str(), NULL, 0);
				fecha.month = strtol(mes.c_str(), NULL, 0);
				fecha.day = strtol(dia.c_str(), NULL, 0);
				fecha.hour = strtol(hora.c_str(), NULL, 0);
				fecha.minutes = strtol(minuto.c_str(), NULL, 0);
				rtc->setDateTime(fecha.year, fecha.month, fecha.day, fecha.hour, fecha.minutes, 0);
				return 0;
			}
			else {
				Serial.println("Numeros incorrectos");
				return 1;
			}
		}
		else {
			Serial.println("Formato Hora incorrecta");
			return 2;
		}
	}
	else {
		Serial.println("Formato Fecha incorrecto");
		return 3;
	}
}