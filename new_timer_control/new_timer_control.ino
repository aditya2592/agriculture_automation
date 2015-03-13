char byteRead;
int val = 0; //this variable will read the value from the sensor
#define RELAY1  4                        
#define RELAY2  5                       
#define RELAY3  6                        
#define RELAY4  7
#define RELAY5  8
#define RELAY6  9

#define CYCLE_MAX 5  //number of cycles of 4 levels

volatile int pulsecount = 5;
int ledPin = 3;
int inputPin = 2; //Pin number 2 for proximity switch
unsigned int pulse;
int time_steps = 0;

char output[6];
int count = 0; //to count number of bytes coming in. 6 at a time, one for each camera
char current[6];
boolean toggle1 = 0;
int cycle_count = 0;

void pulse_fun()
{
  pulsecount++;
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
}





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
    if(current[i] == '1' && (time_steps % 4) == 1)
    {

      digitalWrite(i+4,LOW);
    }
    else if(current[i] == '2'  && (time_steps % 4) == 2)
    {
      digitalWrite(i+4,LOW);
    }
    else if(current[i] == '3'  && (time_steps % 4) == 3)
    {
      digitalWrite(i+4,LOW);
    }


  }
  if(time_steps == CYCLE_MAX * 4)
  {
    //new cyclec will start according to new output
    time_steps = 0;
    for(int i = 0; i < 6; i++)
    {
      current[i] = output[i];
    }
    for(int i = 0; i < 6; i++)
    {
      if(current[i] != '0')
      {
        digitalWrite(i+4,HIGH);
      }
    }
    //TCNT1  = 0;
    //TIMSK1 |= (0 << OCIE1A);
  }
  if((time_steps % 4) == 0 && time_steps != CYCLE_MAX * 4)
  {
    //repeat same cycle
    for(int i = 0; i < 6; i++)
    {
      if(current[i] != '0')
      {
        digitalWrite(i+4,HIGH);
      }
    }
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
  OCR1A = 781;// = (16*10^6) / (2*1024) - 1 (must be <65536) //500ms = 7816
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

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
       Serial.write("\n");
      /*for(int i = 0; i < 6; i++)
       {
       Serial.write(output[i]);
       if(output[i] != '0')
       {
       digitalWrite(i+4,HIGH);
       }
       }*/

      count = 0;
      //TCNT1  = 0;
      //TIMSK1 |= (1 << OCIE1A);

    }


  }
}



