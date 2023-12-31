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
 *         Code managing id<->mac address<->IPv6 address mapping, and doing this
 *         for different deployment scenarios: Cooja, Nodes, Indriya or Twist testbeds
 *
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki-conf.h"
#include "deployment.h"
#include "sys/node-id.h"
#include "net/rpl/rpl.h"
#include "net/mac/tsch/tsch.h"
#include "net/ip/uip-debug.h"
#include "random.h"
#include <string.h>
#include <stdio.h>
#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
#include "cc2420.h"
#endif

#if WITH_DEPLOYMENT

#if IN_COOJA
#pragma message "deployment.c: compiling with flag IN_COOJA"
#endif
#if IN_MOTES
#pragma message "deployment.c: compiling with flag IN_MOTES"
#endif
#if IN_TWIST
#pragma message "deployment.c: compiling with flag IN_TWIST"
#endif
#if IN_INDRIYA
#pragma message "deployment.c: compiling with flag IN_INDRIYA"
#endif
#if IN_NESTESTBED
#pragma message "deployment.c: compiling with flag IN_NESTESTBED"
#endif

#ifndef WITH_TSCH
#define WITH_TSCH 1
#endif

/* Our absolute index in the id_mac table */
uint16_t node_index = 0xffff;

/* Our global IPv6 prefix */
static uip_ipaddr_t prefix;

typedef union {
	uint8_t u8[2];
} linkaddr_short_t;


/* ID<->MAC address mapping */
struct id_mac {
  uint8_t id;
  linkaddr_short_t mac;
};

/* List of ID<->MAC mapping used for different deployments */
static const struct id_mac id_mac_list[] = {
    {  1, {{0x8b,0xdf}}},//H5C
    {  2, {{0xc1,0x58}}},//H5B
    {  3, {{0x83,0x6a}}},//H5D
    {  4, {{0x5c,0x43}}},//G25D
    {  5, {{0x8d,0x70}}},//G25A
    {  6, {{0xd5,0x1b}}},//G25B
    {  7, {{0x79,0xcc}}},//G25C
    {  8, {{0xb7,0x53}}},//G6A
    {  9, {{0x74,0x4e}}},//G6B
    { 10, {{0x5d,0xdd}}},//G8A
    { 11, {{0xd3,0xa0}}},//G8B
    { 12, {{0x80,0xfe}}},//G8C
    { 13, {{0x96,0xcf}}},//G8D
    { 14, {{0xb5,0xfc}}},//G8E
    { 15, {{0xa9,0xe6}}},//G8F
    { 16, {{0x6c,0x5d}}},//G10A
    { 17, {{0xb7,0x93}}},//G10B
    { 18, {{0xc7,0xbe}}},//G12A
    { 19, {{0xc5,0x1c}}},//G12B
    { 20, {{0x81,0xaf}}},//G14A
    { 21, {{0xbd,0xa2}}},//G14B
    { 22, {{0x59,0xbc}}},//G16A
    { 23, {{0x92,0x69}}},//G18A
    { 24, {{0x49,0x6b}}},//G20A
    { 25, {{0xa8,0x50}}},//G20B
    { 26, {{0x12,0x14}}},//G20C
    { 27, {{0x5a,0x19}}},//G20D
    { 28, {{0xce,0x65}}},//G20E
    { 29, {{0x57,0xde}}},//G20F
    { 30, {{0x2e,0x40}}},//G20G
    { 31, {{0xc7,0x49}}},//G20H
    { 32, {{0x0c,0x0e}}},//G19A
    { 33, {{0xbd,0xb1}}},//G21A
    { 34, {{0x5d,0x78}}},//G21B
    { 35, {{0x52,0xfe}}},//G23A
    { 36, {{0xbf,0x82}}},//G23B
    { 37, {{0x87,0x37}}},//G17A
    { 38, {{0x20,0x7b}}},//G17B
    { 39, {{0x69,0xa7}}},//G15A
    { 40, {{0xbd,0xe9}}},//G15B
    { 41, {{0x72,0xa5}}},//G13A
    { 42, {{0xb2,0xb0}}},//G13B
    { 43, {{0x58,0x43}}},//G13C
    { 44, {{0xaf,0x4b}}},//G11A
    { 45, {{0x8a,0x39}}},//G11B
    { 46, {{0x3b,0xa8}}},//G9A
    { 47, {{0xc7,0xe8}}},//G9B
    { 48, {{0x26,0xa6}}},//G7A
    { 49, {{0x78,0xf4}}},//G7B
    { 50, {{0x7e,0xfa}}},//G7C
    { 0,  {{0}}}
};

uint16_t
nodex_index_map(uint16_t index)
{
#if NODE_INDEX_SUFFLE
  return (index * NODE_INDEX_SUFFLE_MULTIPLICATOR) % NODE_INDEX_SUFFLE_MODULUS;
#else
  return index;
#endif
}

