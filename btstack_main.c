#include "btstack_main.h"

#include <stdio.h>

#include "a2dp/a2dp_cover_art.h"
#include "a2dp/a2dp_sink.h"
#include "avrcp/avrcp.h"
#include "btstack_global.h"
#include "classic/avrcp.h"
#include "hci/hci.h"

static btstack_packet_callback_registration_t hci_event_callback_registration;

// we support all configurations with bitpool 2-53
uint8_t media_sbc_codec_capabilities[] = {
    0xFF,  //(AVDTP_SBC_44100 << 4) | AVDTP_SBC_STEREO,
    0xFF,  //(AVDTP_SBC_BLOCK_LENGTH_16 << 4) | (AVDTP_SBC_SUBBANDS_8 << 2) |
           // AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS,
    2, 53};

#ifdef HAVE_BTSTACK_STDIN
static void show_usage(void) {
  bd_addr_t iut_address;
  gap_local_bd_addr(iut_address);
  printf("\n--- A2DP Sink Demo Console %s ---\n", bd_addr_to_str(iut_address));
  printf("b - A2DP Sink create connection to addr %s\n",
         bd_addr_to_str(*get_device_addr()));
  printf("B - A2DP Sink disconnect\n");

  printf("\n--- AVRCP Controller ---\n");
  printf("c - AVRCP create connection to addr %s\n",
         bd_addr_to_str(*get_device_addr()));
  printf("C - AVRCP disconnect\n");
  printf("O - get play status\n");
  printf("j - get now playing info\n");
  printf("k - play\n");
  printf("K - stop\n");
  printf("L - pause\n");
  printf("u - start fast forward\n");
  printf("U - stop  fast forward\n");
  printf("n - start rewind\n");
  printf("N - stop rewind\n");
  printf("i - forward\n");
  printf("I - backward\n");
  printf("M - mute\n");
  printf("r - skip\n");
  printf("q - query repeat and shuffle mode\n");
  printf("v - repeat single track\n");
  printf("w - delay report\n");
  printf("x - repeat all tracks\n");
  printf("X - disable repeat mode\n");
  printf("z - shuffle all tracks\n");
  printf("Z - disable shuffle mode\n");

  printf("a/A - register/deregister TRACK_CHANGED\n");
  printf("R/P - register/deregister PLAYBACK_POS_CHANGED\n");

  printf("s/S - send/release long button press REWIND\n");

  printf("\n--- Volume and Battery Control ---\n");
  printf("t - volume up   for 10 percent\n");
  printf("T - volume down for 10 percent\n");
  printf(
      "V - toggle Battery status from AVRCP_BATTERY_STATUS_NORMAL to "
      "AVRCP_BATTERY_STATUS_FULL_CHARGE\n");

#ifdef ENABLE_AVRCP_COVER_ART
  printf("\n--- Cover Art Client ---\n");
  printf("d - connect to addr %s\n", bd_addr_to_str(*get_device_addr()));
  printf("D - disconnect\n");
  if (get_a2dp_sink_cover_art_client_connected() == false) {
    if (get_avrcp_connection()->avrcp_cid == 0) {
      printf(
          "Not connected, press 'b' or 'c' to first connect AVRCP, then press "
          "'d' to connect cover art client\n");
    } else {
      printf("Not connected, press 'd' to connect cover art client\n");
    }
  } else if ((*get_a2dp_sink_image_handle())[0] == 0) {
    printf("No image handle, use 'j' to get current track info\n");
  }
  printf("---\n");
#endif
}
#endif

#ifdef HAVE_BTSTACK_STDIN

