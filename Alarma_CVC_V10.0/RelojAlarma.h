// RelojAlarma.h

#ifndef _RELOJALARMA_h
#define _RELOJALARMA_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#include "Hora.h"	
#include "RTC.h"
#else
#include "WProgram.h"
#endif

class RelojAlarma
{
private:
	String daysOfTheWeek[7] = { "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" , "Domingo" };
	String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
	Data date;
public:
	RelojAlarma();
	String ImprimeDia(int dia);
	String ConvierteTiempo(RTC rtc);
	void ProximaAlarma(RTC rtc);
	boolean CompruebaAlarma(RTC rtc);
	int CambiaHora(RTC *rtc, char *cadena);
	Hora H[6];
	int ultima;
	int alarmas;
};
#endif


