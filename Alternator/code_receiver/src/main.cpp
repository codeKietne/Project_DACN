#include <Arduino.h>
volatile uint32_t x1 = 0, x2 = 0, n = 0;  
volatile bool capture_flag = false;         
volatile bool display_flag = false;
volatile bool calculate_flag = false; 
volatile bool toggle_state = false;           
volatile uint32_t overflow_counter = 0;    

float f = 0.0, S = 0.0;      
int P = 3;            
volatile uint32_t overflow_count = 0;  
volatile uint32_t elapsed_seconds = 0; 
long F = 16000000;    
int prescaler = 64;   
const unsigned int MAX_TIMER_COUNT = 2500;

ISR(TIMER1_OVF_vect)
{
    n++;
    overflow_count++; 
    TCNT1 = 63035;    
    toggle_state = !toggle_state;
    digitalWrite(7, toggle_state);

    if (overflow_count % 100 == 0) {
        elapsed_seconds++;
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

void setup()
{
    Serial.begin(9600);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    TCCR1A = 0;
    TCCR1B = (0 << ICES1) | (1 << CS11) | (1 << CS10);
    TCNT1 = 63035;
    TIMSK1 = (1 << TOIE1) | (1 << ICIE1);          
    sei();
}

void loop()
{
    static uint32_t last_x2 = 0; 
    volatile uint32_t t1, t2, overflow_count_snapshot;

    if (calculate_flag == true) 
    {
        calculate_flag = false;
        t1 = x1; 
        t2 = x2;
        overflow_count_snapshot = n;
        unsigned long deltaTCNT1 = (overflow_count_snapshot * MAX_TIMER_COUNT) + t2 - t1;
        f = (float)F / (prescaler * deltaTCNT1);
        S = (60.0 * f) / P;
        x1 = x2;
    }

    if (display_flag == true)
    {
        display_flag = false;
        Serial.print("So lan tran sau ");
        Serial.print(elapsed_seconds);
        Serial.print(" giay: ");
        Serial.println(overflow_count);
        
        Serial.print("x1 = ");
        Serial.print(last_x2); 
        Serial.print("; ");
        Serial.print("x2 = ");
        Serial.print(x2);
        Serial.print("; ");
        Serial.print("Frequency: ");
        Serial.print(f, 2);
        Serial.print(" Hz, ");
        Serial.print("Speed: ");
        Serial.print(S, 2);
        Serial.println(" RPM");
        last_x2 = x2;
    }
}