static void stdin_process(char cmd) {
  uint8_t status = ERROR_CODE_SUCCESS;
  uint8_t volume;
  avrcp_battery_status_t old_battery_status;

  a2dp_sink_demo_a2dp_connection_t* a2dp_connection = get_a2dp_connection();
  a2dp_sink_demo_avrcp_connection_t* avrcp_connection = get_avrcp_connection();

  int volume_percent = get_volume_percent();
  avrcp_battery_status_t battery_status = get_battery_status();
  switch (cmd) {
    case 'b':
      status = a2dp_sink_establish_stream(*get_device_addr(),
                                          &a2dp_connection->a2dp_cid);
      printf(
          " - Create AVDTP connection to addr %s, and local seid %d, cid "
          "0x%02x.\n",
          bd_addr_to_str(*get_device_addr()), a2dp_connection->a2dp_local_seid,
          a2dp_connection->a2dp_cid);
      break;
    case 'B':
      printf(" - AVDTP disconnect from addr %s.\n",
             bd_addr_to_str(*get_device_addr()));
      a2dp_sink_disconnect(a2dp_connection->a2dp_cid);
      break;
    case 'c':
      printf(" - Create AVRCP connection to addr %s.\n",
             bd_addr_to_str(*get_device_addr()));
      status = avrcp_connect(*get_device_addr(), &avrcp_connection->avrcp_cid);
      break;
    case 'C':
      printf(" - AVRCP disconnect from addr %s.\n",
             bd_addr_to_str(*get_device_addr()));
      status = avrcp_disconnect(avrcp_connection->avrcp_cid);
      break;

    case '\n':
    case '\r':
      break;
    case 'w':
      printf("Send delay report\n");
      avdtp_sink_delay_report(a2dp_connection->a2dp_cid,
                              a2dp_connection->a2dp_local_seid, 100);
      break;
    // Volume Control
    case 't':
      volume_percent = get_volume_percent();
      set_volume_percent(volume_percent <= 90 ? volume_percent + 10 : 100);
      volume_percent = get_volume_percent();
      volume = volume_percent * 127 / 100;
      printf(" - volume up   for 10 percent, %d%% (%d) \n", volume_percent,
             volume);
      status = avrcp_target_volume_changed(avrcp_connection->avrcp_cid, volume);
      avrcp_volume_changed(volume);
      break;
    case 'T':
      volume_percent = get_volume_percent();
      set_volume_percent(volume_percent >= 10 ? volume_percent - 10 : 0);
      volume_percent = get_volume_percent();
      volume = volume_percent * 127 / 100;
      printf(" - volume down for 10 percent, %d%% (%d) \n", volume_percent,
             volume);
      status = avrcp_target_volume_changed(avrcp_connection->avrcp_cid, volume);
      avrcp_volume_changed(volume);
      break;
    case 'V':
      battery_status = get_battery_status();
      old_battery_status = battery_status;

      if (battery_status < AVRCP_BATTERY_STATUS_FULL_CHARGE) {
        set_battery_status(
            (avrcp_battery_status_t)((uint8_t)battery_status + 1));
      } else {
        set_battery_status(AVRCP_BATTERY_STATUS_NORMAL);
      }
      battery_status = get_battery_status();
      printf(" - toggle battery value, old %d, new %d\n", old_battery_status,
             battery_status);
      status = avrcp_target_battery_status_changed(avrcp_connection->avrcp_cid,
                                                   battery_status);
      break;
    case 'O':
      printf(" - get play status\n");
      status = avrcp_controller_get_play_status(avrcp_connection->avrcp_cid);
      break;
    case 'j':
      printf(" - get now playing info\n");
      status =
          avrcp_controller_get_now_playing_info(avrcp_connection->avrcp_cid);
      break;
    case 'k':
      printf(" - play\n");
      status = avrcp_controller_play(avrcp_connection->avrcp_cid);
      break;
    case 'K':
      printf(" - stop\n");
      status = avrcp_controller_stop(avrcp_connection->avrcp_cid);
      break;
    case 'L':
      printf(" - pause\n");
      status = avrcp_controller_pause(avrcp_connection->avrcp_cid);
      break;
    case 'u':
      printf(" - start fast forward\n");
      status = avrcp_controller_press_and_hold_fast_forward(
          avrcp_connection->avrcp_cid);
      break;
    case 'U':
      printf(" - stop fast forward\n");
      status = avrcp_controller_release_press_and_hold_cmd(
          avrcp_connection->avrcp_cid);
      break;
    case 'n':
      printf(" - start rewind\n");
      status =
          avrcp_controller_press_and_hold_rewind(avrcp_connection->avrcp_cid);
      break;
    case 'N':
      printf(" - stop rewind\n");
      status = avrcp_controller_release_press_and_hold_cmd(
          avrcp_connection->avrcp_cid);
      break;
    case 'i':
      printf(" - forward\n");
      status = avrcp_controller_forward(avrcp_connection->avrcp_cid);
      break;
    case 'I':
      printf(" - backward\n");
      status = avrcp_controller_backward(avrcp_connection->avrcp_cid);
      break;
    case 'M':
      printf(" - mute\n");
      status = avrcp_controller_mute(avrcp_connection->avrcp_cid);
      break;
    case 'r':
      printf(" - skip\n");
      status = avrcp_controller_skip(avrcp_connection->avrcp_cid);
      break;
    case 'q':
      printf(" - query repeat and shuffle mode\n");
      status = avrcp_controller_query_shuffle_and_repeat_modes(
          avrcp_connection->avrcp_cid);
      break;
    case 'v':
      printf(" - repeat single track\n");
      status = avrcp_controller_set_repeat_mode(avrcp_connection->avrcp_cid,
                                                AVRCP_REPEAT_MODE_SINGLE_TRACK);
      break;
    case 'x':
      printf(" - repeat all tracks\n");
      status = avrcp_controller_set_repeat_mode(avrcp_connection->avrcp_cid,
                                                AVRCP_REPEAT_MODE_ALL_TRACKS);
      break;
    case 'X':
      printf(" - disable repeat mode\n");
      status = avrcp_controller_set_repeat_mode(avrcp_connection->avrcp_cid,
                                                AVRCP_REPEAT_MODE_OFF);
      break;
    case 'z':
      printf(" - shuffle all tracks\n");
      status = avrcp_controller_set_shuffle_mode(avrcp_connection->avrcp_cid,
                                                 AVRCP_SHUFFLE_MODE_ALL_TRACKS);
      break;
    case 'Z':
      printf(" - disable shuffle mode\n");
      status = avrcp_controller_set_shuffle_mode(avrcp_connection->avrcp_cid,
                                                 AVRCP_SHUFFLE_MODE_OFF);
      break;
    case 'a':
      printf("AVRCP: enable notification TRACK_CHANGED\n");
      status = avrcp_controller_enable_notification(
          avrcp_connection->avrcp_cid, AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);
      break;
    case 'A':
      printf("AVRCP: disable notification TRACK_CHANGED\n");
      status = avrcp_controller_disable_notification(
          avrcp_connection->avrcp_cid, AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);
      break;
    case 'R':
      printf("AVRCP: enable notification PLAYBACK_POS_CHANGED\n");
      status = avrcp_controller_enable_notification(
          avrcp_connection->avrcp_cid,
          AVRCP_NOTIFICATION_EVENT_PLAYBACK_POS_CHANGED);
      break;
    case 'P':
      printf("AVRCP: disable notification PLAYBACK_POS_CHANGED\n");
      status = avrcp_controller_disable_notification(
          avrcp_connection->avrcp_cid,
          AVRCP_NOTIFICATION_EVENT_PLAYBACK_POS_CHANGED);
      break;
    case 's':
      printf("AVRCP: send long button press REWIND\n");
      status = avrcp_controller_start_press_and_hold_cmd(
          avrcp_connection->avrcp_cid, AVRCP_OPERATION_ID_REWIND);
      break;
    case 'S':
      printf("AVRCP: release long button press REWIND\n");
      status = avrcp_controller_release_press_and_hold_cmd(
          avrcp_connection->avrcp_cid);
      break;
#ifdef ENABLE_AVRCP_COVER_ART
    case 'd':
      printf(" - Create AVRCP Cover Art connection to addr %s.\n",
             bd_addr_to_str(*get_device_addr()));
      status = cover_art_connect();
      break;
    case 'D':
      printf(" - AVRCP Cover Art disconnect from addr %s.\n",
             bd_addr_to_str(*get_device_addr()));
      status = avrcp_cover_art_client_disconnect(get_a2dp_sink_cover_art_cid());
      break;
    case '@':
      printf("Get linked thumbnail for '%s'\n",
             (*get_a2dp_sink_image_handle()));
#ifdef HAVE_POSIX_FILE_IO
      set_cover_art_file(fopen(get_thumbnail_path(), "w"));
#endif
      set_cover_art_download_status(true);
      set_cover_art_file_size(0);
      status = avrcp_cover_art_client_get_linked_thumbnail(
          get_a2dp_sink_cover_art_cid(), (*get_a2dp_sink_image_handle()));
      break;
#endif
    default:
      show_usage();
      return;
  }
  if (status != ERROR_CODE_SUCCESS) {
    printf("Could not perform command, status 0x%02x\n", status);
  }
}
#endif

