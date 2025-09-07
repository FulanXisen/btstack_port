#ifndef __AVRCP_H__
#define __AVRCP_H__

#include "btstack_config.h"
#include <stdint.h>
void avrcp_packet_handler(uint8_t packet_type, uint16_t channel,
                          uint8_t *packet, uint16_t size);
void avrcp_controller_packet_handler(uint8_t packet_type, uint16_t channel,
                                     uint8_t *packet, uint16_t size);
void avrcp_target_packet_handler(uint8_t packet_type, uint16_t channel,
                                 uint8_t *packet, uint16_t size);
void avrcp_volume_changed(uint8_t volume);
#endif