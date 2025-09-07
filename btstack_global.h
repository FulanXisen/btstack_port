#ifndef __BTSTACK_GLOBAL_H__
#define __BTSTACK_GLOBAL_H__

#include "bluetooth.h"
#include "btstack_config.h"
#include "btstack.h"
#include "btstack_types.h"
#include "btstack_resample.h"
#include "classic/avrcp.h"
#include "classic/avrcp_cover_art_client.h"
#include "l2cap.h"
#include "sbc_types.h"
#include <stdio.h>
#ifdef HAVE_POSIX_FILE_IO
#include "wav_util.h"
#define STORE_TO_WAV_FILE
#ifdef _MSC_VER
// ignore deprecated warning for fopen
#pragma warning(disable : 4996)
#endif
#endif
#ifdef HAVE_BTSTACK_STDIN
#include "btstack_stdin.h"
#endif

// ring buffer for SBC Frames
// below 30: add samples, 30-40: fine, above 40: drop samples
#define OPTIMAL_FRAMES_MIN 60
#define OPTIMAL_FRAMES_MAX 80
#define ADDITIONAL_FRAMES  30
#define NUM_CHANNELS 2
#define BYTES_PER_FRAME     (2*NUM_CHANNELS)
#define MAX_SBC_FRAME_SIZE 120

#ifdef HAVE_BTSTACK_STDIN
const char *get_device_addr_string();
bd_addr_t *get_device_addr();
#endif

a2dp_sink_demo_stream_endpoint_t *get_stream_endpoint();
a2dp_sink_demo_a2dp_connection_t *get_a2dp_connection();
a2dp_sink_demo_avrcp_connection_t *get_avrcp_connection();

int get_volume_percent();
void set_volume_percent(int percent);
avrcp_battery_status_t get_battery_status();
void set_battery_status(avrcp_battery_status_t status);

uint8_t (*get_sdp_avdtp_sink_service_buffer())[150];
uint8_t (*get_sdp_avrcp_target_service_buffer())[150];
uint8_t (*get_sdp_avrcp_controller_service_buffer())[200];
uint8_t (*get_sdp_device_id_service_buffer())[100];



#ifdef STORE_TO_WAV_FILE
const char *get_wav_filename();
#endif

#ifdef ENABLE_AVRCP_COVER_ART
char (*get_a2dp_sink_image_handle())[8];

avrcp_cover_art_client_t *get_a2dp_sink_cover_art_client();
bool get_a2dp_sink_cover_art_client_connected();
void set_a2dp_sink_cover_art_client_connected(bool connected);
uint16_t get_a2dp_sink_cover_art_cid();
void set_a2dp_sink_cover_art_cid(uint16_t cid);
uint16_t *get_a2dp_sink_cover_art_cid_ptr();

uint8_t (*get_a2dp_sink_ertm_buffer())[2000]; 
l2cap_ertm_config_t *get_a2dp_sink_ertm_config();

// True for active, False for inactive
bool get_cover_art_download_status();
void set_cover_art_download_status(bool status);
uint32_t get_cover_art_file_size();
void set_cover_art_file_size(uint32_t size);


#ifdef HAVE_POSIX_FILE_IO
const char *get_thumbnail_path();
FILE *get_cover_art_file();
void set_cover_art_file(FILE *file);

#endif
#endif



#endif // __BTSTACK_GLOBAL_H__
