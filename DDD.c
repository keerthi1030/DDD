#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdint.h>
#include<util/delay.h>
#define SET(PORT,BIT1,BIT2) PORT|=((1<<BIT1)|(1<<BIT2))
#define CLEAR(PORT,BIT1,BIT2) PORT&=~((1<<BIT1)|(1<<BIT2))
#define CLR_BIT(PORT,BIT) PORT&=~(1<<BIT)
#define SET_BIT(PORT,BIT) PORT|=(1<<BIT)
#define ENGINE_LED PB2
#define DROWSY_LED  PD3
#define adcpin 0
#define adcpin1 1
struct{
  volatile unsigned int FLAG1:1;
  volatile unsigned int FLAG2:1;
}FLAG_BIT;
uint16_t value=0x0000;
uint16_t val=0x0000;
int main()
{
  Serial.begin(9600);
  SET(DDRD,PD7,PD6);
  CLEAR(DDRD,PD2,PD3);
  CLEAR(PORTD,PD7,PD6);
  SET_BIT(DDRB,PB2);
  SET_BIT(DDRB,PB3);
  
  EICRA|=(1<<ISC00);
  EIMSK|=(1<<INT0);
  sei();
  FLAG_BIT.FLAG1=0;
  FLAG_BIT.FLAG2=0;
  //SET_BIT(PORTB,PB2);
  while(1)
  {
    
    while(FLAG_BIT.FLAG1==1)
    {
      SET_BIT(PORTB,PB2);
      _delay_ms(1000);
      ADCSRA|=(1<<ADEN);
      cli();
      val=adc_read1(adcpin1);
         if (val >= 0 && val < 768)  //Normal mode 

        { 
           SET_BIT(PORTB,PB2);
           PORTB|=(1<<ENGINE_LED);
           PORTB&=~(1<<DROWSY_LED); 

         } 

         else if (val > 768 && val <= 1023)  //drowziness mode 

         { 

          PORTB|=(1<<DROWSY_LED);
           CLR_BIT(PORTB,PB2);
          //PORTB&=~(1<<ENGINE_LED);
           Serial.println("LEDDD");
          } 

      value=adc_read(adcpin);
      
      //Serial.println(value);
          if(value>200)
         {
            TCCR0B|=((1<<CS02)|(1<<CS00)); 
            OCR0B=100;
            PORTD|=(1<<PD7);
            Serial.println("a");
          }
          else if(value<=200)
         {
            TCCR0B=0x00;
            PORTD&=~(1<<PD7);
            CLR_BIT(PORTB,PB2);
            Serial.println("b");
         }
     
      sei();
    }
CLR_BIT(PORTB,PB2);
CLR_BIT(PORTB,PB3);
CLR_BIT(PORTD,PD7);
CLR_BIT(PORTD,PD6);
  }
}

uint16_t adc_read1(uint8_t adc)
{
  ADMUX = adc;
  ADMUX |= (1<<REFS0);
  ADMUX|=(adc & 0x0f);
  ADCSRA|=(1<<ADSC);
    while(ADCSRA &(1<<ADSC))
  {
  }
  return ADC;
}

uint16_t adc_read(uint8_t adc)
{
  ADMUX = adc;
  ADMUX |= (1<<REFS0);
  ADMUX|=(adc & 0x0f);
  ADCSRA|=(1<<ADSC);
    while(ADCSRA &(1<<ADSC))
  {
  }
  return ADC;
}

ISR(INT0_vect)
{
  cli();
  if(FLAG_BIT.FLAG1==0)
  {
   
FLAG_BIT.FLAG1=1;
   
TCNT0=0x00;
  
TCCR0A=0x00;
  
TCCR0B|=((1<<CS02)|(1<<CS00));  
  
TIMSK0|=((1<<OCIE0A)|(1<<OCIE0B));
  
sei();
  }
  else if(FLAG_BIT.FLAG1==1)
  {
    FLAG_BIT.FLAG1=0;
    TCCR0B=0x00;
  }
}
  
ISR(TIMER0_COMPA_vect)
{
  PORTD|=(1<<PD6);
}
  
ISR(TIMER0_COMPB_vect)
{
  PORTD&=~(1<<PD6);
}

