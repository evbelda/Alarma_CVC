#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "RTC.h"   


char BotToken[50] = "352056115:AAFRNjyshZNlKjcyAaDqzDpezn77YK6x0bM";    // your Bot Token;
char BOTname[50] = "AlarmaCasa";
char BOTusername[50] = "@evbeldabot";
char ssid[50] = "MOVISTAR_63EA";
char pwd[50] = "vB9v9VFSgdNzJe7HBSMw";

int maxRepeticion = 3, tiempoAlarmaEncendida = 300000, tiempoDeteccion = 4500, tiempoEsperaSonar= 20000, tiempoEntreCorreos=120000;
int N_Dispositivo = 1;
String secretCode = "1234"; // Variable que contiene el número PIN.
String secretCode2 = "12345678"; // Variable que contiene el número PUK.
int N_Desactivacion=0;
long vigia = 0;
String vigia_tiempo;
long Bot_mtbs = 4500; //mean time between scan messages
//long Bot_lasttime = 0;   //last time messages' scan has been done
long tiempoTel; // Variable para guardar tiempo.
long tiempo = 0;
long Bot_envio=0;
String chat_id="";
int numNewMessages=0;

int PIRPin = D0; 
WiFiClientSecure client2;
UniversalTelegramBot bot(BotToken, client2);

String readString=String(50);
int setVal = 0, modo_survey = 0;
int  alarmaPIR = 0, envio = 0;
int estado = 0; // Variable donde se almacena el estado de la alarma.
boolean claveBien = false; // Variable para verificar si la clave es correcta.
boolean claveMal = false; // Variable para verificar si la clave es errónea.

long tiempoEnvio, valorRepAlarma = 0;
int repeticion = 0;

//---------------------------------------Reloj-----------------------------------
RTC rtc(DST_ON);
String daysOfTheWeek[7] = { "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" , "Domingo"};
//String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
Data date;
//---------------------------------------fin inicializacion Reloj-----------------------------------

