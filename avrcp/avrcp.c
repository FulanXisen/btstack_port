#include "avrcp.h"
#include "a2dp/a2dp_cover_art.h"
#include "btstack.h"
#include "btstack_global.h"
#include "btstack_types.h"
#include "inttypes.h"
#include <stdio.h>

void avrcp_packet_handler(uint8_t packet_type, uint16_t channel,
                          uint8_t *packet, uint16_t size) {
  UNUSED(channel);
  UNUSED(size);
  uint16_t local_cid;
  uint8_t status;
  bd_addr_t address;

  a2dp_sink_demo_avrcp_connection_t *connection = get_avrcp_connection();

  if (packet_type != HCI_EVENT_PACKET)
    return;
  if (hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META)
    return;
  switch (packet[2]) {
  case AVRCP_SUBEVENT_CONNECTION_ESTABLISHED: {
    local_cid = avrcp_subevent_connection_established_get_avrcp_cid(packet);
    status = avrcp_subevent_connection_established_get_status(packet);
    if (status != ERROR_CODE_SUCCESS) {
      printf("AVRCP: Connection failed, status 0x%02x\n", status);
      connection->avrcp_cid = 0;
      return;
    }

    connection->avrcp_cid = local_cid;
    avrcp_subevent_connection_established_get_bd_addr(packet, address);
    printf("AVRCP: Connected to %s, cid 0x%02x\n", bd_addr_to_str(address),
           connection->avrcp_cid);

#ifdef HAVE_BTSTACK_STDIN
    // use address for outgoing connections
    avrcp_subevent_connection_established_get_bd_addr(
        packet, *get_device_addr());
#endif

    avrcp_target_support_event(connection->avrcp_cid,
                               AVRCP_NOTIFICATION_EVENT_VOLUME_CHANGED);
    avrcp_target_support_event(connection->avrcp_cid,
                               AVRCP_NOTIFICATION_EVENT_BATT_STATUS_CHANGED);
    avrcp_target_battery_status_changed(connection->avrcp_cid, get_battery_status());

    // query supported events:
    avrcp_controller_get_supported_events(connection->avrcp_cid);
    return;
  }

  case AVRCP_SUBEVENT_CONNECTION_RELEASED:
    printf("AVRCP: Channel released: cid 0x%02x\n",
           avrcp_subevent_connection_released_get_avrcp_cid(packet));
    connection->avrcp_cid = 0;
    connection->notifications_supported_by_target = 0;
    return;
  default:
    break;
  }
}

