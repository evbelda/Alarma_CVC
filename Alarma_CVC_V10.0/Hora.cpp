// 
// 
// 

#include "Hora.h"

int Hora::hora_l() {
	return hor;
}

int Hora::minuto_l() {
	return minuto;
}

boolean Hora::esdia(int d) {

	for (int i = 0; i<7; i++) {
		if (dias[i] == d) {
			return true;
		}
	}
	return false;
}

String Hora::dias_leer() {
	//return String(dias[0]);
	return String(dias[0]) + "," + String(dias[1]) + "," + String(dias[2]) + "," + String(dias[3]) + "," + String(dias[4]) + "," + String(dias[5]) + "," + String(dias[6]);
}

String Hora::tipo_l() {
	return tipo;
}

void Hora::guardar() {
	char charTemp[14];
	String inString;
	int j;
	tipo = temp.substring(temp.indexOf("=") + 1, temp.indexOf(";"));
	temp = temp.substring(temp.indexOf(";") + 1);
	hor = temp.substring(0, temp.indexOf(":")).toInt();
	temp = temp.substring(temp.indexOf(":") + 1);
	minuto = temp.substring(0, temp.indexOf(";")).toInt();
	temp = temp.substring(temp.indexOf(";") + 1);
	temp.toCharArray(charTemp, temp.length() + 1);
	j = 0;
	for (int k = 0; k < 13; k++) {
		if (isDigit(charTemp[k])) {
			//Serial.println("dentroq" + String(charTemp[i]));
			inString = (char)charTemp[k];
			//dias[j] = charTemp[i]-'0';
			dias[j] = inString.toInt();
			//Serial.println("dentroq " + String(charTemp[k]) + String(dias[j]));
			j++;
		}
	}
}

void Hora::leer() {
	Serial.println("La alarmaprogramada es: ");
	Serial.println("Es de tipo: " + tipo + " a las " + String(hor) + ":" + String(minuto));
	Serial.println("Salta los dias: " + String(dias[0]) + String(dias[1]) + String(dias[2]) + String(dias[3]) + String(dias[4]) + String(dias[5]) + String(dias[6]));
}

