#include "sensor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.1415926535
// Get a random value between min and max
float random_value(float min, float max) {
float scale = rand() / (float) RAND_MAX;
return min + scale * (max - min);
}
// There if something worng while compeling the exp() function in the "math.h"file.
// my_exp() function is to calculate exponential value of x
double my_exp(double x) {
int integer = (int)x; // Integer part of x
double decimal = x - integer; // Decimal part of x
int order = 10; // Order of Taylor Formula
double numerator = 1.0; // Numerator of Taylor Formula
double denominator = 1.0; // Denominator of Taylor Formula
double sum = 1.0; // Sum of Taylor's Formula for decimal part
double production = 1.0; // Production of integer part
// Calculate exp(integer) of x
for(int i = 0; i < abs(integer); i++)
production *= 2.718281;
// Get the reciprocal if x less than 0
if (x < 0) production = 1 / production;
// Calculate exp(decimal) of x with Taylor formula
for(int i = 1; i < order; i++){
numerator *= decimal;
denominator *= i;
sum += numerator / denominator;
}
// Return result
return sum * production;
}
// Calculate the Gaussian Distribution vaule of num
float gaussian(float num) {
float temp = my_exp(-0.5 * num * num) * sqrt(2 * PI);
return temp;
}
// Read temperature from sensor
float read_temperature(float additive_noise, float amplification_factor, float fading_factor) {
float temperature = 25.0;
temperature = (temperature + additive_noise) * amplification_factor *
fading_factor;
return temperature;
}
// Read humidity from sensor
float read_humidity(float additive_noise, float amplification_factor, float fading_factor) {
float humidity = 40.0;
humidity = (humidity + additive_noise) * amplification_factor * fading_factor;
return humidity;
}
// Read pressure from sensor
float read_pressure(float additive_noise, float amplification_factor, float fading_factor) {
float pressure = 101.0 * 1000;
pressure = (pressure + additive_noise) * amplification_factor *
fading_factor;
return pressure;
}
// Get additive noise for readed value
float get_additive_noise() {
return gaussian(random_value(-5.0, 5.0));
}
// Get amplification factor for sensor
float get_amplification_factor() {
return 2.0;
}
// Get fading factor for transmitting process
float get_fading_factor() {
return 0.5;
}
