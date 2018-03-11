#include <DHT.h>
#include<Stepper.h>

class Event
{
	public:
		void onEvent( void (*func)() )
		{
		 this->func = func;
		}

    void call()
    {
      this->func();
    }

	private:
		void (*func)();
};


class Motor
{
  public:
    Motor(int pin1)
    {
      this->pin1 = pin1;
      pinMode(pin1, OUTPUT);
    }

    void fwd()
    {
      digitalWrite(pin1, HIGH);
    }

    void stp()
    {
      digitalWrite(pin1, LOW);
    }
  
  private:
    int pin1;
};


class FotoSenzor
{
	public:
		FotoSenzor(int dayNight, int pin)
		{
      pinMode(A0, INPUT);
      this->pin = pin;
			this->dayNight = dayNight;
		}

		void setEvent(void (*func)())
		{
      this->event.onEvent(func);
		}

   bool isNight()
   {
    return analogRead(this->pin) >= dayNight;
   }

   bool isDay()
   {
    return analogRead(this->pin) <= dayNight;
   }

   int value()
    {
      return (analogRead(A0)); 
    }

   Event event;
	
	private:
    int pin;
		int dayNight;
};

//#############################
//PALJENJE I GAŠENJE SUNCOBRANA
//#############################
int s1StepsPerRevolution=2048; //BROJ TAKTOVA PO REVOLUCIJI MOTORA
int stepCounter = 0;  //OVO NE DIRAJ
int s1p1=8, s1p2=9, s1p3=10, s1p4=11; //PINOVI STEP MOTORA
int s1pin = A0; //PIN FOTOOTPORNIKA
Stepper s1Stepper(s1StepsPerRevolution, s1p1, s1p2, s1p3, s1p4); //STEPPER MOTOR
FotoSenzor s1(350, s1pin);  //OVO NE DIRAJ
void s1Func()
{
  Serial.println(s1.value());
  if(s1.isDay() && stepCounter != 1)
  {
    Serial.print("UPALI SUNCOBRAN");
    Serial.print(s1.value());
    Serial.println();
    s1Stepper.setSpeed(20);
    s1Stepper.step(s1StepsPerRevolution);
    stepCounter = 1;
  }
  else if(s1.isNight() && stepCounter != -1)
  {
    Serial.print("UGASI SUNCOBRAN");
    Serial.print(s1.value());
    Serial.println();
    s1Stepper.setSpeed(10);
    s1Stepper.step(-s1StepsPerRevolution);
    stepCounter = -1;
  }
}


//###########
//TEMPERATURA
//###########
int dhtpin; //PIN DHT SENZORA
DHT dht(dhtpin, "DHT11"); //NE DIRAJ
int m1p1; //PIN DC MOTORA
int ledpin; //PIN LEDICE
Motor motor(m1p1);  //NE DIRAJ
Event tempEvent; //NE DIRAJ
void onHighTemp()
{
  float temperature = dht.readTemperature();
  if(temperature >= 27)
  {
    motor.fwd();
    digitalWrite(ledpin, HIGH);
  }
  else
  {
    motor.stp();
    digitalWrite(ledpin, LOW);
  }
}



//#####################
//OKRETANJE PREMA SUNCU
//#####################
int s2s3StepsPerRevolution; //BROJ TAKTOVA PO REVOLUCIJI STEPER motORA
int s2pin, s3pin;  //PINOVI PRVOG I DRUGOG FOTOOTPORNIKA
int s2p1, s2p2, s2p3, s2p4; //PINOVI STEPPER MOTORA
FotoSenzor s2(350, s2pin);  //NE DIRAJ
FotoSenzor s3(350, s3pin);  //NE DIRAJ
Stepper s2s3Stepper(s2s3StepsPerRevolution, s2p1, s2p2, s2p3, s2p4); //NE DIRAJ
void s2Func()
{
  if(s2.value() > s3.value())
  {
    s1Stepper.setSpeed(50);
    s1Stepper.step(s2s3StepsPerRevolution/2);
  }
}

void s3Func(){
  if(s3.value() > s2.value())
  {
    s1Stepper.setSpeed(50);
    s1Stepper.step(-(s2s3StepsPerRevolution/2));
  }  
}

void setup()
{ 
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  s1.setEvent(s1Func);
  s2.setEvent(s2Func);
  s3.setEvent(s3Func);
}

void loop()
{
  s1.event.call();  //PALJENJE I GAŠENJE SUNCOBRANA
  tempEvent.call(); //VENTILATOR
  s2.event.call();  //OKRETANJE PREMA SUNCU
    s3.event.call();  //OKRETANJE PREMA SUNCU

}
