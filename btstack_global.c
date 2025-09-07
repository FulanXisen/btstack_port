#include "btstack.h"
#include "btstack_config.h"
#include "btstack_types.h"

static const char *device_addr_string = "00:1B:DC:08:E2:72"; // pts v5.0
static bd_addr_t device_addr;

const char *get_device_addr_string() { return device_addr_string; }
bd_addr_t *get_device_addr() { return &device_addr; }