void avrcp_controller_packet_handler(uint8_t packet_type, uint16_t channel,
                                     uint8_t *packet, uint16_t size) {
  UNUSED(channel);
  UNUSED(size);

  // helper to print c strings
  uint8_t avrcp_subevent_value[256];
  uint8_t play_status;
  uint8_t event_id;

  a2dp_sink_demo_avrcp_connection_t *avrcp_connection = get_avrcp_connection();

  if (packet_type != HCI_EVENT_PACKET)
    return;
  if (hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META)
    return;
  if (avrcp_connection->avrcp_cid == 0)
    return;

  memset(avrcp_subevent_value, 0, sizeof(avrcp_subevent_value));
  switch (packet[2]) {
  case AVRCP_SUBEVENT_GET_CAPABILITY_EVENT_ID:
    avrcp_connection->notifications_supported_by_target |=
        (1 << avrcp_subevent_get_capability_event_id_get_event_id(packet));
    break;
  case AVRCP_SUBEVENT_GET_CAPABILITY_EVENT_ID_DONE:

    printf("AVRCP Controller: supported notifications by target:\n");
    for (event_id = (uint8_t)AVRCP_NOTIFICATION_EVENT_FIRST_INDEX;
         event_id < (uint8_t)AVRCP_NOTIFICATION_EVENT_LAST_INDEX; event_id++) {
      printf("   - [%s] %s\n",
             (avrcp_connection->notifications_supported_by_target &
              (1 << event_id)) != 0
                 ? "X"
                 : " ",
             avrcp_notification2str((avrcp_notification_event_id_t)event_id));
    }
    printf("\n\n");

    // automatically enable notifications
    avrcp_controller_enable_notification(
        avrcp_connection->avrcp_cid,
        AVRCP_NOTIFICATION_EVENT_PLAYBACK_STATUS_CHANGED);
    avrcp_controller_enable_notification(
        avrcp_connection->avrcp_cid,
        AVRCP_NOTIFICATION_EVENT_NOW_PLAYING_CONTENT_CHANGED);
    avrcp_controller_enable_notification(
        avrcp_connection->avrcp_cid, AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);

#ifdef ENABLE_AVRCP_COVER_ART
    // image handles become invalid on player change, registe for notifications
    avrcp_controller_enable_notification(
        avrcp_connection->avrcp_cid,
        AVRCP_NOTIFICATION_EVENT_UIDS_CHANGED);
    // trigger cover art client connection
    a2dp_sink_demo_cover_art_connect();
#endif
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_STATE:
    event_id = (avrcp_notification_event_id_t)
        avrcp_subevent_notification_state_get_event_id(packet);
    printf("AVRCP Controller: %s notification registered\n",
           avrcp_notification2str(event_id));
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_PLAYBACK_POS_CHANGED:
    printf(
        "AVRCP Controller: Playback position changed, position %d ms\n",
        (unsigned int)
            avrcp_subevent_notification_playback_pos_changed_get_playback_position_ms(
                packet));
    break;
  case AVRCP_SUBEVENT_NOTIFICATION_PLAYBACK_STATUS_CHANGED:
    printf(
        "AVRCP Controller: Playback status changed %s\n",
        avrcp_play_status2str(
            avrcp_subevent_notification_playback_status_changed_get_play_status(
                packet)));
    play_status =
        avrcp_subevent_notification_playback_status_changed_get_play_status(
            packet);
    switch (play_status) {
    case AVRCP_PLAYBACK_STATUS_PLAYING:
      avrcp_connection->playing = true;
      break;
    default:
      avrcp_connection->playing = false;
      break;
    }
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_NOW_PLAYING_CONTENT_CHANGED:
    printf("AVRCP Controller: Playing content changed\n");
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_TRACK_CHANGED:
    printf("AVRCP Controller: Track changed\n");
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_AVAILABLE_PLAYERS_CHANGED:
    printf("AVRCP Controller: Available Players Changed\n");
    break;

  case AVRCP_SUBEVENT_SHUFFLE_AND_REPEAT_MODE: {
    uint8_t shuffle_mode =
        avrcp_subevent_shuffle_and_repeat_mode_get_shuffle_mode(packet);
    uint8_t repeat_mode =
        avrcp_subevent_shuffle_and_repeat_mode_get_repeat_mode(packet);
    printf("AVRCP Controller: %s, %s\n", avrcp_shuffle2str(shuffle_mode),
           avrcp_repeat2str(repeat_mode));
    break;
  }
  case AVRCP_SUBEVENT_NOW_PLAYING_TRACK_INFO:
    printf("AVRCP Controller: Track %d\n",
           avrcp_subevent_now_playing_track_info_get_track(packet));
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_TOTAL_TRACKS_INFO:
    printf(
        "AVRCP Controller: Total Tracks %d\n",
        avrcp_subevent_now_playing_total_tracks_info_get_total_tracks(packet));
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_TITLE_INFO:
    if (avrcp_subevent_now_playing_title_info_get_value_len(packet) > 0) {
      memcpy(avrcp_subevent_value,
             avrcp_subevent_now_playing_title_info_get_value(packet),
             avrcp_subevent_now_playing_title_info_get_value_len(packet));
      printf("AVRCP Controller: Title %s\n", avrcp_subevent_value);
    }
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_ARTIST_INFO:
    if (avrcp_subevent_now_playing_artist_info_get_value_len(packet) > 0) {
      memcpy(avrcp_subevent_value,
             avrcp_subevent_now_playing_artist_info_get_value(packet),
             avrcp_subevent_now_playing_artist_info_get_value_len(packet));
      printf("AVRCP Controller: Artist %s\n", avrcp_subevent_value);
    }
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_ALBUM_INFO:
    if (avrcp_subevent_now_playing_album_info_get_value_len(packet) > 0) {
      memcpy(avrcp_subevent_value,
             avrcp_subevent_now_playing_album_info_get_value(packet),
             avrcp_subevent_now_playing_album_info_get_value_len(packet));
      printf("AVRCP Controller: Album %s\n", avrcp_subevent_value);
    }
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_GENRE_INFO:
    if (avrcp_subevent_now_playing_genre_info_get_value_len(packet) > 0) {
      memcpy(avrcp_subevent_value,
             avrcp_subevent_now_playing_genre_info_get_value(packet),
             avrcp_subevent_now_playing_genre_info_get_value_len(packet));
      printf("AVRCP Controller: Genre %s\n", avrcp_subevent_value);
    }
    break;

  case AVRCP_SUBEVENT_PLAY_STATUS:
    printf("AVRCP Controller: Song length %" PRIu32
           " ms, Song position %" PRIu32 " ms, Play status %s\n",
           avrcp_subevent_play_status_get_song_length(packet),
           avrcp_subevent_play_status_get_song_position(packet),
           avrcp_play_status2str(
               avrcp_subevent_play_status_get_play_status(packet)));
    break;

  case AVRCP_SUBEVENT_OPERATION_COMPLETE:
    printf("AVRCP Controller: %s complete\n",
           avrcp_operation2str(
               avrcp_subevent_operation_complete_get_operation_id(packet)));
    break;

  case AVRCP_SUBEVENT_OPERATION_START:
    printf("AVRCP Controller: %s start\n",
           avrcp_operation2str(
               avrcp_subevent_operation_start_get_operation_id(packet)));
    break;

  case AVRCP_SUBEVENT_NOTIFICATION_EVENT_TRACK_REACHED_END:
    printf("AVRCP Controller: Track reached end\n");
    break;

  case AVRCP_SUBEVENT_PLAYER_APPLICATION_VALUE_RESPONSE:
    printf(
        "AVRCP Controller: Set Player App Value %s\n",
        avrcp_ctype2str(
            avrcp_subevent_player_application_value_response_get_command_type(
                packet)));
    break;

#ifdef ENABLE_AVRCP_COVER_ART
  case AVRCP_SUBEVENT_NOTIFICATION_EVENT_UIDS_CHANGED:
    if (get_a2dp_sink_cover_art_client_connected()) {
      printf("AVRCP Controller: UIDs changed -> disconnect cover art client\n");
      avrcp_cover_art_client_disconnect(get_a2dp_sink_cover_art_cid());
    }
    break;

  case AVRCP_SUBEVENT_NOW_PLAYING_COVER_ART_INFO:
    if (avrcp_subevent_now_playing_cover_art_info_get_value_len(packet) == 7) {
      memcpy((*get_a2dp_sink_image_handle()),
             avrcp_subevent_now_playing_cover_art_info_get_value(packet), 7);
      printf("AVRCP Controller: Cover Art %s\n", (*get_a2dp_sink_image_handle()));
    }
    break;
#endif

  default:
    break;
  }
}

