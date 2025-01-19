#include <Arduino.h>
// Global variable
volatile unsigned int x1 = 0, x2 = 0, n = 0;  
volatile bool capture_flag = false;         
volatile bool display_flag = false;
volatile bool calculate_flag = false; 
volatile bool toggle_state = false;           
volatile unsigned int overflow_counter = 0;    

float f = 0.0, S = 0.0;      
int P = 3;            
long F = 16000000;    
int prescaler = 64;   
const unsigned int MAX_TIMER_COUNT = 2500;

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
        display_flag = 1;  
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
        calculate_flag = 1;
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(7,OUTPUT);
    digitalWrite(7,LOW);
    TCCR1A = 0;
    TCCR1B = (0 << ICES1) | (1 << CS11) | (1 << CS10);
    TCNT1 = 63035;
    TIMSK1 = (1 << TOIE1) | (1 << ICIE1);          
    sei();
}

void loop()
{
    static unsigned int t1, t2, overflow_count;
   // static bool calculate_flag = false;
    if (calculate_flag == 1) 
    {
        calculate_flag = 0;
        t1 = x1; 
        t2 = x2;
        overflow_count = n;
        unsigned long deltaTCNT1 = (overflow_count * MAX_TIMER_COUNT) + t2 - t1;
        f = (float)F / (prescaler * deltaTCNT1);
        S = (60.0 * f) / P;
    }

    if (display_flag == 1)
    {
        display_flag = 0;
        Serial.print("x1 = ");
        Serial.print(x1);
        Serial.print("; ");
        Serial.print("x2 = ");
        Serial.print(x2);
        Serial.print("; ");
        Serial.print("n = ");
        Serial.print(n);
        Serial.print("; ");
        Serial.print("Frequency: ");
        Serial.print(f, 2);
        Serial.print(" Hz, ");
        Serial.print("Speed: ");
        Serial.print(S, 2);
        Serial.println(" RPM");
    }
}