/* Returns the node's node-id */
uint16_t
get_node_id()
{
#if CONTIKI_TARGET_Z1 || CONTIKI_TARGET_JN5168
  extern unsigned char node_mac[8];
  return node_id_from_linkaddr((const linkaddr_t *)node_mac);
#else
  extern unsigned char ds2411_id[8];
  return node_id_from_linkaddr((const linkaddr_t *)&ds2411_id);
#endif
}
/* Build a global link-layer address from an IPv6 based on its UUID64 */
static void
lladdr_from_ipaddr_uuid(uip_lladdr_t *lladdr, const uip_ipaddr_t *ipaddr)
{
#if (UIP_LLADDR_LEN == 8)
  memcpy(lladdr, ipaddr->u8 + 8, UIP_LLADDR_LEN);
  lladdr->addr[0] ^= 0x02;
#else
#error orpl.c supports only EUI-64 identifiers
#endif
}
/* Returns a node-index from a node's linkaddr */
uint16_t
node_index_from_linkaddr(const linkaddr_t *addr)
{
#if IN_COOJA
  if(addr == NULL) {
    return 0xffff;
  } else {
    return nodex_index_map(node_id_from_linkaddr(addr)-1);
  }
#else /* IN_COOJA */
  if(addr == NULL) {
    return 0xffff;
  }
  const struct id_mac *curr = id_mac_list;
  while(curr->id != 0) {
    /* Assume network-wide unique 16-bit MAC addresses */
    if(curr->mac.u8[0] == addr->u8[6] && curr->mac.u8[1] == addr->u8[7]) {
      return nodex_index_map(curr - id_mac_list);
    }
    curr++;
  }
  return 0xffff;
#endif /* IN_COOJA */
}
/* Returns a node-id from a node's linkaddr */
uint16_t
node_id_from_linkaddr(const linkaddr_t *addr)
{
#if IN_COOJA
  if(addr == NULL) {
    return 0;
  } else {
    return addr->u8[7];
  }
#else /* IN_COOJA */
  if(addr == NULL) {
    return 0;
  }
  const struct id_mac *curr = id_mac_list;
  //printf("MAC-Addr: %02x %02x\n", addr->u8[6], addr->u8[7]);
  while(curr->id != 0) {
    /* Assume network-wide unique 16-bit MAC addresses */
    if(curr->mac.u8[0] == addr->u8[6] && curr->mac.u8[1] == addr->u8[7]) {
      //printf("nodeid: %u\n",curr->id);
      return curr->id;
    }
    curr++;
  }
  return 0;
#endif /* IN_COOJA */
}
/* Returns a node-id from a node's IPv6 address */
uint16_t
node_id_from_ipaddr(const uip_ipaddr_t *addr)
{
  uip_lladdr_t lladdr;
  lladdr_from_ipaddr_uuid(&lladdr, addr);
  return node_id_from_linkaddr((const linkaddr_t *)&lladdr);
}
/* Returns a node-index from a node-id */
uint16_t
get_node_index_from_id(uint16_t id)
{
#if IN_COOJA
  return nodex_index_map(id - 1);
#else
  const struct id_mac *curr = id_mac_list;
  while(curr->id != 0) {
    if(curr->id == id) {
      return nodex_index_map(curr - id_mac_list);
    }
    curr++;
  }
  return 0xffff;
#endif
}
/* Sets an IPv6 from a node-id */
void
set_ipaddr_from_id(uip_ipaddr_t *ipaddr, uint16_t id)
{
  linkaddr_t lladdr;
  memcpy(ipaddr, &prefix, 8);
  set_linkaddr_from_id(&lladdr, id);
  uip_ds6_set_addr_iid(ipaddr, (uip_lladdr_t *)&lladdr);
}
/* Sets an linkaddr from a link-layer address */
/* Sets a linkaddr from a node-id */
void
set_linkaddr_from_id(linkaddr_t *lladdr, uint16_t id)
{
#if IN_COOJA
  lladdr->u8[0] = 0x00;
  lladdr->u8[1] = 0x12;
  lladdr->u8[2] = 0x74;
  lladdr->u8[3] = id;
  lladdr->u8[4] = 0x00;
  lladdr->u8[5] = id;
  lladdr->u8[6] = id;
  lladdr->u8[7] = id;
#else
  if(id == 0 || lladdr == NULL) {
    return;
  }
  const struct id_mac *curr = id_mac_list;
  while(curr->id != 0) {
    if(curr->id == id) {
      linkaddr_copy(lladdr, &curr->mac);
      return;
    }
    curr++;
  }
#endif
}
/* Initializes global IPv6 and creates DODAG */
int
deployment_init(uip_ipaddr_t *ipaddr, uip_ipaddr_t *br_prefix, int root_id)
{
  rpl_dag_t *dag;
  
  //printf("deployment_init() starts\n");
  node_id_restore();
  node_index = get_node_index_from_id(node_id);
  //printf("node_id = %u, node_index = %u\n", node_id, node_index);

  if(node_id == 0) {
    NETSTACK_RDC.off(0);
    NETSTACK_MAC.off(0);
    return 0;
  }

#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
  NETSTACK_RADIO_set_txpower(RF_POWER);
  NETSTACK_RADIO_set_cca_threshold(RSSI_THR);
#endif
  
  if(!br_prefix) {
    uip_ip6addr(&prefix, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  } else {
    memcpy(&prefix, br_prefix, 16);
  }
  set_ipaddr_from_id(ipaddr, node_id);
  uip_ds6_addr_add(ipaddr, 0, ADDR_AUTOCONF);

  if(WITH_RPL && node_id == root_id) {
    rpl_set_root(RPL_DEFAULT_INSTANCE, ipaddr);
    dag = rpl_get_any_dag();
    rpl_set_prefix(dag, &prefix, 64);
#if WITH_TSCH
    tsch_is_coordinator = 1;
#endif
    /* If an RDC layer is used, turn it off (i.e. keep the radio on at the root) */
    NETSTACK_RDC.off(1);
  }

#if WITH_LOG
  log_start();
#endif /* WITH_LOG */

  //printf("before NETSTACK_MAC.on()\n");
  NETSTACK_MAC.on();
  //printf("deployment_init() returns\n");

  return 1;
}

#endif /* WITH_DEPLOYMENT */