void avrcp_volume_changed(uint8_t volume) {
  const btstack_audio_sink_t *audio = btstack_audio_sink_get_instance();
  if (audio) {
    audio->set_volume(volume);
  }
}

void avrcp_target_packet_handler(uint8_t packet_type, uint16_t channel,
                                 uint8_t *packet, uint16_t size) {
  UNUSED(channel);
  UNUSED(size);

  if (packet_type != HCI_EVENT_PACKET)
    return;
  if (hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META)
    return;

  uint8_t volume;
  char const *button_state;
  avrcp_operation_id_t operation_id;

  switch (packet[2]) {
  case AVRCP_SUBEVENT_NOTIFICATION_VOLUME_CHANGED:
    volume =
        avrcp_subevent_notification_volume_changed_get_absolute_volume(packet);
    set_volume_percent(volume * 100 / 127);
    printf("AVRCP Target    : Volume set to %d%% (%d)\n", get_volume_percent(),
           volume);
    avrcp_volume_changed(volume);
    break;

  case AVRCP_SUBEVENT_OPERATION:
    operation_id =
        (avrcp_operation_id_t)avrcp_subevent_operation_get_operation_id(packet);
    button_state = avrcp_subevent_operation_get_button_pressed(packet) > 0
                       ? "PRESS"
                       : "RELEASE";
    switch (operation_id) {
    case AVRCP_OPERATION_ID_VOLUME_UP:
      printf("AVRCP Target    : VOLUME UP (%s)\n", button_state);
      break;
    case AVRCP_OPERATION_ID_VOLUME_DOWN:
      printf("AVRCP Target    : VOLUME DOWN (%s)\n", button_state);
      break;
    default:
      return;
    }
    break;
  default:
    printf("AVRCP Target    : Event 0x%02x is not parsed\n", packet[2]);
    break;
  }
}