#include "pico/stdlib.h"               // 12-10-2024 Simple-Circuit
#include "hardware/pwm.h"
#include "hardware/clocks.h"

uint slice_num ;       //Global variable for PWM slice number
volatile float vg;       //Global ADC average value
volatile int flag=0;   //Global flag data ready==1

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
  volatile int i;
  float v2;
 while(true){
  while (flag==0);  //wait for data update by core-2
  v2 = -0.00315937*(vg-516.5); //scale data with offset and gain
  //apply third order error correcton for improved linearity
  //v2 = v2-((0.0018097*v2*v2*v2)+(0.00078963*v2*v2)-(0.0030251*v2)-0.001784);
//apply forth order error correcton for improved linearity
v2 = v2-((-0.00046508*v2*v2*v2*v2)+(0.0017405*v2*v2*v2)+(0.0017531*v2*v2)-(0.0025101*v2)-0.00080592);
  Serial.println(v2,4);  
  flag=0;
 }
}

void loop1() {
 volatile int i,n; 
 volatile uint64_t t;
 volatile float v,v2;
 
 n=0;
 v=0.0;
 t=time_us_64()+12;
 
 while(true){
  i=analogRead(A0); //ADC read of integrator output
  pwm_set_chan_level(slice_num, PWM_CHAN_A, i); //set PWM to ADC value
  v = (v*0.999 + i*0.001);    //Low pass IIR filter
  while (t>time_us_64());         //sample at 12us rate
  t = t+12;
  n++;
  n = n % 1000;              //update data every 12ms
  if (n==0){
    vg =v;
    flag=1;                 //flag new data available
  }
 }
}
