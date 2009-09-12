/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * LADI Session Handler (ladish)
 *
 * Copyright (C) 2009 Nedko Arnaudov <nedko@arnaudov.name>
 *
 **************************************************************************
 * This file contains implementation of the graph dispatcher object
 **************************************************************************
 *
 * LADI Session Handler is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LADI Session Handler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LADI Session Handler. If not, see <http://www.gnu.org/licenses/>
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "jack_dispatch.h"
#include "../dbus_constants.h"

struct jack_dispatcher
{
  graph_proxy_handle jack_graph;
  ladish_graph_handle studio_graph;
  uint64_t system_client_id;
  ladish_client_handle system_capture_client;
  ladish_client_handle system_playback_client;
};

/* 47c1cd18-7b21-4389-bec4-6e0658e1d6b1 */
UUID_DEFINE(g_system_capture_guid,0x47,0xC1,0xCD,0x18,0x7B,0x21,0x43,0x89,0xBE,0xC4,0x6E,0x06,0x58,0xE1,0xD6,0xB1);

/* b2a0bb06-28d8-4bfe-956e-eb24378f9629 */
UUID_DEFINE(g_system_playback_guid,0xB2,0xA0,0xBB,0x06,0x28,0xD8,0x4B,0xFE,0x95,0x6E,0xEB,0x24,0x37,0x8F,0x96,0x29);

#define dispatcher_ptr ((struct jack_dispatcher *)context)

static void clear(void * context)
{
  lash_info("clear");
}

static void client_appeared(void * context, uint64_t id, const char * name)
{
  ladish_client_handle client;

  lash_info("client_appeared(%llu, %s)", (unsigned long long)id, name);

  if (strcmp(name, "system") == 0)
  {
    dispatcher_ptr->system_client_id = id;
  }
  else
  {
    ladish_client_create(NULL, true, false, true, &client);
    ladish_graph_add_client(dispatcher_ptr->studio_graph, client, name);
  }
}

static void client_disappeared(void * context, uint64_t id)
{
  lash_info("client_disappeared(%llu)", (unsigned long long)id);

  if (id == dispatcher_ptr->system_client_id)
  {
    dispatcher_ptr->system_client_id = 0;
  }
}

static void port_appeared(void * context, uint64_t client_id, uint64_t port_id, const char * port_name, bool is_input, bool is_terminal, bool is_midi)
{
  ladish_client_handle client;
  ladish_port_handle port;
  uint32_t type;
  uint32_t flags;

  lash_info("port_appeared(%llu, %llu, %s (%s, %s))", (unsigned long long)client_id, (unsigned long long)port_id, port_name, is_input ? "in" : "out", is_midi ? "midi" : "audio");

  if (client_id == dispatcher_ptr->system_client_id)
  {
    if (!is_input)
    {
      if (dispatcher_ptr->system_capture_client == NULL)
      {
        if (!ladish_client_create(g_system_capture_guid, true, false, true, &dispatcher_ptr->system_capture_client))
        {
          lash_error("ladish_client_create() failed.");
          return;
        }

        if (!ladish_graph_add_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_capture_client, "Hardware Capture"))
        {
          lash_error("ladish_graph_add_client() failed.");
          ladish_graph_remove_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_capture_client, true);
          dispatcher_ptr->system_capture_client = NULL;
          return;
        }
      }

      client = dispatcher_ptr->system_capture_client;
    }
    else
    {
      if (dispatcher_ptr->system_playback_client == NULL)
      {
        if (!ladish_client_create(g_system_playback_guid, true, false, true, &dispatcher_ptr->system_playback_client))
        {
          lash_error("ladish_client_create() failed.");
          return;
        }

        if (!ladish_graph_add_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_playback_client, "Hardware Playback"))
        {
          ladish_graph_remove_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_playback_client, true);
          dispatcher_ptr->system_playback_client = NULL;
          return;
        }
      }

      client = dispatcher_ptr->system_playback_client;
    }
  }
  else
  {
    return; /* TODO: find client by client_id */
  }

  type = is_midi ? JACKDBUS_PORT_TYPE_MIDI : JACKDBUS_PORT_TYPE_AUDIO;
  flags = is_input ? JACKDBUS_PORT_FLAG_INPUT : JACKDBUS_PORT_FLAG_OUTPUT;
  if (is_terminal)
  {
    flags |= JACKDBUS_PORT_FLAG_TERMINAL;
  }

  if (!ladish_port_create(NULL, &port))
  {
    lash_error("ladish_port_create() failed.");
    return;
  }

  if (!ladish_graph_add_port(dispatcher_ptr->studio_graph, client, port, port_name, type, flags))
  {
    lash_error("ladish_graph_add_port() failed.");
    return;
  }
}

static void port_disappeared(void * context, uint64_t client_id, uint64_t port_id)
{
  lash_info("port_disappeared(%llu)", (unsigned long long)port_id);
}

static void ports_connected(void * context, uint64_t client1_id, uint64_t port1_id, uint64_t client2_id, uint64_t port2_id)
{
  lash_info("ports_connected");
}

static void ports_disconnected(void * context, uint64_t client1_id, uint64_t port1_id, uint64_t client2_id, uint64_t port2_id)
{
  lash_info("ports_disconnected");
}

#undef dispatcher_ptr

bool
ladish_jack_dispatcher_create(
  graph_proxy_handle jack_graph,
  ladish_graph_handle studio_graph,
  ladish_jack_dispatcher_handle * handle_ptr)
{
  struct jack_dispatcher * dispatcher_ptr;

  dispatcher_ptr = malloc(sizeof(struct jack_dispatcher));
  if (dispatcher_ptr == NULL)
  {
    lash_error("malloc() failed for struct jack_dispatcher");
    return false;
  }

  dispatcher_ptr->jack_graph = jack_graph;
  dispatcher_ptr->studio_graph = studio_graph;
  dispatcher_ptr->system_client_id = 0;
  dispatcher_ptr->system_capture_client = NULL;
  dispatcher_ptr->system_playback_client = NULL;

  if (!graph_proxy_attach(
        jack_graph,
        dispatcher_ptr,
        clear,
        client_appeared,
        client_disappeared,
        port_appeared,
        port_disappeared,
        ports_connected,
        ports_disconnected))
  {
    free(dispatcher_ptr);
    return false;
  }

  *handle_ptr = (ladish_jack_dispatcher_handle)dispatcher_ptr;
  return true;
}

#define dispatcher_ptr ((struct jack_dispatcher *)handle)

void
ladish_jack_dispatcher_destroy(
  ladish_jack_dispatcher_handle handle)
{
  graph_proxy_detach((graph_proxy_handle)handle, dispatcher_ptr);

  if (dispatcher_ptr->system_capture_client != NULL)
  {
    ladish_graph_remove_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_capture_client, true);
  }

  if (dispatcher_ptr->system_playback_client != NULL)
  {
    ladish_graph_remove_client(dispatcher_ptr->studio_graph, dispatcher_ptr->system_playback_client, true);
  }
}

#undef dispatcher_ptr