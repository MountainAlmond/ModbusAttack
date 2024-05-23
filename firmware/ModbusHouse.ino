//RS-485 Modbus Slave (Arduino UNO)
//Circuit Digest
#include<ModbusRtu.h>       //Библиотека для использования Modbus RTU в режиме master-slave
#include "DHT.h"
#include "Ultrasonic.h"  
//#include<LiquidCrystal.h>   //Библиотека для экранчика (опционально)

#include <Servo.h>          //Library for using Servo Motor
#define led1 2              //Уличное освещение 
#define led2 3
#define PIN_TRIG 8
#define PIN_ECHO 7
#define ALARM 9
#define DHTPIN 6               
Servo servo;                          //Initilize servo object for class Servo
Modbus bus;                          //Define Object bus for class modbus
DHT dht(DHTPIN, DHT11);             //Инициация датчика влажности
Ultrasonic ultrasonic(8,7);

int8_t state = 0; 
uint16_t pint[12];    //первоначально в массив записываем нулевые значения

                      
void setup()
{
    dht.begin();
    pint[6]=0;

  
  pinMode(led1,OUTPUT);           //Led1 на вывод данных
  pinMode(led2,OUTPUT);           //Led2 на вывод данных
  pinMode(ALARM,OUTPUT);          //Светодиод, отвечающий за сигнализацию

  servo.attach(5);                //Servo PWM pin 5 (контакт, к которому подключен сервомотор ШИМ)
  bus = Modbus(1,1,4);            //Modbus slave ID as 1 and 1 connected via RS-485 and 4 connected to DE & RE pin of RS-485 Module
//адрес ведомого равен 1, используется связь через интерфейс RS-485 (вторая 1), 4 – номер контакта Arduino, к которому подключены контакты DE & RE модуля RS-485
  bus.begin(9600);                //Modbus slave baudrate at 9600 (скорость 9600 бод)
}
void loop()
{  // Запрос slave девайса и обработка сообщений
   state = bus.poll(pint,sizeof(pint)/sizeof(pint[0]));       //Записываем и читаем массив pint (master-slave)
   float h = dht.readHumidity(); //Измеряем влажность
   float t = dht.readTemperature(); //Измеряем температуру
   int dist = ultrasonic.distanceRead();
   pint[5]=dist;
   if (pint[1] == 0)    //если в scada нажата кнопка уличного освещения
  {
    digitalWrite(led1,LOW);    //выключаем первый фонарь
    digitalWrite(led2,LOW);    //выключаем второй фонарь

  }
  else
  {
     digitalWrite(led1,HIGH);  // включаем первый фонарь
     digitalWrite(led2, HIGH); // 
       
  }
  
   if(pint[7]!=0)
   {
     
    if(dist>20&&pint[6]==0)
    {
        digitalWrite(ALARM,HIGH);
        delay(10);
        digitalWrite(ALARM,LOW);
        delay(10);    
    }
    else
      {
        digitalWrite(ALARM,HIGH);
        pint[6]=1;
      }
   
      
   }   
   else
   {
    digitalWrite(ALARM,LOW);
    pint[6]=0;
   }
     
  int pwm = pint[2]; //Depends upon value in modbus_array[2] written by Master Modbus
  servo.write(pwm);          // поворачиваем сервомотор на угол pwm (от 0 до 180 градусов), принятый  от Modbus Master
  pint[3]=h;
  pint[4]=t;
}
