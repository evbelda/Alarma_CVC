# AlarmaAlarma_WebMos_CVC

Proyecto Arduino de alarma WIFI escalable El proyecto pretende los siguientes objetivos

    Alarma basada en dispositivo Arduino WEBMOS D1 R2 y sensor PIR detector de movimiento
    Código escalable para poder incorporar hasta 4 dispositivos WEBMOS dependientes de un controlador principal
    La alarma consta de dos modos: normal y vigia que detecta movimiento y guarda el nº de veces que lo detecta y la hora
    La activación y desactivación se realiza a traves de un BOT de Telegram o desde la programación de seis fechas guardadas en una SD card
    La configuración se lee de una tarjeta SD conectada al controlador principal
    Se puede programar hasta 6 fechas de activación y desactivación de la alarma automáticamente registradas en la configuración de la SD
    Controlador principal consta de Reloj, lector SD y sensor PIR (funciona también como detector)
