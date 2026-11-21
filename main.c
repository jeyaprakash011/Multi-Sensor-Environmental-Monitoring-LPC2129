
#include "header.h"

#define WATER_PIN     (1<<5)
#define GREEN_LED     (1<<0)
#define YELLOW_LED    (1<<1)
#define RED_LED       (1<<2)

float temperature;
unsigned int ldr_value, soil_value,adc_value;
int soil_percent, water_status;

void lcd_display(void){
    
	lcd_cmd(0x80);
    lcd_string("TEMP:");
    lcd_integer((int)temperature);
    lcd_string("C ");

    lcd_string("SOIL:");
    lcd_integer(soil_percent);
    lcd_string("% ");

    lcd_cmd(0xC0);
    lcd_string("LIGHT:");

    if(ldr_value > 600)
        lcd_string("NORMAL ");

    else
        lcd_string("LOW    ");
}

void uart_display(void){
    
	uart0_tx_string("\r\nTemperature: ");
    uart0_int((int)temperature);

    uart0_tx_string(" C\r\nLight ADC: ");
    uart0_int(ldr_value);

    uart0_tx_string("\r\nLight: ");

    if(ldr_value > 600)
        uart0_tx_string("NORMAL");

    else
        uart0_tx_string("LOW");

    uart0_tx_string("\r\nSoil Moisture: ");
    uart0_int(soil_percent);
    uart0_tx_string("%");

    uart0_tx_string("\r\nWater Sensor: ");

    uart0_tx_string(water_status ? "DETECTED" : "NOT DETECTED");
    uart0_tx_string("\r\n");
}

int main(void)
{
    
    IODIR0 |= GREEN_LED|YELLOW_LED|RED_LED;
    IOSET0 = GREEN_LED|YELLOW_LED|RED_LED; 
		adc_init();
    uart0_init(9600);
    lcd_init();
    lcd_cmd(0x0C);

    while(1)
    {
				adc_value = adc_read(0);
				temperature =((adc_value * 3.3f) / 1023.0f) * 100.0f;
				
			ldr_value = adc_read(1);
			soil_value = adc_read(2);
			
    soil_percent = ((800 -(int)soil_value)*100)/500;
    if(soil_percent> 100)
        soil_percent= 100;

    if(soil_percent< 0)
       soil_percent= 0;
			
			water_status = (IOPIN0 & WATER_PIN) ? 0 : 1;
      
       lcd_display();
       uart_display();
			
      IOSET0 = GREEN_LED|YELLOW_LED|RED_LED;
			if(water_status){
					IOSET0 = RED_LED;
					uart0_tx_string("SYSTEM STATUS: WATER ALERT\r\n");
			}
			else if(temperature >= 40 || soil_percent < 30){
					IOSET0 = YELLOW_LED;
					uart0_tx_string("SYSTEM STATUS: WARNING\r\n");
			}
			else{
					IOSET0 = GREEN_LED;
					uart0_tx_string("SYSTEM STATUS: SAFE\r\n");
			}

        delay_ms(1000);
    }
}

