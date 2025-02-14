#include <Arduino.h>
int CKP_count =0 ;
int CMP_count =0 ;
int n ;
int count_ADC ;
bool CKP_array[72]=
{0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0} ;
bool
CMP_array[144]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0 };
unsigned int ne ;
const float _CLK_PRE = 16000000.0/64 ;
float _adc_value, _volt_value ;
float F_ne ;
float F_tooth ;
float F_INTERRUPT ;
bool f_ADC ;
bool f_CKP_array;
bool f_CMP_array;

void setup()
{
Serial.begin(9600);
pinMode(4, OUTPUT);
pinMode(7, OUTPUT);
cli();
//setup timer 0
TCCR0A = 0;
TCCR0B = 0;
TCNT0=0;
TIMSK0 = 0;

TCCR0B |= (1 << CS01 )|(1 << CS00) ;
TCCR0A |= (1 << WGM01) | (1 << COM0A1);
OCR0A = (unsigned short int)(16000000/64/1000.0) -1 ;
TIMSK0 |= (1 << OCIE0A) ;

TCCR1A = 0;
TCCR1B = 0;
TCNT1=0;
TIMSK1 = 0;

TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
TCCR1A |= (1 << WGM11) | (1 << COM1A1)| (1 << COM1A0) ;
TIMSK1 |= (1 << TOIE1);
ICR1 = 249 ;
sei();
}

void loop()
{
if(f_ADC==1)
{
f_ADC = 0;
_volt_value = map(analogRead(A0), 0 , 1023 , 1000, 5000 );
ne = map(_volt_value , 1000 , 5000 , 600 , 6000 );
F_ne = ne/60 ; //round per sencond
F_tooth = F_ne*36 ; //tan so của CKP
F_INTERRUPT = F_tooth*2 ;
ICR1 = (unsigned short int)( _CLK_PRE/F_INTERRUPT ) -1 ;
Serial.print( _volt_value/1000.0);
Serial.print( " " );
Serial.print( ne );
Serial.print( " " );
Serial.println(F_tooth);
}
 }


ISR(TIMER0_COMPA_vect)
{
if(++ count_ADC == 50 )
{
f_ADC = 1 ;
count_ADC = 0;
}
}

ISR(TIMER1_OVF_vect)
{
f_CKP_array = CKP_array[CKP_count];
++ CKP_count ;
if(CKP_count == 72 )
{CKP_count = 0; }
digitalWrite( 4, f_CKP_array );
f_CMP_array = CMP_array[CMP_count];
++ CMP_count ;
if( CMP_count == 144 )
{CMP_count = 0; }
digitalWrite ( 7, f_CMP_array );}