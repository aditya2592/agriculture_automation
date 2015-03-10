char byteRead;
int val = 0; //this variable will read the value from the sensor
#define RELAY1  4                        
#define RELAY2  5                       
#define RELAY3  6                        
#define RELAY4  7
#define RELAY5  8
#define RELAY6  9

volatile int pulsecount = 0;
int ledPin = 3;
int inputPin = 0; //Pin number 2 for proximity switch
unsigned int pulse;
//unsigned long timeold;

void pulse_fun()
 {
      pulsecount++;
      digitalWrite(ledPin, HIGH);
      delay(50);
      digitalWrite(ledPin, LOW);
 }

boolean toggle1 = 0;
ISR(TIMER1_COMPA_vect){
  if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
  }
}
int time_steps = 0;
void setup() {                
  Serial.begin(9600);
  
  digitalWrite(13, LOW);
  pinMode(inputPin, INPUT); //declare infrared sensor as input
  pinMode(ledPin, OUTPUT);
  pinMode(RELAY1, OUTPUT);       
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(RELAY5, OUTPUT);
  pinMode(RELAY6, OUTPUT);
  
   cli();
  pinMode(13, OUTPUT);   
  
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 7816;// = (16*10^6) / (2*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //attachInterrupt(0, pulse_fun, FALLING);
  sei();//allow interrupts
  
  for(int i = 0; i < 6; i++)
  {
    {
       int pin = i+4;
       digitalWrite(pin,LOW);
       delay(100);
    }
  }
  
}
char output[6];
void turn_off(int nummax, char level)
{
  for(int i = 0; i < nummax; i++)
  {
    if(output[i] == level)
    {
        
       digitalWrite(i+4,LOW);
    }
  }
}
int count = 0;
void loop() 
{
  pulsecount = 5;
  //Serial.write("s");
  if (Serial.available()) 
  {
    byteRead = Serial.read();
    
    
    output[count] = byteRead;
    
    
    count = count + 1;
    if(count == 6)
    {
      if(pulsecount > 4)
      {
         for(int j = 0; j < 5; j++)
         {
              int nummax = 6;
              for(int i = 0; i < nummax; i++)
              {
                //Serial.write(output[i]);
                if(output[i] != '0')
                {
                   int pin = i+4;
                   digitalWrite(pin,HIGH);
                }
              }
              delay(50); 
              turn_off(nummax, '1');
              delay(50); 
              turn_off(nummax, '2');
              delay(50); 
              turn_off(nummax, '3');
              delay(50); 
              turn_off(nummax, '4');
                
          }
      
          pulsecount = 0;
      }
      else
      {
          for(int i = 0; i < 6; i++)
          {

            digitalWrite(i+4, LOW);
          
          }  
      }
      count = 0;
    }
    //Serial.write(byteRead);
  }
  
}
