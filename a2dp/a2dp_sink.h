#ifndef __A2DP_SINK_H__
#define __A2DP_SINK_H__

#include <stdint.h>

#include "btstack_config.h"

void a2dp_sink_packet_handler(uint8_t packet_type, uint16_t channel,
                              uint8_t *packet, uint16_t event_size);
void handle_l2cap_media_data_packet(uint8_t seid, uint8_t *packet,
                                    uint16_t size);

#endif
