#ifndef __HCI_H__
#define __HCI_H__

#include <stdint.h>

#include "btstack_config.h"

void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet,
                        uint16_t size);

#endif