//--------------------------------------Clase Registro de Hora-----------------------------------------
class Hora{
  private:
   String tipo; 
   int dias[7]={-1,-1,-1,-1,-1,-1,-1};
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

int Hora::hora_l(){
  return hor;
}

int Hora::minuto_l(){
  return minuto;
}

boolean Hora::esdia(int d){

  for (int i= 0; i<7;i++){
    if (dias[i] == d){
      return true;
    }
  }
  return false;
}

String Hora::dias_leer(){
  //return String(dias[0]);
  return String(dias[0])+","+ String(dias[1])+","+ String(dias[2])+","+String(dias[3])+","+ String(dias[4])+","+ String(dias[5])+","+String(dias[6]);
}

String Hora::tipo_l(){
  return tipo;
}

void Hora::guardar(){
    char charTemp[14];
    String inString;
    int j;
    tipo= temp.substring(temp.indexOf("=")+1,temp.indexOf(";"));  
    temp = temp.substring(temp.indexOf(";")+1);
    hor = temp.substring(0,temp.indexOf(":")).toInt();
    temp = temp.substring(temp.indexOf(":")+1);
    minuto = temp.substring(0,temp.indexOf(";")).toInt();
    temp = temp.substring(temp.indexOf(";")+1);
    temp.toCharArray(charTemp, temp.length()+1);
    j=0;
    for (int k=0; k < 13;k++){
      if (isDigit(charTemp[k])){
        //Serial.println("dentroq" + String(charTemp[i]));
        inString = (char)charTemp[k];
        //dias[j] = charTemp[i]-'0';
        dias[j] = inString.toInt();
        //Serial.println("dentroq " + String(charTemp[k]) + String(dias[j]));
        j++;
      }
    }  
}

void Hora::leer(){
    Serial.println("La alarmaprogramada es:");
    Serial.println("Es de tipo: " +  tipo + " a las " + String(hor) + ":" +String(minuto));
    Serial.println("Salta los dias: " + String(dias[0]) + String(dias[1])+String(dias[2])+String(dias[3])+ String(dias[4])+String(dias[5])+String(dias[6]));  
}

Hora H[6];
//-------------------------------------------Finde Hora-----------------------------------------------------



void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Accediendo al Modo de Configuración");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//flag para guardar datos
bool shouldSaveConfig = false;

void saveConfigCallback () {
  Serial.println("Debería grabar la configuracion");
  shouldSaveConfig = true;
}

int alarmas;
int modo_alarma = 1;
int ultima;
File myFile;
const int chipSelect = D8;


void setup() {
  
  // Open serial communications and wait for port to open:

  Serial.begin(9600);

   while (!Serial) {

    ; // wait for serial port to connect. Needed for Leonardo only

  }

  WiFiManager wifiManager;

  //start-block2
  /*int last_ip= 65+N_Dispositivo;
  IPAddress _ip = IPAddress(192,168,1,last_ip);
  IPAddress _gw = IPAddress(192, 168, 1, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);*/
  //end-block2
  
  //wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  wifiManager.setAPCallback(configModeCallback); //Cuando la conexión al WiFi falla, se entra en el modo AP
  wifiManager.setConnectTimeout(10); // Se da un tiempo de 10 segundos para iniciar la conexion a la red WiFi
  wifiManager.autoConnect("Local_modulo" + N_Dispositivo); // Nombre de la red WiFi creada por la ESP8266-01
  
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  // Conexión a la red wifi exitosa
  Serial.println("Conectado de forma exitosa a la red WiFi");

 //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
 
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  pinMode(PIRPin, INPUT);
  pinMode(SS, OUTPUT); 
  //----------Ponemos el reloj en Hora si ha perdido la corriente
  //rtc.setDateTime( 2018, 04, 12, 18, 06, 00 ); // Year, Month, Day, Hour24H, Minutes and Seconds
  Serial.println("La hora es: " + ConvierteTiempo());
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  leerTarjeta(0);
  for (int k; k < alarmas;k++){
      H[k].leer();
  }

  proxima_alarma();
  if (ultima !=-1){
    Serial.println("La proxima alarma es");
    Serial.println("Alarma tipo:" + H[ultima].tipo_l() + " " + String(H[ultima].hora_l())+ ":"+ String(H[ultima].minuto_l()));
    Serial.println("Dia: " + H[ultima].dias_leer());
  }
  else{
    Serial.println("No ha alarma detectada");
  }
}

void loop() {
 while(estado == 0)
  {
    if (modo_alarma == 1){
      if(comprueba_Alarma()){
        Serial.println("El tipo de alarma es: " + H[ultima].tipo_l());
        if (H[ultima].tipo_l() == "ON"){
          Serial.println("Se activa la alarma en ON: " + H[ultima].tipo_l());
          bot.sendMessage(chat_id, "Activar","");
          claveBien = true;
          
        }
        else if (H[ultima].tipo_l() == "ONV"){
          bot.sendMessage(chat_id, "Activar","");
          modo_survey = 1;
          claveBien = true;
          bot.sendMessage(chat_id, "Activando modo vigia","");
        }
      }
    }
    
    telegramServidor();
    repeticion=0;
    if (envio == 1){
      claveBien = false;
      envio = 0;
    }
    if(claveBien)
    {
      estado = 1;
      //Serial.print("1");
      claveBien = false;
      tiempoEnvio = millis();
    }
    else if(claveMal)
    {
      estado = 5;
      //Serial.print("5");
      claveMal = false;
    }
    delay(1);
  }
/*
//// En el siguiente estado (estado '1'), la alarma acaba de activarse, pero no estará operativa hasta que pasen 20 segundos ////
En este estado, el programa realizará de forma secuencial las siguientes acciones:
- El programa espera 20 segundos.
- Posteriormente, el programa pasa a estado '2'.
- Notifica el cambio a través de Telegram.
*/
  while(estado == 1)
  {
    if (millis()> tiempoEnvio+tiempoEsperaSonar){
      estado = 2;
      //Serial.print("2"); 
      valorRepAlarma = 0;
      bot.sendMessage(chat_id, " Activado el " + String(N_Dispositivo) +" .","");
      vigia = 0;
    }
    delay(100);
  }
/*
//// En el siguiente estado (estado '2') la alarma está operativa e inspecciona si un intruso ha entrado en casa ////
La detección consta de dos modos:
- Modo normal: Si detecta un intruso con el PIR pasa a estado 3
- Modo Survey: Detecta el numero de veces que pasa el intruso por delante y registra la hora
En este estado, el programa realizará de forma secuencial las siguientes acciones:
- Verificar si el sensor pir han detectado algún intruso.
- Realizar una segunda comprobación para evitar errores de medición del pir
- Notificar por Telegram que hay un posible intruso
- Pasar a estado 3
*/
  while(estado == 2)
  { 
    if (modo_alarma == 1){
      if(comprueba_Alarma()){
        if (H[ultima].tipo_l() == "OFF"){
          bot.sendMessage(chat_id, "Desactivar","");  
          envio = 1;
          claveBien = true;
          estado = 0;
          bot.sendMessage(chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");          
        }
        if (H[ultima].tipo_l() == "OFV"){
          bot.sendMessage(chat_id, "Desactivar","");  
          envio = 1;
          claveBien = true;
          estado = 0;
          modo_survey = 0;
          bot.sendMessage(chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");
          bot.sendMessage(chat_id, "Vigia desactivado dispositivo " + String(N_Dispositivo) +".", "");
        }
      }
    }
    
    telegramServidor();
     if (modo_survey == 0){
      if(digitalRead (PIRPin) == HIGH){
        if (valorRepAlarma !=1){  
          tiempoEnvio = millis();
          Serial.println("No lo cambies por dios");
          valorRepAlarma = 1;
        }
        Serial.println("Dentro de pir1: "+ String(valorRepAlarma)+ "-" + String(tiempoEnvio));
      }
     if (millis()>tiempoEnvio+tiempoDeteccion){
       Serial.println("Dentro de pir2: "+ String(valorRepAlarma)+ "-" + String(tiempoEnvio));
       if((digitalRead (PIRPin) == HIGH && valorRepAlarma == 1))
       {
          estado = 3;
          Serial.println("Dentro de pir3");
          bot.sendMessage(chat_id, "Que tienes a alguien en casa!!!! Estado 3. Dispositivo " + String(N_Dispositivo) +" .","");
        delay(1000);
        }
        else
          valorRepAlarma = 0;
      }
      
    }
    else{
      if(digitalRead (PIRPin) == HIGH){
        if (valorRepAlarma !=1){  
          tiempoEnvio = millis();
          valorRepAlarma = 1;
        }
      }
      if (millis()>tiempoEnvio+tiempoDeteccion){
          if((digitalRead (PIRPin) == HIGH && valorRepAlarma == 1))
          {
            vigia_tiempo = ConvierteTiempo();
            vigia++;
            delay(100);
          }
          else
            valorRepAlarma = 0;            
      }   
    }
  }
/*
//// En el siguiente estado (estado '3') la alarma acaba de detectar un intruso y pondrá en funcionamiento la sirena (si se pone) si en 20 segundos no se introduce el código PIN ////
En este estado y durante 20 segundos, el programa realizará de forma secuencial las siguientes acciones:
- Espera al codigo de desactivacion de Telegram (clave) para desactivar la alarma antes de que pase al estado 4 o 6
- Verificar si el número marcado es el correcto (claveBien o claveMal).
- Si la claveMal esta a true pasa al estado 6 en el que hay que poner el PUK
- Si la claveBien esta a true pasa al estado 0 y desactiva la alarma
- Si no se pone ninguna clave pasa al estado 4
Después de la verificación del número marcado, el programa irá al estado que le corresponda y notificará el cambio a la aplicación de c# enviando un dato a través del puerto serie.
*/
  while(estado == 3)
  {
    tiempo = millis() + tiempoEsperaSonar;
    do
    {
      telegramServidor();
      if(claveBien || claveMal)
      {
        break;
      }
    }while(tiempo > millis());
    valorRepAlarma = millis();
    if(claveBien)
    {
      estado = 0;
      claveBien = false;
    }
    else if(claveMal)
    {
      estado = 6;
      //Serial.print("6");
      claveMal = false;
    }
    else if (estado!= 0)
    {
      estado = 4;
      tiempo = 0;
      //Serial.print("4");
    }
  }
/*
//// En el siguiente estado (estado '4') la sirena de la alarma (si se conecta) está en funcionamiento y se necesita el código PIN para detener la alarma ////
En este estado, el programa realizará de forma secuencial las siguientes acciones:
- Comprobar si se ha marcado enviado algún comando por Telegram para desactivar la alarma
- Verificar si el PIN que devuelve el comando "Desactivar xxxx" es correcto (claveBien o claveMal).
Después de la verificación del número marcado, el programa irá al estado que le corresponda y notificará el cambio a la aplicación de c# enviando un dato a través del puerto serie.
*/

  while(estado == 4)
  {
    //Serial.println(String(tiempo)+"-" + String(millis())+ "repeticion:"+ String(repeticion));
    telegramServidor();
      if(tiempo < millis()&& (repeticion < maxRepeticion))
      {
       bot.sendMessage(chat_id, "Que te está sonando la alarma!!!! Estado 4. Dispositivo " + String(N_Dispositivo) +".","");
       tiempo = millis() + tiempoEntreCorreos;
       repeticion++;
      }
    
    if(claveBien)
    {
      estado = 0;
      claveBien = false;
    }
    else if(claveMal)
    {
      repeticion = 0;
      tiempo = 0;
      estado = 6;
      claveMal = false;
    }
  }
/*
//// En el siguiente estado (estado '5') el código PIN está bloqueado y se necesita el código PUK para activar la alarma ////
Este estado no se ha trabajado todavia. Nunca se llega a estado 5 pero deja para futuras versiones
Después de la verificación del número marcado, el programa irá al estado que le corresponda y notificará el cambio a la aplicación de c# enviando un dato a través del puerto serie.
*/

  while(estado == 5)
  {
    //telegramServidor();
    if(claveBien)
    {
      estado = 1;
      //Serial.print("1");
      claveBien = false;
    }
  }
/*
//// En el siguiente estado (estado '6') el código PIN está bloqueado y se necesita el código PUK para desactivar la alarma ////
En este estado, el programa realizará de forma secuencial las siguientes acciones:


- Comprobar si se ha enviado algún comando por Telegram.
- Verificar si el número marcado PUK es el correcto (claveBien).
Después de la verificación del número marcado, el programa irá al estado que le corresponda y notificará el cambio a la aplicación de c# enviando un dato a través del puerto serie.
*/

  while(estado == 6)
  {
    telegramServidor();  
    if(tiempo < millis()&& (repeticion < maxRepeticion))
    {
        tiempo = millis() + 20000;
       repeticion++;
    }
    if(claveBien)
    {
    estado = 0;
    Serial.print("0");
    claveBien = false;
    }
  }         
}

/*
 Funcion de comunicacion con Telegram que detecta los comandos que vienen en los mensajes en modo text
 Los comandos pueden ser:
 - Activar (activa la alarma)
 - Desactivar (desactiva la alarma)
 - Estado (comprueba el estado del dispositivo)
 - Vigia activar (activa el modo vigia)
 - Vigia desactivar (desactiva el modo vigia)
 - Vigia estado (indica el valor de registro de movimiento y la hora del ultimo movimiento)
 - Vigia reset (resetea el contador de movimiento y el tiempo)
*/

void telegramServidor(){

 int Orden_Dispositivo = 0; // variable sirve para indicar si este dispositivo debe responder en el canal de Telegram
 char cadenaMensaje[50]; //cadena temporal para subtividir los mensajes de Telegram
 String mensaje;
 //Serial.println("Estado: " + String(estado) + "-Chat_id: " + chat_id + " numMensajes: " + String(numNewMessages));

 if (tiempoTel < millis())  {
  numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while(numNewMessages) {
      for (int i=0; i<numNewMessages; i++){
          Orden_Dispositivo = 0;
          Serial.println("Mensaje nuevo");
          bot.messages[i].text.toCharArray(cadenaMensaje,50);
          Serial.println(cadenaMensaje);
          char * item = strtok (cadenaMensaje, " ");
            if (String(item) == "Estado") 
            {
              item = strtok (NULL, " ");
              while (item != NULL)
              {
                Orden_Dispositivo = 1;
                if (String(item) == String(N_Dispositivo)){
                  bot.sendMessage(bot.messages[i].chat_id, "El estado es: " + String(estado) + ".Dispositivo " + String(N_Dispositivo) +".", "");
                }
                item = strtok (NULL, " ");
              }
              if (Orden_Dispositivo == 0){
                  bot.sendMessage(bot.messages[i].chat_id, "El estado es: " + String(estado) + ". Dispositivo " + String(N_Dispositivo) +".", "");               
              }
            }
            if (String(item) == "Activar") 
            {
              item = strtok (NULL, " ");
              while (item != NULL)
              {
                Orden_Dispositivo = 1;
                if (String(item) == String(N_Dispositivo)){
                  Serial.println("Activando el " + N_Dispositivo);
                  if (estado == 0){
                    claveBien = true;
                  }
                  else if(estado == 5){
                    claveBien = true;
                    claveMal = false;
                    //Serial.println("Dentro de poner el puk estado 5 > 3");
                  }
                  //envio = 1;
                  estado = 1;
                  bot.sendMessage(bot.messages[i].chat_id, "Se esta activando. Espera 20 segundos... Dispositivo " + String(N_Dispositivo) +".",""); 
                }
                item = strtok (NULL, " ");  
              }
              if (Orden_Dispositivo == 0){
                if (estado == 0){
                    claveBien = true;
                }
                else if(estado == 5){
                    claveBien = true;
                    claveMal = false;
                    //Serial.println("Dentro de poner el puk estado 5 > 3");
                }
                //envio = 1;
                estado = 1;
                bot.sendMessage(bot.messages[i].chat_id, "Se esta activando. Espera 20 segundos... Dispositivo " + String(N_Dispositivo) +".","");
              }
            }
            if (String(item) == "Desactivar") 
            {
              Serial.println("Dentro de desactivar");
              item = strtok (NULL, " ");
              if (estado == 2){
                if (String(item) == secretCode){                            
                    item = strtok (NULL, " ");
                    while (item != NULL)
                    {
                      Orden_Dispositivo = 1;
                      if (String(item) == String(N_Dispositivo)){                
                        Serial.println("Dentro de desactivar disp");
                        envio = 1;
                        claveBien = true;
                        estado = 0;
                        bot.sendMessage(bot.messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");
                      }
                      item = strtok (NULL, " ");
                    }
                    if (Orden_Dispositivo == 0){
                      Serial.println("Dentro de desactivar disp");
                      envio = 1;
                      claveBien = true;
                      estado = 0;
                      bot.sendMessage(bot.messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");                   
                    }
                } 
                
              }
              else{
                  if ((estado == 3 || estado == 4) && (N_Desactivacion<3)){
                    if (String(item) == secretCode){ 
                      Serial.println("Desactivar alarma");
                      envio = 1;
                      claveBien = true;
                      estado = 0;
                      bot.sendMessage(bot.messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");                                         
                      N_Desactivacion = 0;
                    }
                  }
                  else{
                    estado = 6;
                    if (String(item) == secretCode2){ 
                      Serial.println("Desactivar alarma");
                      envio = 1;
                      claveBien = true;
                      estado = 0;
                      bot.sendMessage(bot.messages[i].chat_id, "Alarma desactivada. Dispositivo " + String(N_Dispositivo) +".", "");                                      
                      N_Desactivacion = 0;
                    }
                  }
                     
              }
            }
            if (String(item) == "Vigia") 
            {
              item = strtok (NULL, " ");
              if (String(item) == "activar"){             
                modo_survey = 1;
                bot.sendMessage(bot.messages[i].chat_id, "Vigia activado dispositivo " + String(N_Dispositivo) +": "+ String(vigia), "");
              }
              else if (String(item) == "desactivar"){
                modo_survey = 0;
                bot.sendMessage(bot.messages[i].chat_id, "Vigia desactivado dispositivo " + String(N_Dispositivo) +".", "");
              }
              else if (String(item) == "reset"){
                vigia = 0;
                bot.sendMessage(bot.messages[i].chat_id, "Vigia reiniciado dispositivo " + String(N_Dispositivo) +": "+ String(vigia), "");                
              }
              else if (String(item) == "estado")
                bot.sendMessage(bot.messages[i].chat_id, "Vigia dispositivo " + String(N_Dispositivo) +": "+ String(vigia) + "." + vigia_tiempo, "");                                                     
            }
            if (String(item) == "Horas") 
            {
              item = strtok (NULL, " ");
              if (String(item) == "listado"){
                bot.sendMessage(bot.messages[i].chat_id, "Las alarmas existentes son:", "");
                for (int j = 0; j< alarmas; j++){
                  bot.sendMessage(bot.messages[i].chat_id, H[j].tipo_l() + " hora: " + String(H[j].hora_l()) + ":" + String(H[j].minuto_l()) + " dias: " + String(H[j].dias_leer()), "");
                }
              }
              else if (String(item) == "activar"){ 
                leerTarjeta(1);
                proxima_alarma();
                modo_alarma = 1;
                bot.sendMessage(bot.messages[i].chat_id, "Se han activado las alarmas automaticas.", "");
              }
              else if (String(item) == "leer"){ 
                leerTarjeta(1);
                proxima_alarma();
                //modo_alarma = 1;
                bot.sendMessage(bot.messages[i].chat_id, "Releyendo alarma de tarjeta.", "");
              }
              else if (String(item) == "desactivar"){
                modo_alarma = 0;
                bot.sendMessage(bot.messages[i].chat_id, "Se han desactivado la automatización de la alarma.", "");
              }
              else if (String(item) == "estado"){
                if (modo_alarma == 1){
                  bot.sendMessage(bot.messages[i].chat_id, "Proxima hora: tipo " + H[ultima].tipo_l() +" hora: " + String(H[ultima].hora_l()) + ":" + String(H[ultima].minuto_l()) + " dias: " + String(H[ultima].dias_leer()), "");                                                     
                  //Data date = rtc.getData();
                  mensaje.concat(date.hour24h);
                  mensaje.concat(":");
                  mensaje.concat(date.minutes);
                  bot.sendMessage(chat_id, "La hora actual es: " + mensaje, "");
                }
                else{
                  bot.sendMessage(bot.messages[i].chat_id, "La automatizacion de la alarma está desactivada.","");  
                }
              }
            }            
            chat_id = bot.messages[i].chat_id;
            //Serial.println("El chat id es: " + String(chat_id));                    
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  }
  tiempoTel = millis()+Bot_mtbs;
 }
 else
  tiempoTel = millis();
}


/*
 * Funcion que devielve en una cadena la hora actual del reloj
 */
String ConvierteTiempo(){
  String temporal="Fecha: ";
  date = rtc.getData();
  //date = rtc.now();
  //time_t t = now();
  temporal.concat(date.year);
  temporal.concat("/");
  temporal.concat(date.month);
  temporal.concat("/");
  temporal.concat(date.day);
  temporal.concat("(");
  temporal.concat(daysOfTheWeek[date.dayWeek-1]);
  temporal.concat(")");
  temporal.concat(date.hour24h);
  temporal.concat(":");
  temporal.concat(date.minutes);
  temporal.concat(":");
  temporal.concat(date.seconds);
  return  String(temporal);
}

void leerTarjeta(int tipo){
  int i;
  String cadenatemp;
  String cadena[15];
  alarmas = 0;
  ultima = -1;

  for (i = 0;i<15;i++){
    cadena[i] = "";
  }

  /*cadena[0] = "H1=ON;18:35;1,2,3,4,5";
  cadena[1] = "H2=OFF;18:37;1,2,3,4,5";
  cadena[2] = "H3=ON;12:00;3";
  cadena[3] = "H4=OFF;15:00;3";
  cadena[4] = "H5=ONV;8:10;3";
  cadena[5] = "H6=OFV;11:59;3";*/
  
  //---------------------buscamos el archivo y lo leermos
  myFile = SD.open("datos.txt");
  Serial.println("Leemos el fichero:" + String(myFile));
  
  if (myFile){

    Serial.println("datos.txt:");

    // read from the file until there's nothing else in it:
    i = 0;
    while (myFile.available()) {

      char caracter =myFile.read();
      cadena[i] = cadena[i] +caracter;
      if ((caracter == 10) && (i <15)){
        i++;
      }
    }
    //myFile.close();
    Serial.println("La tarjeta leida es");
    for (int j=0;j<=i;j++){
      Serial.println(cadena[j]);
    }
    // close the file:
    //leerTarjeta(0);
  } else {

    // if the file didn't open, print an error:

    Serial.println("error opening datos.txt");

  }
  myFile.close();
  //-------------una vez leido lo almacenamos en cadena--------
  
  for (int j=0; j < 15; j++){
    
    //Serial.println(cadena[j]);
    if(tipo == 0){
      if (cadena[j].indexOf("BotToken")>=0){     
        cadenatemp = cadena[j].substring(cadena[j].indexOf("=")+1);
        //Serial.println(cadenatemp);
        cadenatemp.toCharArray(BotToken, cadenatemp.length()+1);
        //Serial.println(BotToken);
      }
      if (cadena[j].indexOf("BOTname")>=0){
        cadenatemp=cadena[j].substring(cadena[j].indexOf("=")+1);
        cadenatemp.toCharArray(BOTname, cadenatemp.length()+1);
        //Serial.println(BotToken);
        //Serial.println(BOTname);  
      }
      if (cadena[j].indexOf("BOTusername")>=0){
        cadenatemp=cadena[j].substring(cadena[j].indexOf("=")+1);
        cadenatemp.toCharArray(BOTusername, cadenatemp.length()+1);
      }
      if (cadena[j].indexOf("SSID")>=0){
        cadenatemp=cadena[j].substring(cadena[j].indexOf("=")+1);
        cadenatemp.toCharArray(ssid, cadenatemp.length()+1);
      }
      if (cadena[j].indexOf("PWD")>=0){
        cadenatemp=cadena[j].substring(cadena[j].indexOf("=")+1);
        cadenatemp.toCharArray(pwd, cadenatemp.length()+1);
      }
    }
    if ((tipo == 1) || (tipo == 0)){
      if (cadena[j].indexOf("H1=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }
      if (cadena[j].indexOf("H2=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }
      if (cadena[j].indexOf("H3=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }
      if (cadena[j].indexOf("H4=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }  
      if (cadena[j].indexOf("H5=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }
      if (cadena[j].indexOf("H6=")>=0){
        H[alarmas].temp = cadena[j];
        H[alarmas].guardar();
        alarmas++;
        Serial.println("dentro de  alarmas"+cadena[j]);
      }
    }       
  }
}

void proxima_alarma(){
    int hora_sel=24*60*7;
    int hora_temp;
    int hora_actual;
    int dia_actual=0;
    int ultima_llamada=0;
    ultima =-1;
    //DateTime date = rtc.now();
    date = rtc.getData();
    dia_actual = date.dayWeek;
    Serial.println("Hora actual: dia: " + String(dia_actual) + " hora: "+ String(date.hour24h) + " minuto: " +String(date.minutes));

    hora_actual = date.hour24h*60+date.minutes+dia_actual*24*60;
    while ((ultima == -1) && (dia_actual<7)){
      //Serial.println("Dia actual" + String(dia_actual) + "hora actual " + String(hora_actual) + " Alarmas: " + String(alarmas));
      for (int i =0;i<alarmas;i++){
          if (H[i].esdia(dia_actual)){
            //Serial.println("Estamos dentro del dia:" + String(dia_actual));
            hora_temp = dia_actual*24*60+H[i].hora_l()*60+H[i].minuto_l(); 
            //Serial.println("Estamos dentro del dia:" + String(dia_actual));           
            if ((hora_actual < hora_temp) && (hora_temp <= hora_sel)){
              ultima = i;
              hora_sel = hora_temp;
              //Serial.println("Dentro de la busqueda: " + String(hora_temp));
            }
            //Serial.println("Dentro de la busqueda: " + hora_temp);
          }
          else{
            hora_temp =24*60*7;
          }
      }
      dia_actual++;
      if ((dia_actual == date.dayWeek-2)&& (ultima == -1) && (ultima_llamada == 0)){
        ultima_llamada = 1;
        //hora_actual = (-1+dia_actual)*24*60+date.hour()*60+date.minute();
        //dia_actual = 0;
        //hora_actual = -1*24*60+date.hour()*60+date.minute();
      }
      else if (dia_actual == 7){
        dia_actual = 0;
        hora_actual = (-1+dia_actual)*24*60+date.hour24h*60+date.minutes;
      }
      else if (ultima_llamada == 1){
        dia_actual = 7;
      }
      //Serial.println("Estamos en: " + String(ultima) + "-" + String(hora_sel) + "-hora act: " + String(hora_actual));
    }
}


boolean comprueba_Alarma(){
  Serial.println("La hora es: " + ConvierteTiempo());
  //date = rtc.getData();
  /*if (H[ultima].esdia(date.dayWeek)){
    Serial.println("Detecta dia.");
  }
  if (H[ultima].hora_l() == date.hour24h*1){
    Serial.println("Detecta hora." + H[ultima].hora_l());
  }  
  if (H[ultima].minuto_l() == date.minutes*1){
    Serial.println("Detecta minutos." + H[ultima].minuto_l());
  }*/  
  if (H[ultima].esdia(date.dayWeek) && (H[ultima].hora_l() == date.hour24h*1) && (H[ultima].minuto_l() == date.minutes*1)){
    return true;  
  }
  else{
    return false;
  }
}


