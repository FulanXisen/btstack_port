#include "btstack.h"
#include "btstack_config.h"
#include "btstack_types.h"

static const char *device_addr_string = "00:1B:DC:08:E2:72"; // pts v5.0
static bd_addr_t device_addr;

static a2dp_sink_demo_stream_endpoint_t a2dp_sink_demo_stream_endpoint;
static a2dp_sink_demo_a2dp_connection_t a2dp_sink_demo_a2dp_connection;
static a2dp_sink_demo_avrcp_connection_t a2dp_sink_demo_avrcp_connection;

// sink state
static int volume_percentage = 0;
static avrcp_battery_status_t battery_status = AVRCP_BATTERY_STATUS_WARNING;

uint8_t sdp_avdtp_sink_service_buffer[150];
uint8_t sdp_avrcp_target_service_buffer[150];
uint8_t sdp_avrcp_controller_service_buffer[200];
uint8_t sdp_device_id_service_buffer[100];



const char *get_device_addr_string() { return device_addr_string; }
bd_addr_t *get_device_addr() { return &device_addr; }

a2dp_sink_demo_stream_endpoint_t *get_stream_endpoint() {
  return &a2dp_sink_demo_stream_endpoint;
}
a2dp_sink_demo_a2dp_connection_t *get_a2dp_connection() {
  return &a2dp_sink_demo_a2dp_connection;
}
a2dp_sink_demo_avrcp_connection_t *get_avrcp_connection() {
  return &a2dp_sink_demo_avrcp_connection;
}

int get_volume_percent() { return volume_percentage; }
void set_volume_percent(int percent) { volume_percentage = percent; }
avrcp_battery_status_t get_battery_status() { return battery_status; }
void set_battery_status(avrcp_battery_status_t status) {
  battery_status = status;
}

uint8_t (*get_sdp_avdtp_sink_service_buffer())[150] {
  return &sdp_avdtp_sink_service_buffer;
}
uint8_t (*get_sdp_avrcp_target_service_buffer())[150] {
  return &sdp_avrcp_target_service_buffer;
}
uint8_t (*get_sdp_avrcp_controller_service_buffer())[200] {
  return &sdp_avrcp_controller_service_buffer;
}
uint8_t (*get_sdp_device_id_service_buffer())[100] {
  return &sdp_device_id_service_buffer;
}