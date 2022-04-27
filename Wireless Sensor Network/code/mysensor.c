#include "contiki.h"
#include "sensor.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "random.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h>
#define LOG_MODULE "Sensor"
#define LOG_LEVEL LOG_LEVEL_INFO
#define WITH_CONTROLLER_REPLY 0
#define UDP_SENSOR_PORT 8765
#define UDP_CONTROLLER_PORT 5678
#define COLLECTION_INTERVAL (3 * CLOCK_SECOND)
static struct simple_udp_connection udp_connection;
//----------------------------------------------------------------
PROCESS(sensor_process, "Sensor Process");
AUTOSTART_PROCESSES(&sensor_process);
//----------------------------------------------------------------
static void
udp_rx_callback(struct simple_udp_connection *c,
const uip_ipaddr_t *sender_addr,
uint16_t sender_port,
const uip_ipaddr_t *receiver_addr,
uint16_t receiver_port,
const uint8_t *data,
uint16_t datalen)
{
LOG_INFO("Controller %.*s", datalen, (char *) data);
LOG_INFO_("\n");
}
//----------------------------------------------------------------
PROCESS_THREAD(sensor_process, ev, data) {
static struct etimer timer;
static struct Sensor sensor;
uip_ipaddr_t dest_ipaddr;
PROCESS_BEGIN();
// Register the UDP connection
simple_udp_register(&udp_connection, UDP_SENSOR_PORT, NULL,
UDP_CONTROLLER_PORT, udp_rx_callback);
// Set the timer
etimer_set(&timer, COLLECTION_INTERVAL);
while(1) {
// Execute the following function when timer expired
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
// Get the factors
float amplification_factor = get_amplification_factor();
float fading_factor = get_fading_factor();
// Get the Guassian noise and generate temperature value
float additive_noise = get_additive_noise();
sensor.temperature = read_temperature(additive_noise,
amplification_factor, fading_factor);
// Get the Guassian noise and generate humidity value
additive_noise = get_additive_noise();
sensor.humidity = read_humidity(additive_noise,
amplification_factor, fading_factor);
// Get the Guassian noise and generate pressure value
additive_noise = get_additive_noise();
sensor.pressure = read_pressure(additive_noise,
amplification_factor, fading_factor);
// Determine whether the controller is reachable
if(NETSTACK_ROUTING.node_is_reachable() &&
NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
LOG_INFO("Sending collected data #%u to ", sensor.count);
LOG_INFO_6ADDR(&dest_ipaddr);
LOG_INFO_("\n");
// Send the stuct "sensor" to destination
simple_udp_sendto(&udp_connection, &sensor, sizeof(sensor),
&dest_ipaddr);
sensor.count++;
} else {
LOG_INFO("Controller not reachable yet\n");
}
// Add some jitter
etimer_set(&timer, COLLECTION_INTERVAL);
}
PROCESS_END();
}
