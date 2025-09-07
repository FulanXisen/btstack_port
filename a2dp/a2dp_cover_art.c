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
    if (a2dp_sink_cover_art_download_active) {
      a2dp_sink_cover_art_file_size += size;
#ifdef HAVE_POSIX_FILE_IO
      fwrite(packet, 1, size, a2dp_sink_cover_art_file);
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
          a2dp_sink_demo_cover_art_client_connected = true;
        } else {
          printf("Cover Art       : connection failed, status 0x%02x\n",
                 status);
          a2dp_sink_demo_cover_art_cid = 0;
        }
        break;
      case AVRCP_SUBEVENT_COVER_ART_OPERATION_COMPLETE:
        if (a2dp_sink_cover_art_download_active) {
          a2dp_sink_cover_art_download_active = false;
#ifdef HAVE_POSIX_FILE_IO
          printf("Cover Art       : download of '%s complete, size %u bytes'\n",
                 a2dp_sink_demo_thumbnail_path, a2dp_sink_cover_art_file_size);
          fclose(a2dp_sink_cover_art_file);
          a2dp_sink_cover_art_file = NULL;
#else
          printf("Cover Art: download completed\n");
#endif
        }
        break;
      case AVRCP_SUBEVENT_COVER_ART_CONNECTION_RELEASED:
        a2dp_sink_demo_cover_art_client_connected = false;
        a2dp_sink_demo_cover_art_cid = 0;
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
      &a2dp_sink_demo_cover_art_client, a2dp_sink_demo_cover_art_packet_handler,
      *get_device_addr(), a2dp_sink_demo_ertm_buffer,
      sizeof(a2dp_sink_demo_ertm_buffer), &a2dp_sink_demo_ertm_config,
      &a2dp_sink_demo_cover_art_cid);
  return status;
}

#endif