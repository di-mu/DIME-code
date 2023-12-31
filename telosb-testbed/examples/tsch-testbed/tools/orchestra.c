/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \file
 *         Orchestra
 *
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"

#include "lib/memb.h"
#include "net/packetbuf.h"
#include "net/rpl/rpl.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-private.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "deployment.h"
#include "net/rime/rime.h"
#include "tools/orchestra.h"
#include <stdio.h>

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

#if ORCHESTRA_WITH_EBSF || WITH_CTC
static struct tsch_slotframe *sf_eb;
#endif
#if ORCHESTRA_WITH_COMMON_SHARED
static struct tsch_slotframe *sf_common;
#endif
#if ORCHESTRA_WITH_RBUNICAST
static struct tsch_slotframe *sf_rb;
#endif
#if ORCHESTRA_WITH_SBUNICAST
static struct tsch_slotframe *sf_sb;
static struct tsch_slotframe *sf_sb2;
/* Delete dedicated slots after 2 minutes */
#define DEDICATED_SLOT_LIFETIME TSCH_CLOCK_TO_SLOTS(2 * 60 * CLOCK_SECOND)
/* A net-layer sniffer for packets sent and received */
static void orchestra_packet_received(void);
static void orchestra_packet_sent(int mac_status);
RIME_SNIFFER(orhcestra_sniffer, orchestra_packet_received, orchestra_packet_sent);
#endif

