#include "pico/stdlib.h"                // 12-10-2024 Simple-Circuit
#include "hardware/pwm.h"
#include "hardware/clocks.h"

uint slice_num ;       //Global variable for PWM slice number
volatile int flag=0;   //Global flag data ready==1
volatile float rms;    //Global RMS voltage variable
 
void setup() {
    Serial.begin(115200);  //Readings sent out of USB serial port
    //Set GPIO 14 for 133MHz clock and 10-bit PWM
    gpio_set_function(14, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(14);
    pwm_set_wrap(slice_num, 1023);
    pwm_set_clkdiv(slice_num,1);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 512);
    pwm_set_enabled(slice_num, true);

   pinMode(23, OUTPUT); //set GPIO 23 high for PWM regulation of 3.3V regulator
   digitalWrite(23,1);  //this improves 3.3V ripple
}

void setup1() {
}

void loop(){
 rms = 0.0;
 while(true){
  while (flag==0);
  flag=0;
  Serial.println(sqrt(rms),4);
 }
}

void loop1() {
 volatile int i,n; 
 volatile uint64_t t;
 volatile float v,v2;
 
 v2=0.0;
 v=0.0;
 t=time_us_64()+20;
 
 while(true){
  i=analogRead(A0); //ADC read of integrator output
  pwm_set_chan_level(slice_num, PWM_CHAN_A, i); //set PWM to ADC value
  v2=sq(-0.00315937*(i-516.5)); //adjust gain and offset then square it
  v = (v*0.99995 + v2*0.00005); //Low pass IIR filter
  while (t>time_us_64()); //set sample time to 20us
  t = t+20;
  n++;
  n = n % 20000; //flag data ready every 0.4 seconds
  if (n==0) {
    rms = v;
    flag=1;
  }
 }
}
