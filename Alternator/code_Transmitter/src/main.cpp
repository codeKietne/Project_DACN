#include <Arduino.h>
#define D10 10
#define D8 8
#define PIN A0
const int P = 3;
int value;            
float dutyCycle;      
int pwmValue;         
float frequency;      
int digital_value; 
float S;
void setup() {
    Serial.begin(9600);
    pinMode(D10, OUTPUT);
    pinMode(D8, OUTPUT);
}

void loop() {
    
    value = analogRead(PIN); 
    dutyCycle = map(value, 0, 1023, 0, 100);  
    frequency = map(value, 0, 1023, 0, 500);  
    pwmValue = map(dutyCycle, 0, 100, 0, 255); 
    
	S = (60 * frequency) / P; 
    analogWrite(D10, pwmValue);  // Để hiển thị tín hiệu từ biến trở
    tone(D8, (int)frequency);    // Xuất thêm 1 xung để đo tần số khi thay đổi biến trở
	
    Serial.print("Duty Cycle: ");
    Serial.print(dutyCycle);
    Serial.print("% | Frequency: ");
    Serial.print(frequency);
    Serial.print(" Hz | Speed: ");
    Serial.print(S);
    Serial.println(" RPM");
    delay(500);
}
