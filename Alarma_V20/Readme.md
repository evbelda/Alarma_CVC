Version de la alarma que funciona con los siguientes componentes:
- PIR
- Reloj DS3231
- Arduino

Las librerias usadas y comprobada la compatibilidad:
- WifiManager
- UniversalTelegramBot
- ESP8266Wifi
- RTClib.h // para el reloj

NOTA: para que funcionara Telegram con WifiManager fue necesario quitar el codigo que asigna IP estatica para el dispositivo. Necesario poner un Issue en esta libreria
