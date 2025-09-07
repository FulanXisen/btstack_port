#include <stdio.h>

#include "btstack.h"
#include "btstack_config.h"
#include "btstack_types.h"

static const char *device_addr_string = "00:1B:DC:08:E2:72";  // pts v5.0
static bd_addr_t device_addr;

static a2dp_sink_demo_stream_endpoint_t a2dp_sink_demo_stream_endpoint;
static a2dp_sink_demo_a2dp_connection_t a2dp_sink_demo_a2dp_connection;
static a2dp_sink_demo_avrcp_connection_t a2dp_sink_demo_avrcp_connection;

// sink state
static int volume_percentage = 0;
static avrcp_battery_status_t battery_status = AVRCP_BATTERY_STATUS_WARNING;

static uint8_t sdp_avdtp_sink_service_buffer[150];
static uint8_t sdp_avrcp_target_service_buffer[150];
static uint8_t sdp_avrcp_controller_service_buffer[200];
static uint8_t sdp_device_id_service_buffer[100];

static char a2dp_sink_demo_image_handle[8];
static avrcp_cover_art_client_t a2dp_sink_demo_cover_art_client;
static bool a2dp_sink_demo_cover_art_client_connected;
static uint16_t a2dp_sink_demo_cover_art_cid;

static uint8_t a2dp_sink_demo_ertm_buffer[2000];
static l2cap_ertm_config_t a2dp_sink_demo_ertm_config = {
    1,  // ertm mandatory
    2,  // max transmit, some tests require > 1
    2000, 12000,
    512,  // l2cap ertm mtu
    2,    2,
    1,  // 16-bit FCS
};

static bool a2dp_sink_cover_art_download_active;
static uint32_t a2dp_sink_cover_art_file_size;

const char *a2dp_sink_demo_thumbnail_path = "cover.jpg";
FILE *a2dp_sink_cover_art_file;
char *wav_filename = "a2dp_sink_demo.wav";

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

char (*get_a2dp_sink_image_handle())[8] { return &a2dp_sink_demo_image_handle; }

avrcp_cover_art_client_t *get_a2dp_sink_cover_art_client() {
  return &a2dp_sink_demo_cover_art_client;
}

bool get_a2dp_sink_cover_art_client_connected() {
  return a2dp_sink_demo_cover_art_client_connected;
}

void set_a2dp_sink_cover_art_client_connected(bool connected) {
  a2dp_sink_demo_cover_art_client_connected = connected;
}

uint16_t get_a2dp_sink_cover_art_cid() { return a2dp_sink_demo_cover_art_cid; }

void set_a2dp_sink_cover_art_cid(uint16_t cid) {
  a2dp_sink_demo_cover_art_cid = cid;
}

uint16_t *get_a2dp_sink_cover_art_cid_ptr() {
  return &a2dp_sink_demo_cover_art_cid;
}

uint8_t (*get_a2dp_sink_ertm_buffer())[2000] {
  return &a2dp_sink_demo_ertm_buffer;
}

l2cap_ertm_config_t *get_a2dp_sink_ertm_config() {
  return &a2dp_sink_demo_ertm_config;
}

bool get_cover_art_download_status() {
  return a2dp_sink_cover_art_download_active;
}

void set_cover_art_download_status(bool status) {
  a2dp_sink_cover_art_download_active = status;
}

uint32_t get_cover_art_file_size() { return a2dp_sink_cover_art_file_size; }

void set_cover_art_file_size(uint32_t size) {
  a2dp_sink_cover_art_file_size = size;
}

const char *get_thumbnail_path() { return a2dp_sink_demo_thumbnail_path; }

FILE *get_cover_art_file() { return a2dp_sink_cover_art_file; }

void set_cover_art_file(FILE *file) { a2dp_sink_cover_art_file = file; }

const char *get_wav_filename() { return wav_filename; }
