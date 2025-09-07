#include "btstack.h"
#include "btstack_config.h"
#include "btstack_global.h"
#ifdef ENABLE_AVRCP_COVER_ART
void a2dp_sink_demo_cover_art_packet_handler(uint8_t packet_type,
                                             uint16_t channel, uint8_t *packet,
                                             uint16_t size) {
  UNUSED(channel);
  UNUSED(size);
  uint8_t status;
  uint16_t cid;
  switch (packet_type) {
  case BIP_DATA_PACKET:
    if (get_cover_art_download_status()) {
      set_cover_art_file_size(get_cover_art_file_size() + size);
#ifdef HAVE_POSIX_FILE_IO
      fwrite(packet, 1, size, get_cover_art_file());
#else
      printf("Cover art       : TODO - store %u bytes image data\n", size);
#endif
    } else {
      uint16_t i;
      for (i = 0; i < size; i++) {
        putchar(packet[i]);
      }
      printf("\n");
    }
    break;
  case HCI_EVENT_PACKET:
    switch (hci_event_packet_get_type(packet)) {
    case HCI_EVENT_AVRCP_META:
      switch (hci_event_avrcp_meta_get_subevent_code(packet)) {
      case AVRCP_SUBEVENT_COVER_ART_CONNECTION_ESTABLISHED:
        status =
            avrcp_subevent_cover_art_connection_established_get_status(packet);
        cid = avrcp_subevent_cover_art_connection_established_get_cover_art_cid(
            packet);
        if (status == ERROR_CODE_SUCCESS) {
          printf("Cover Art       : connection established, cover art cid "
                 "0x%02x\n",
                 cid);
          set_a2dp_sink_cover_art_client_connected(true);
        } else {
          printf("Cover Art       : connection failed, status 0x%02x\n",
                 status);
          set_a2dp_sink_cover_art_cid(0);
        }
        break;
      case AVRCP_SUBEVENT_COVER_ART_OPERATION_COMPLETE:
        if (get_cover_art_download_status()) {
          set_cover_art_download_status(false);
#ifdef HAVE_POSIX_FILE_IO
          printf("Cover Art       : download of '%s complete, size %u bytes'\n",
                 get_thumbnail_path(), get_cover_art_file_size());
          fclose(get_cover_art_file());
          set_cover_art_file(NULL);
#else
          printf("Cover Art: download completed\n");
#endif
        }
        break;
      case AVRCP_SUBEVENT_COVER_ART_CONNECTION_RELEASED:
        set_a2dp_sink_cover_art_client_connected(false);
        set_a2dp_sink_cover_art_cid(0);
        printf("Cover Art       : connection released 0x%02x\n",
               avrcp_subevent_cover_art_connection_released_get_cover_art_cid(
                   packet));
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

uint8_t a2dp_sink_demo_cover_art_connect(void) {
  uint8_t status;
  status = avrcp_cover_art_client_connect(
      get_a2dp_sink_cover_art_client(), a2dp_sink_demo_cover_art_packet_handler,
      *get_device_addr(), *get_a2dp_sink_ertm_buffer(),
      sizeof(*get_a2dp_sink_ertm_buffer()), get_a2dp_sink_ertm_config(),
      get_a2dp_sink_cover_art_cid_ptr());
  return status;
}

#endif