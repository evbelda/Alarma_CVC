// Hora.h

#ifndef _HORA_h
#define _HORA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Hora {
	private:
		String tipo;
		int dias[7] = { -1,-1,-1,-1,-1,-1,-1 };
		int hor;
		int minuto;
	public:
		String temp;
		void guardar();
		void leer();
		int hora_l();
		int minuto_l();
		boolean esdia(int d);
		String dias_leer();
		String tipo_l();
};

#endif