#if ORCHESTRA_WITH_SBUNICAST
struct link_timestamps {
  uint32_t last_tx;
  uint32_t last_rx;
};
MEMB(nbr_timestamps, struct link_timestamps, TSCH_MAX_LINKS);
/*---------------------------------------------------------------------------*/
int
orchestra_callback_do_nack(struct tsch_link *link, linkaddr_t *src, linkaddr_t *dst)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
orchestra_callback_joining_network_sf(struct tsch_slotframe *sf)
{
  /* Cleanup sf: reset timestamps. Old links remain active for DEDICATED_SLOT_LIFETIME */
  struct tsch_link *l = list_head(sf->links_list);
  while(l != NULL) {
    struct link_timestamps *ts = (struct link_timestamps *)l->data;
    if(ts != NULL) {
      ts->last_tx = ts->last_rx = current_asn.ls4b;
    }
    l = list_item_next(l);
  }
}
/*---------------------------------------------------------------------------*/
void
orchestra_callback_joining_network(void)
{
  orchestra_callback_joining_network_sf(sf_sb);
  //printf("joining_network: sf_sb done\n");
#ifdef ORCHESTRA_SBUNICAST_PERIOD2
  orchestra_callback_joining_network_sf(sf_sb2);
  //printf("joining_network: sf_sb2 done\n");
#endif
  tsch_rpl_callback_joining_network();
  //printf("joining_network: rpl done\n");
}
/*---------------------------------------------------------------------------*/
static void
orchestra_delete_old_links_sf(struct tsch_slotframe *sf)
{
  struct tsch_link *l = list_head(sf->links_list);
  struct tsch_link *prev = NULL;
  /* Loop over all links and remove old ones. */
  while(l != NULL) {
    struct link_timestamps *ts = (struct link_timestamps *)l->data;
    if(ts != NULL) {
      int tx_outdated = current_asn.ls4b - ts->last_tx > DEDICATED_SLOT_LIFETIME;
      int rx_outdated = current_asn.ls4b - ts->last_rx > DEDICATED_SLOT_LIFETIME;
      if(tx_outdated && rx_outdated) {
        /* Link outdated both for tx and rx, delete */
        PRINTF("Orchestra: removing link at %u\n", l->timeslot);
        tsch_schedule_remove_link(sf, l);
        memb_free(&nbr_timestamps, ts);
        l = prev;
      } else if(!rx_outdated && tx_outdated && (l->link_options & LINK_OPTION_TX)) {
        PRINTF("Orchestra: removing tx flag at %u\n", l->timeslot);
        /* Link outdated for tx, update */
        tsch_schedule_add_link(sf,
            l->link_options & ~(LINK_OPTION_TX | LINK_OPTION_SHARED),
            LINK_TYPE_NORMAL, &linkaddr_null,
            l->timeslot, l->channel_offset);
      } else if(!tx_outdated && rx_outdated && (l->link_options & LINK_OPTION_RX)) {
        PRINTF("Orchestra: removing rx flag at %u\n", l->timeslot);
        /* Link outdated for rx, update */
        linkaddr_t link_addr;
        linkaddr_copy(&link_addr, &l->addr);
        tsch_schedule_add_link(sf,
            l->link_options & ~LINK_OPTION_RX,
            LINK_TYPE_NORMAL, &link_addr,
            l->timeslot, l->channel_offset);
      }
    }
    prev = l;
    l = list_item_next(l);
  }
}
/*---------------------------------------------------------------------------*/
static void
orchestra_delete_old_links()
{
  orchestra_delete_old_links_sf(sf_sb);
#ifdef ORCHESTRA_SBUNICAST_PERIOD2
  orchestra_delete_old_links_sf(sf_sb2);
#endif
}
/*---------------------------------------------------------------------------*/
static void
orchestra_packet_received(void)
{
  if(packetbuf_attr(PACKETBUF_ATTR_PROTO) == UIP_PROTO_ICMP6) {
    /* Filter out ICMP */
    return;
  }

  uint16_t dest_id = node_id_from_linkaddr(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  if(dest_id != 0) { /* Not a broadcast */
    uint16_t src_id = node_id_from_linkaddr(packetbuf_addr(PACKETBUF_ADDR_SENDER));
    uint16_t src_index = get_node_index_from_id(src_id);
    /* Successful unicast Rx
     * We schedule a Rx link to listen to the source's dedicated slot,
     * in all unicast SFs */
    linkaddr_t link_addr;
    uint8_t timeslot;
    uint8_t choffset;
    static struct tsch_slotframe *sf;
#ifdef ORCHESTRA_SBUNICAST_PERIOD2
    if(src_index < ORCHESTRA_SBUNICAST_PERIOD) {
      /* Use the first slotframe */
      timeslot = src_index;
      sf = sf_sb;
      choffset = 2;
    } else {
      timeslot = src_index - ORCHESTRA_SBUNICAST_PERIOD;
      sf = sf_sb2;
      choffset = 3;
    }
#else
    sf = sf_sb;
    timeslot = src_index % ORCHESTRA_SBUNICAST_PERIOD;
    choffset = 2;
#endif
    struct link_timestamps *ts;
    uint8_t link_options = LINK_OPTION_RX;
    struct tsch_link *l = tsch_schedule_get_link_from_timeslot(sf, timeslot);
    if(l == NULL) {
      linkaddr_copy(&link_addr, &linkaddr_null);
      ts = memb_alloc(&nbr_timestamps);
    } else {
      link_options |= l->link_options;
      linkaddr_copy(&link_addr, &l->addr);
      ts = l->data;
      if(link_options != l->link_options) {
        /* Link options have changed, update the link */
        l = NULL;
      }
    }
    /* Now add/update the link */
    if(l == NULL) {
      PRINTF("Orchestra: adding rx link at %u\n", timeslot);
      l = tsch_schedule_add_link(sf,
          link_options,
          LINK_TYPE_NORMAL, &link_addr,
          timeslot, choffset);
    } else {
      PRINTF("Orchestra: updating rx link at %u\n", timeslot);
    }
    /* Update Rx timestamp */
    if(l != NULL && ts != NULL) {
      ts->last_rx = current_asn.ls4b;
      l->data = ts;
    }
  }
  orchestra_delete_old_links();
}
/*---------------------------------------------------------------------------*/
static void
orchestra_packet_sent(int mac_status)
{
  if(packetbuf_attr(PACKETBUF_ATTR_PROTO) == UIP_PROTO_ICMP6) {
    /* Filter out ICMP */
    return;
  }
  uint16_t dest_id = node_id_from_linkaddr(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  uint16_t dest_index = get_node_index_from_id(dest_id);
  if(dest_index != 0xffff && mac_status == MAC_TX_OK) {
    /* Successful unicast Tx
     * We schedule a Tx link to this neighbor, in all unicast SFs */
    uint8_t timeslot;
    uint8_t choffset;
    static struct tsch_slotframe *sf;
#ifdef ORCHESTRA_SBUNICAST_PERIOD2
    if(node_index < ORCHESTRA_SBUNICAST_PERIOD) {
      /* Use the first slotframe */
      timeslot = node_index;
      sf = sf_sb;
      choffset = 2;
    } else {
      timeslot = node_index - ORCHESTRA_SBUNICAST_PERIOD;
      sf = sf_sb2;
      choffset = 3;
    }
#else
    sf = sf_sb;
    timeslot = node_index % ORCHESTRA_SBUNICAST_PERIOD;
#endif
    struct link_timestamps *ts;
    uint8_t link_options = LINK_OPTION_TX | (ORCHESTRA_SBUNICAST_SHARED ? LINK_OPTION_SHARED : 0);
    struct tsch_link *l = tsch_schedule_get_link_from_timeslot(sf, timeslot);
    if(l == NULL) {
      ts = memb_alloc(&nbr_timestamps);
    } else {
      link_options |= l->link_options;
      ts = l->data;
      if(link_options != l->link_options
          || !linkaddr_cmp(&l->addr, packetbuf_addr(PACKETBUF_ADDR_RECEIVER))) {
        /* Link options or address have changed, update the link */
        l = NULL;
      }
    }
    /* Now add/update the link */
    if(l == NULL) {
      PRINTF("Orc: adding tx link at %u\n", timeslot);
      l = tsch_schedule_add_link(sf,
          link_options,
          LINK_TYPE_NORMAL, packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
          timeslot, choffset);
    } else {
      PRINTF("Orc: update tx link at %u\n", timeslot);
    }
    /* Update Tx timestamp */
    if(l != NULL && ts != NULL) {
      ts->last_tx = current_asn.ls4b;
      l->data = ts;
    }
  }
  orchestra_delete_old_links();
}
#endif /* ORCHESTRA_WITH_SBUNICAST */

void
orchestra_callback_new_time_source(struct tsch_neighbor *old, struct tsch_neighbor *new)
{
  uint16_t old_id = node_id_from_linkaddr(&old->addr);
  uint16_t old_index = get_node_index_from_id(old_id);
  uint16_t new_id = node_id_from_linkaddr(&new->addr);
  uint16_t new_index = get_node_index_from_id(new_id);

  if(new_index == old_index) {
    return;
  }

#if ORCHESTRA_WITH_EBSF
  if(old_index != 0xffff) {
    PRINTF("Orchestra: removing rx link for %u (%u) EB\n", old_id, old_index);
    /* Stop listening to the old time source's EBs */
    tsch_schedule_remove_link_from_timeslot(sf_eb, old_index);
  }
  if(new_index != 0xffff) {
    /* Listen to the time source's EBs */
    PRINTF("Orchestra: adding rx link for %u (%u) EB\n", new_id, new_index);
    tsch_schedule_add_link(sf_eb,
        LINK_OPTION_RX,
        LINK_TYPE_ADVERTISING_ONLY, NULL,
        new_index, 0);
  }
#endif /* ORCHESTRA_WITH_EBSF */

#if ORCHESTRA_WITH_RBUNICAST
  if(old_index != 0xffff) {
    /* Shared-slot: remove unicast Tx link to the old time source */
    if(old_index % ORCHESTRA_RBUNICAST_PERIOD == node_index % ORCHESTRA_RBUNICAST_PERIOD) {
      /* This same link is also our unicast Rx link! Instead of removing it, we update it */
      PRINTF("Orchestra: removing tx option to link for %u (%u) unicast\n", old_id, old_index);
      tsch_schedule_add_link(sf_rb,
          LINK_OPTION_RX,
          LINK_TYPE_NORMAL, &tsch_broadcast_address,
          node_index % ORCHESTRA_RBUNICAST_PERIOD, 2);
    } else {
      PRINTF("Orchestra: removing tx link for %u (%u) unicast\n", old_id, old_index);
      tsch_schedule_remove_link_from_timeslot(sf_rb, old_index % ORCHESTRA_RBUNICAST_PERIOD);
    }
  }

  if(new_index != 0xffff) {
    /* Shared-slot: schedule a shared Tx link to the new time source */
    PRINTF("Orchestra: adding tx (rx=%d) link for %u (%u) unicast\n",
        new_index % ORCHESTRA_RBUNICAST_PERIOD == node_index % ORCHESTRA_RBUNICAST_PERIOD,
        new_id, new_index);
    tsch_schedule_add_link(sf_rb,
        LINK_OPTION_TX | LINK_OPTION_SHARED
        /* If the source's timeslot and ours are the same, we must not only Tx but also Rx */
        | ((new_index % ORCHESTRA_RBUNICAST_PERIOD == node_index % ORCHESTRA_RBUNICAST_PERIOD) ? LINK_OPTION_RX : 0),
        LINK_TYPE_NORMAL, &new->addr,
        new_index % ORCHESTRA_RBUNICAST_PERIOD, 2);
  }
#endif /* ORCHESTRA_WITH_RBUNICAST */
}

extern uint8_t src_dst_flow[2];
uint8_t src_dst_flow[2] = {0xff, 0xff};
uint8_t dl_parent = 0, dl_flow_child[N_FLOW] = {0};
extern uint32_t dl_fwdflow_bitmap;
uint32_t dl_fwdflow_bitmap = 0;
const uint8_t src_dst_nodes[N_FLOW][2] = {
	{43,42}, {45,44}, {4,10}, {19,16},
	{23,31}, {24,26}, {33,34}, {40,36}
};

#define MAX_DL_FWDS 8
#define REMOVE_NODES 18
#if REMOVE_NODES == 0
const uint8_t dl_fwd_nodes[N_FLOW][MAX_DL_FWDS] = {
	{39,41}, {39,41,45}, {39,41,45,44,4},
	{20,21,18}, {20,22,23}, {20,22,23,24},
	{20,22,23,24,32}, {37,38}
};
#endif
#if REMOVE_NODES == 6
const uint8_t dl_fwd_nodes[N_FLOW][MAX_DL_FWDS] = {
	{39}, {39,45}, {39,45,44,4},
	{21,18}, {22,23}, {22,23,24},
	{22,23,24,32}, {38}
};
#endif
#if REMOVE_NODES == 12
const uint8_t dl_fwd_nodes[N_FLOW][MAX_DL_FWDS] = {
	{39}, {39,45}, {39,45,44,4},
	{18}, {23}, {23,24},
	{23,24}, {38}
};
#endif
#if REMOVE_NODES == 18
const uint8_t dl_fwd_nodes[N_FLOW][MAX_DL_FWDS] = {
	{0}, {45}, {45,44,4},
	{0}, {23}, {23,24},
	{23,24}, {0}
};
#endif

void dataflow_init() {
	uint8_t ii, jj, kk;
	for (ii = 0; ii < N_FLOW; ii++) {
		if (node_id == 1) {
			dl_flow_child[ii] = dl_fwd_nodes[ii][0] ? dl_fwd_nodes[ii][0] : src_dst_nodes[ii][1];
			continue;
		}
		for (jj = 0; jj < 2; jj++) {
			if (node_id != src_dst_nodes[ii][jj]) continue;
			src_dst_flow[jj] = ii;
			if (!jj) continue;
			for (kk = 0; kk < MAX_DL_FWDS; kk++) {
				if (!dl_fwd_nodes[ii][kk]) break;
			}
			dl_parent = kk ? dl_fwd_nodes[ii][kk-1] : 1;
		}
		for (jj = 0; jj < MAX_DL_FWDS; jj++) {
			if (node_id != dl_fwd_nodes[ii][jj]) continue;
			dl_fwdflow_bitmap |= 1UL << ii;
			dl_parent = jj ? dl_fwd_nodes[ii][jj-1] : 1;
			dl_flow_child[ii] = jj < MAX_DL_FWDS - 1 && dl_fwd_nodes[ii][jj+1] ? dl_fwd_nodes[ii][jj+1] : src_dst_nodes[ii][1];
			break;
		}
	}
}
/*
struct SlotPair {
	uint8_t nodeid;
	uint8_t slot[2]; //RX,TX
};

const struct SlotPair dl_node[3] = {
	{1, {0,1}},
	{34,{1,34}},
	{4, {34,0}}
};
*/
extern uint8_t dl_slot_rx, dl_slot_tx[];
uint8_t dl_slot_rx = 0, dl_slot_tx[N_FLOW] = {0};
extern const uint8_t dl_slot_0;
const uint8_t dl_slot_0 = 50;

void add_downlink(struct tsch_slotframe *sf) {
	uint8_t ii, jj;
	if (node_id == 1 || dl_fwdflow_bitmap) {
		for (ii = 0; ii < N_FLOW; ii++) {
			if (!dl_flow_child[ii]) continue;
#ifdef DL_SENDER_BASED
			dl_slot_tx[ii] = dl_slot_0 + node_id;
#else //Receiver based downlink by default
			dl_slot_tx[ii] = dl_slot_0 + dl_flow_child[ii];
#endif
			for (jj = 0; jj < ii; jj++) {
				if (dl_slot_tx[ii] == dl_slot_tx[jj]) goto NEXT;
			}
			tsch_schedule_add_link(sf, LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, dl_slot_tx[ii], 2);
NEXT:;
		}
	}
	if (dl_parent) {
#ifdef DL_SENDER_BASED
		dl_slot_rx = dl_slot_0 + dl_parent;
#else //Receiver based downlink by default
		dl_slot_rx = dl_slot_0 + node_id;
#endif
		tsch_schedule_add_link(sf, LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, dl_slot_rx, 2);
	}
#if WITH_CTC
	if (src_dst_flow[1] != 0xff) {
		tsch_schedule_add_link(sf, LINK_OPTION_RX, LINK_TYPE_ADVERTISING_ONLY, NULL, dl_slot_0, 2);
	}
#endif
/*for (ii=0; ii < dl_nodes; ii++) {
		if (node_id != dl_node[ii].nodeid) continue;
		for (jj=0; jj < 2; jj++) {
			if (!dl_node[ii].slot[jj]) continue;
			dl_slot[jj] = dl_node[ii].slot[jj] + dl_slot_0;
			tsch_schedule_add_link(sf, LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, dl_slot[jj], 2);
		}
	}*/
}

void
orchestra_init()
{
#if ORCHESTRA_WITH_EBSF || WITH_CTC
  sf_eb = tsch_schedule_add_slotframe(0, ORCHESTRA_EBSF_PERIOD);
#endif

#if ORCHESTRA_WITH_EBSF
  /* EB link: every neighbor uses its own to avoid contention */
#if WITH_CTC
//  if (node_index) //root node doesn't send EB (CTC beacons at slot 0)
#endif
    tsch_schedule_add_link(sf_eb,
      LINK_OPTION_TX,
      LINK_TYPE_ADVERTISING_ONLY, &tsch_broadcast_address,
      node_index, 0);
#endif

#if WITH_CTC
	tsch_schedule_add_link(sf_eb, LINK_OPTION_RX, LINK_TYPE_ADVERTISING_ONLY, NULL, 0, 0);
#endif

#if ORCHESTRA_WITH_RBUNICAST
  /* Receiver-based slotframe for unicast */
  sf_rb = tsch_schedule_add_slotframe(2, ORCHESTRA_RBUNICAST_PERIOD);
  /* Rx link, dedicated to us */
  /* Tx links are added from tsch_callback_new_time_source */
  tsch_schedule_add_link(sf_rb,
      LINK_OPTION_RX,
      LINK_TYPE_NORMAL, &tsch_broadcast_address,
      node_index % ORCHESTRA_RBUNICAST_PERIOD, 2);
#endif

#if ORCHESTRA_WITH_SBUNICAST
  memb_init(&nbr_timestamps);
  dataflow_init();
  /* Sender-based slotframe for unicast */
  sf_sb = tsch_schedule_add_slotframe(2, ORCHESTRA_SBUNICAST_PERIOD);
  add_downlink(sf_sb);
#ifdef ORCHESTRA_SBUNICAST_PERIOD2
  sf_sb2 = tsch_schedule_add_slotframe(3, ORCHESTRA_SBUNICAST_PERIOD2);
  //add_downlink(sf_sb2);
#endif
  /* Rx links (with lease time) will be added upon receiving unicast */
  /* Tx links (with lease time) will be added upon transmitting unicast (if ack received) */
  rime_sniffer_add(&orhcestra_sniffer);
#endif

#if ORCHESTRA_WITH_COMMON_SHARED
  /* Default slotframe: for broadcast or unicast to neighbors we
   * do not have a link to */
  sf_common = tsch_schedule_add_slotframe(1, ORCHESTRA_COMMON_SHARED_PERIOD);
  tsch_schedule_add_link(sf_common,
      LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
      ORCHESTRA_COMMON_SHARED_TYPE, &tsch_broadcast_address,
      0, 1);
#endif
}
