char byteRead;
int val = 0; //this variable will read the value from the sensor
#define RELAY1  4                        
#define RELAY2  5                       
#define RELAY3  6                        
#define RELAY4  7
#define RELAY5  8
#define RELAY6  9

volatile int pulsecount = 5;
int ledPin = 3;
int inputPin = 2; //Pin number 2 for proximity switch
unsigned int pulse;
volatile int time_steps = 0;

char output[6];
int count = 0; //to count number of bytes coming in. 6 at a time, one for each camera

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
 
    time_steps = time_steps + 1;
    for(int i = 0; i < 6; i++)
      {
        //Serial.write(output[i]);
        if(output[i] == '1' && time_steps == 1)
        {
            
           digitalWrite(i+4,LOW);
        }
        else if(output[i] == '2'  && time_steps == 2)
        {
           digitalWrite(i+4,LOW);
        }
        else if(output[i] == '3'  && time_steps == 3)
        {
           digitalWrite(i+4,LOW);
        }
        else if(output[i] == '4'  && time_steps == 4)
        {
           digitalWrite(i+4,LOW);
           
        }
        
      }
      if(time_steps == 4)
      {
        time_steps = 0;
        TCNT1  = 0;
        TIMSK1 |= (0 << OCIE1A);
      }
  
}
void setup() {                
  Serial.begin(9600);
  
  //pinMode(inputPin, INPUT); //declare infrared sensor as input
  pinMode(ledPin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(RELAY1, OUTPUT);       
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(RELAY5, OUTPUT);
  pinMode(RELAY6, OUTPUT);
  cli();
  
  
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
  //TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  
  
  
}

void loop() 
{
  
  
  if (Serial.available()) 
  {
    byteRead = Serial.read();
    Serial.write(byteRead);
    
    output[count] = byteRead;
    
    count = count + 1;
    
    if(count == 6)
    {
        Serial.write("ok\n");
        for(int i = 0; i < 6; i++)
        {
          Serial.write(output[i]);
          if(output[i] != '0')
          {
             int pin = i+4;
             digitalWrite(pin,HIGH);
          }
        }
        delay(2000);
        count = 0;
        TCNT1  = 0;
        TIMSK1 |= (1 << OCIE1A);
        
    }
    
    
  }
}
