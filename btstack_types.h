#ifndef __BTSTACK_TYPES_H__
#define __BTSTACK_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

#include "btstack.h"

typedef struct {
  uint8_t reconfigure;
  uint8_t num_channels;
  uint16_t sampling_frequency;
  uint8_t block_length;
  uint8_t subbands;
  uint8_t min_bitpool_value;
  uint8_t max_bitpool_value;
  btstack_sbc_channel_mode_t channel_mode;
  btstack_sbc_allocation_method_t allocation_method;
} media_codec_configuration_sbc_t;

typedef enum {
  STREAM_STATE_CLOSED,
  STREAM_STATE_OPEN,
  STREAM_STATE_PLAYING,
  STREAM_STATE_PAUSED,
} stream_state_t;

typedef struct {
  uint8_t a2dp_local_seid;
  uint8_t media_sbc_codec_configuration[4];
} a2dp_sink_demo_stream_endpoint_t;

typedef struct {
  bd_addr_t addr;
  uint16_t a2dp_cid;
  uint8_t a2dp_local_seid;
  stream_state_t stream_state;
  media_codec_configuration_sbc_t sbc_configuration;
} a2dp_sink_demo_a2dp_connection_t;

typedef struct {
  bd_addr_t addr;
  uint16_t avrcp_cid;
  bool playing;
  uint16_t notifications_supported_by_target;
} a2dp_sink_demo_avrcp_connection_t;

#endif /* __BTSTACK_TYPES_H__ */
