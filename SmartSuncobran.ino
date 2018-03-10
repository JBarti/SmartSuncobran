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


int s1StepsPerRevolution=2048;  
int stepCounter = 0;
int s1p1=8, s1p2=9, s1p3=10, s1p4=11;
int s1pin = A0;
Stepper s1Stepper(s1StepsPerRevolution, s1p1, s1p2, s1p3, s1p4);
FotoSenzor s1(350, s1pin);
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


int m1p1;
Motor motor(m1p1);
int temperature = 15;
Event tempEvent;
int ledpin;
void onHighTemp()
{
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


int s2s3StepsPerRevolution;
int s2pin, s3pin;
int s2p1, s2p2, s2p3, s2p4;
FotoSenzor s2(120, s2pin);
FotoSenzor s3(120, s3pin);
Stepper s2s3Stepper(s2s3StepsPerRevolution, s2p1, s2p2, s2p3, s2p4);
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
  s1.event.call();

}