int btstack_setup() {
  // init protocols
  l2cap_init();
  sdp_init();
#ifdef ENABLE_BLE
  // Initialize LE Security Manager. Needed for cross-transport key derivation
  sm_init();
#endif
#ifdef ENABLE_AVRCP_COVER_ART
  goep_client_init();
  avrcp_cover_art_client_init();
#endif

  // Init profiles
  a2dp_sink_init();
  avrcp_init();
  avrcp_controller_init();
  avrcp_target_init();

  // Configure A2DP Sink
  a2dp_sink_register_packet_handler(&a2dp_sink_packet_handler);
  a2dp_sink_register_media_handler(&handle_l2cap_media_data_packet);
  a2dp_sink_demo_stream_endpoint_t* stream_endpoint = get_stream_endpoint();
  avdtp_stream_endpoint_t* local_stream_endpoint =
      a2dp_sink_create_stream_endpoint(
          AVDTP_AUDIO, AVDTP_CODEC_SBC, media_sbc_codec_capabilities,
          sizeof(media_sbc_codec_capabilities),
          stream_endpoint->media_sbc_codec_configuration,
          sizeof(stream_endpoint->media_sbc_codec_configuration));
  btstack_assert(local_stream_endpoint != NULL);
  // - Store stream enpoint's SEP ID, as it is used by A2DP API to identify the
  // stream endpoint
  stream_endpoint->a2dp_local_seid = avdtp_local_seid(local_stream_endpoint);

  // Configure AVRCP Controller + Target
  avrcp_register_packet_handler(&avrcp_packet_handler);
  avrcp_controller_register_packet_handler(&avrcp_controller_packet_handler);
  avrcp_target_register_packet_handler(&avrcp_target_packet_handler);

  // Configure SDP

  // - Create and register A2DP Sink service record
  memset(*get_sdp_avdtp_sink_service_buffer(), 0,
         sizeof(*get_sdp_avdtp_sink_service_buffer()));
  a2dp_sink_create_sdp_record(*get_sdp_avdtp_sink_service_buffer(),
                              sdp_create_service_record_handle(),
                              AVDTP_SINK_FEATURE_MASK_HEADPHONE, NULL, NULL);
  btstack_assert(de_get_len(*get_sdp_avdtp_sink_service_buffer()) <=
                 sizeof(*get_sdp_avdtp_sink_service_buffer()));
  sdp_register_service(*get_sdp_avdtp_sink_service_buffer());

  // - Create AVRCP Controller service record and register it with SDP. We send
  // Category 1 commands to the media player, e.g. play/pause
  memset(*get_sdp_avrcp_controller_service_buffer(), 0,
         sizeof(*get_sdp_avrcp_controller_service_buffer()));
  uint16_t controller_supported_features =
      1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER;
#ifdef AVRCP_BROWSING_ENABLED
  controller_supported_features |=
      1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_BROWSING;
#endif
#ifdef ENABLE_AVRCP_COVER_ART
  controller_supported_features |=
      1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_COVER_ART_GET_LINKED_THUMBNAIL;
#endif
  avrcp_controller_create_sdp_record(*get_sdp_avrcp_controller_service_buffer(),
                                     sdp_create_service_record_handle(),
                                     controller_supported_features, NULL, NULL);
  btstack_assert(de_get_len(*get_sdp_avrcp_controller_service_buffer()) <=
                 sizeof(*get_sdp_avrcp_controller_service_buffer()));
  sdp_register_service(*get_sdp_avrcp_controller_service_buffer());

  // - Create and register AVRCP Target service record
  //   -  We receive Category 2 commands from the media player, e.g. volume
  //   up/down
  memset(*get_sdp_avrcp_target_service_buffer(), 0,
         sizeof(*get_sdp_avrcp_target_service_buffer()));
  uint16_t target_supported_features =
      1 << AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_MONITOR_OR_AMPLIFIER;
  avrcp_target_create_sdp_record(*get_sdp_avrcp_target_service_buffer(),
                                 sdp_create_service_record_handle(),
                                 target_supported_features, NULL, NULL);
  btstack_assert(de_get_len(*get_sdp_avrcp_target_service_buffer()) <=
                 sizeof(*get_sdp_avrcp_target_service_buffer()));
  sdp_register_service(*get_sdp_avrcp_target_service_buffer());

  // - Create and register Device ID (PnP) service record
  memset(*get_sdp_device_id_service_buffer(), 0,
         sizeof(*get_sdp_device_id_service_buffer()));
  device_id_create_sdp_record(*get_sdp_device_id_service_buffer(),
                              sdp_create_service_record_handle(),
                              DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH,
                              BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
  btstack_assert(de_get_len(*get_sdp_device_id_service_buffer()) <=
                 sizeof(*get_sdp_device_id_service_buffer()));
  sdp_register_service(*get_sdp_device_id_service_buffer());

  // Configure GAP - discovery / connection

  // - Set local name with a template Bluetooth address, that will be
  // automatically
  //   replaced with an actual address once it is available, i.e. when BTstack
  //   boots up and starts talking to a Bluetooth module.
  gap_set_local_name("A2DP Sink Demo 00:00:00:00:00:00");

  // - Allow to show up in Bluetooth inquiry
  gap_discoverable_control(1);

  // - Set Class of Device - Service Class: Audio, Major Device Class: Audio,
  // Minor: Headphone
  gap_set_class_of_device(0x200404);

  // - Allow for role switch in general and sniff mode
  gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_ROLE_SWITCH |
                                       LM_LINK_POLICY_ENABLE_SNIFF_MODE);

  // - Allow for role switch on outgoing connections
  //   - This allows A2DP Source, e.g. smartphone, to become master when we
  //   re-connect to it.
  gap_set_allow_role_switch(true);

  // Register for HCI events
  hci_event_callback_registration.callback = &hci_packet_handler;
  hci_add_event_handler(&hci_event_callback_registration);

  // Inform about audio playback / test options
#ifdef HAVE_POSIX_FILE_IO
  if (!btstack_audio_sink_get_instance()) {
    printf("No audio playback.\n");
  } else {
    printf("Audio playback supported.\n");
  }
#ifdef STORE_TO_WAV_FILE
  printf("Audio will be stored to \'%s\' file.\n", get_wav_filename());
#endif
#endif
  return 0;
}

int btstack_main(int argc, const char* argv[]) {
  UNUSED(argc);
  (void)argv;

#ifdef HAVE_BTSTACK_STDIN
  printf("HAVE_BTSTACK_STDIN\n");
#endif

#ifdef STORE_TO_WAV_FILE
  printf("STORE_TO_WAV_FILE\n");
#endif

#ifdef ENABLE_AVRCP_COVER_ART
  printf("ENABLE_AVRCP_COVER_ART\n");
#endif

#ifdef HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE
  printf("HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE\n");
#endif

#ifdef HAVE_POSIX_FILE_IO
  printf("HAVE_POSIX_FILE_IO\n");
#endif

  btstack_setup();

#ifdef HAVE_BTSTACK_STDIN
  // parse human-readable Bluetooth address
  sscanf_bd_addr(get_device_addr_string(), *get_device_addr());
  btstack_stdin_setup(stdin_process);
#endif

  // turn on!
  printf("Starting BTstack ...\n");
  hci_power_control(HCI_POWER_ON);
  return 0;
}
/* EXAMPLE_END */