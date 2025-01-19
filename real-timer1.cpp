#include <Arduino.h>
#define D10 10
#define D8 8
#define PIN A0
volatile unsigned int x1 = 0, x2 = 0, n = 0;  
volatile bool capture_flag = false;         
volatile bool display_flag = false;
volatile bool calculate_flag = false; 
volatile bool toggle_state = false;           
volatile unsigned int overflow_counter = 0;
const int P = 3;
long F = 16000000;    
int prescaler = 64;   
const unsigned int MAX_TIMER_COUNT = 2500;  
float S;      
int digital_value; 
int pwmValue;
float dutyCycle;
int analog_value;
float output_frequency = 0;
float measured_frequency = 0; 

ISR(TIMER1_OVF_vect)
{
    n++;
    TCNT1 = 63035;
    overflow_counter++; 
    toggle_state = !toggle_state;
    digitalWrite(7, toggle_state);
    if (overflow_counter > 100)  
    {
        overflow_counter = 0;  
        display_flag = true;  
    }
}

ISR(TIMER1_CAPT_vect)
{
    static unsigned int prev_n = 0;       
    if (!capture_flag) 
    {
        x1 = ICR1; 
        prev_n = n;       
        capture_flag = true;
    } 
    else 
    {
        x2 = ICR1; 
        n = n - prev_n;    
        capture_flag = false;
        calculate_flag = true;
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    pinMode(D10, OUTPUT);
    pinMode(D8, OUTPUT);
    TCCR1A = 0; 
    TCCR1B = (0 << ICES1) | (1 << CS11) | (1 << CS10);
    TCNT1 = 0;
    TIMSK1 = (1 << TOIE1) | (1 << ICIE1);
    sei();
}

void loop() 
{
    analog_value = analogRead(PIN); 
    dutyCycle = map(analog_value, 0, 1023, 0, 100);  
    output_frequency = map(analog_value, 0, 1023, 10, 1000);
    pwmValue = map(analog_value, 0, 1023, 0, 255);
    analogWrite(D10, pwmValue);  
    tone(D8, (int)output_frequency, 500); 
    static unsigned int t1, t2, overflow_count;

    if (calculate_flag == true) 
    {
        calculate_flag = false;
        t1 = x1; 
        t2 = x2;
        overflow_count = n;
        unsigned long deltaTCNT1 = (overflow_count * MAX_TIMER_COUNT) + t2 - t1;
        measured_frequency = (float)F / (prescaler * deltaTCNT1);
        S = (60.0 * measured_frequency) / P;
    }
    
    if (display_flag == true)
    {
        display_flag = false;
        Serial.print("Duty Cycle = ");
        Serial.print(dutyCycle);
        Serial.print("%; PWM Value = ");
        Serial.print(pwmValue);
        Serial.print("; Output Frequency = ");
        Serial.print(output_frequency, 2);
        Serial.println(" Hz");
        Serial.print("x1 = ");
        Serial.print(x1);
        Serial.print("; x2 = ");
        Serial.print(x2);
        Serial.print("; n = ");
        Serial.print(n);
        Serial.print("; Measured Frequency = ");
        Serial.print(measured_frequency, 2);
        Serial.print(" Hz; Speed = ");
        Serial.print(S, 2);
        Serial.println(" RPM.");
        Serial.println("--------------------");
    }
}
