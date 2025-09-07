#include "btstack.h"
#include "btstack_config.h"
#include "btstack_types.h"

static const char *device_addr_string = "00:1B:DC:08:E2:72"; // pts v5.0
static bd_addr_t device_addr;

static a2dp_sink_demo_stream_endpoint_t a2dp_sink_demo_stream_endpoint;
static a2dp_sink_demo_a2dp_connection_t a2dp_sink_demo_a2dp_connection;
static a2dp_sink_demo_avrcp_connection_t a2dp_sink_demo_avrcp_connection;

const char *get_device_addr_string() { return device_addr_string; }
bd_addr_t *get_device_addr() { return &device_addr; }


a2dp_sink_demo_stream_endpoint_t *get_stream_endpoint(){
    return &a2dp_sink_demo_stream_endpoint;
}
a2dp_sink_demo_a2dp_connection_t *get_a2dp_connection(){
    return &a2dp_sink_demo_a2dp_connection;
}
a2dp_sink_demo_avrcp_connection_t *get_avrcp_connection(){
    return &a2dp_sink_demo_avrcp_connection;
}