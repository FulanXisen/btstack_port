#ifndef __BTSTACK_GLOBAL_H__
#define __BTSTACK_GLOBAL_H__

#include "bluetooth.h"
#include "btstack_config.h"
#include "btstack.h"
#include "btstack_types.h"
#include "btstack_resample.h"
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

#ifdef HAVE_BTSTACK_STDIN
const char *get_device_addr_string();
bd_addr_t *get_device_addr();
#endif

a2dp_sink_demo_stream_endpoint_t *get_stream_endpoint();
a2dp_sink_demo_a2dp_connection_t *get_a2dp_connection();
a2dp_sink_demo_avrcp_connection_t *get_avrcp_connection();


// sink state
int volume_percentage = 0;
avrcp_battery_status_t battery_status = AVRCP_BATTERY_STATUS_WARNING;

uint8_t sdp_avdtp_sink_service_buffer[150];
uint8_t sdp_avrcp_target_service_buffer[150];
uint8_t sdp_avrcp_controller_service_buffer[200];
uint8_t device_id_sdp_service_buffer[100];

btstack_packet_callback_registration_t hci_event_callback_registration;
// we support all configurations with bitpool 2-53
uint8_t media_sbc_codec_capabilities[] = {
    0xFF, //(AVDTP_SBC_44100 << 4) | AVDTP_SBC_STEREO,
    0xFF, //(AVDTP_SBC_BLOCK_LENGTH_16 << 4) | (AVDTP_SBC_SUBBANDS_8 << 2) |
          // AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS,
    2, 53};

#ifdef ENABLE_AVRCP_COVER_ART
char a2dp_sink_demo_image_handle[8];
avrcp_cover_art_client_t a2dp_sink_demo_cover_art_client;
bool a2dp_sink_demo_cover_art_client_connected;
uint16_t a2dp_sink_demo_cover_art_cid;
uint8_t a2dp_sink_demo_ertm_buffer[2000];
l2cap_ertm_config_t a2dp_sink_demo_ertm_config = {
        1,  // ertm mandatory
        2,  // max transmit, some tests require > 1
        2000,
        12000,
        512,    // l2cap ertm mtu
        2,
        2,
        1,      // 16-bit FCS
};
bool a2dp_sink_cover_art_download_active;
uint32_t a2dp_sink_cover_art_file_size;
#ifdef HAVE_POSIX_FILE_IO
const char * a2dp_sink_demo_thumbnail_path = "cover.jpg";
FILE * a2dp_sink_cover_art_file;
#endif
#endif

// WAV File
#ifdef STORE_TO_WAV_FILE
uint32_t audio_frame_count = 0;
char * wav_filename = "a2dp_sink_demo.wav";
#endif

// SBC Decoder for WAV file or live playback
const btstack_sbc_decoder_t *   sbc_decoder_instance;
btstack_sbc_decoder_bluedroid_t sbc_decoder_context;
// ring buffer for SBC Frames
// below 30: add samples, 30-40: fine, above 40: drop samples
#define OPTIMAL_FRAMES_MIN 60
#define OPTIMAL_FRAMES_MAX 80
#define ADDITIONAL_FRAMES  30
#define NUM_CHANNELS 2
#define BYTES_PER_FRAME     (2*NUM_CHANNELS)
#define MAX_SBC_FRAME_SIZE 120
unsigned int sbc_frame_size;
btstack_ring_buffer_t sbc_frame_ring_buffer;
btstack_resample_t resample_instance;
int media_initialized = 0;
int audio_stream_started;
btstack_ring_buffer_t decoded_audio_ring_buffer;
// temp storage of lower-layer request for audio samples
int16_t * request_buffer;
int       request_frames;
uint8_t sbc_frame_storage[(OPTIMAL_FRAMES_MAX + ADDITIONAL_FRAMES) * MAX_SBC_FRAME_SIZE];
uint8_t decoded_audio_storage[(128+16) * BYTES_PER_FRAME];

#ifdef HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE
#include "btstack_sample_rate_compensation.h"
#endif
#ifdef HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE
int l2cap_stream_started;
#endif
#ifdef HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE
btstack_sample_rate_compensation_t sample_rate_compensation;
#endif
#endif
