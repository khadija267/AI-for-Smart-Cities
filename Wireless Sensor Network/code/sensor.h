#ifndef ASSIGNMENT_ONE_SENSOR_H
#define ASSIGNMENT_ONE_SENSOR_H
struct Sensor {
float temperature;
float humidity;
float pressure;
unsigned count;
};
float random_value(float min, float max);
double my_exp(double x);
float gaussian(float num);
float read_temperature(float additive_noise, float amplification_factor, float
fading_factor);
float read_humidity(float additive_noise, float amplification_factor, float
fading_factor);
float read_pressure(float additive_noise, float amplification_factor, float fading_factor);
float get_additive_noise();
float get_amplification_factor();
float get_fading_factor();
#endif
