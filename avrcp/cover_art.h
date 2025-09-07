#ifndef __A2DP_COVER_ART_H__
#define __A2DP_COVER_ART_H__
#include <stdint.h>

#include "btstack_config.h"
void cover_art_packet_handler(uint8_t packet_type, uint16_t channel,
                              uint8_t *packet, uint16_t size);
uint8_t cover_art_connect(void);

#endif  // __A2DP_COVER_ART_H__
