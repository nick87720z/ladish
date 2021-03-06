/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * LADI Session Handler (ladish)
 *
 * Copyright (C) 2009, 2010, 2011 Nedko Arnaudov <nedko@arnaudov.name>
 *
 **************************************************************************
 * This file contains interface to the D-Bus patchbay interface helpers
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

#ifndef PATCHBAY_H__30334B9A_8847_4E8C_AFF9_73DB13406C8E__INCLUDED
#define PATCHBAY_H__30334B9A_8847_4E8C_AFF9_73DB13406C8E__INCLUDED

#include "client.h"
#include "port.h"

typedef struct ladish_graph_tag { int unused; } * ladish_graph_handle;

typedef
bool
(* ladish_graph_connect_request_handler)(
  void * context,
  ladish_graph_handle graph_handle,
  ladish_port_handle port1,
  ladish_port_handle port2);

typedef
bool
(* ladish_graph_disconnect_request_handler)(
  void * context,
  ladish_graph_handle graph_handle,
  uint64_t connection_id);

typedef void (* ladish_graph_simple_port_callback)(ladish_port_handle port_handle);

bool ladish_graph_create(ladish_graph_handle * graph_handle_ptr, const char * opath);
bool ladish_graph_copy(ladish_graph_handle src, ladish_graph_handle dest);
void ladish_graph_destroy(ladish_graph_handle graph_handle);

const char * ladish_graph_get_opath(ladish_graph_handle graph_handle);
const char * ladish_graph_get_description(ladish_graph_handle graph_handle);

void
ladish_graph_set_connection_handlers(
  ladish_graph_handle graph_handle,
  void * graph_context,
  ladish_graph_connect_request_handler connect_handler,
  ladish_graph_disconnect_request_handler disconnect_handler);

void ladish_graph_clear(ladish_graph_handle graph_handle, ladish_graph_simple_port_callback port_callback);
void * ladish_graph_get_dbus_context(ladish_graph_handle graph_handle);
ladish_dict_handle ladish_graph_get_dict(ladish_graph_handle graph_handle);
ladish_dict_handle ladish_graph_get_connection_dict(ladish_graph_handle graph_handle, uint64_t connection_id);
bool ladish_graph_add_client(ladish_graph_handle graph_handle, ladish_client_handle client_handle, const char * name, bool hidden);

void
ladish_graph_remove_client(
  ladish_graph_handle graph_handle,
  ladish_client_handle client_handle);

bool
ladish_graph_rename_client(
  ladish_graph_handle graph_handle,
  ladish_client_handle client_handle,
  const char * new_client_name);

bool
ladish_graph_add_port(
  ladish_graph_handle graph_handle,
  ladish_client_handle client_handle,
  ladish_port_handle port_handle,
  const char * name,
  uint32_t type,
  uint32_t flags,
  bool hidden);

ladish_client_handle
ladish_graph_remove_port(
  ladish_graph_handle graph_handle,
  ladish_port_handle port_handle);

ladish_client_handle
ladish_graph_remove_port_by_jack_id(
  ladish_graph_handle graph_handle,
  uint64_t jack_port_id,
  bool room,
  bool studio);

void
ladish_graph_move_port(
  ladish_graph_handle graph_handle,
  ladish_port_handle port_handle,
  ladish_client_handle client_handle);

bool
ladish_graph_rename_port(
  ladish_graph_handle graph_handle,
  ladish_port_handle port_handle,
  const char * new_port_name);

uint64_t
ladish_graph_add_connection(
  ladish_graph_handle graph_handle,
  ladish_port_handle port1_handle,
  ladish_port_handle port2_handle,
  bool hidden);

void
ladish_graph_remove_connection(
  ladish_graph_handle graph_handle,
  uint64_t connection_id,
  bool force);

bool
ladish_graph_get_connection_ports(
  ladish_graph_handle graph_handle,
  uint64_t connection_id,
  ladish_port_handle * port1_handle_ptr,
  ladish_port_handle * port2_handle_ptr);

bool
ladish_graph_find_connection(
  ladish_graph_handle graph_handle,
  ladish_port_handle port1_handle,
  ladish_port_handle port2_handle,
  uint64_t * connection_id_ptr);

ladish_client_handle ladish_graph_find_client_by_id(ladish_graph_handle graph_handle, uint64_t client_id);
ladish_port_handle ladish_graph_find_port_by_id(ladish_graph_handle graph_handle, uint64_t port_id);
ladish_client_handle ladish_graph_find_client_by_jack_id(ladish_graph_handle graph_handle, uint64_t client_id);
ladish_port_handle ladish_graph_find_port_by_jack_id(ladish_graph_handle graph_handle, uint64_t port_id, bool room, bool studio);
ladish_client_handle ladish_graph_find_client_by_name(ladish_graph_handle graph_handle, const char * name, bool appless);
ladish_client_handle ladish_graph_find_client_by_app(ladish_graph_handle graph_handle, const uuid_t app_uuid);
ladish_port_handle ladish_graph_find_port_by_name(ladish_graph_handle graph_handle, ladish_client_handle client_handle, const char * name, void * vgraph_filter);
ladish_client_handle ladish_graph_find_client_by_uuid(ladish_graph_handle graph_handle, const uuid_t uuid);
ladish_port_handle ladish_graph_find_port_by_uuid(ladish_graph_handle graph_handle, const uuid_t uuid, bool use_link_override_uuids, void * vgraph_filter);

ladish_port_handle
ladish_graph_find_client_port_by_uuid(
  ladish_graph_handle graph,
  ladish_client_handle client,
  const uuid_t uuid,
  bool use_link_override_uuids);

void
ladish_graph_set_link_port_override_uuid(
  ladish_graph_handle graph,
  ladish_port_handle port,
  const uuid_t override_uuid);

ladish_client_handle ladish_graph_get_port_client(ladish_graph_handle graph_handle, ladish_port_handle port_handle);
uint64_t ladish_graph_get_client_id(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
const char * ladish_graph_get_client_name(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
const char * ladish_graph_get_port_name(ladish_graph_handle graph, ladish_port_handle port);
bool ladish_graph_client_is_empty(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
bool ladish_graph_client_looks_empty(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
bool ladish_graph_client_is_hidden(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
bool ladish_graph_is_port_present(ladish_graph_handle graph_handle, ladish_port_handle port_handle);
void ladish_graph_show_port(ladish_graph_handle graph_handle, ladish_port_handle port_handle);
void ladish_graph_hide_port(ladish_graph_handle graph_handle, ladish_port_handle port_handle);
void ladish_graph_show_client(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
void ladish_graph_hide_client(ladish_graph_handle graph_handle, ladish_client_handle client_handle);
void ladish_graph_adjust_port(ladish_graph_handle graph_handle, ladish_port_handle port_handle, uint32_t type, uint32_t flags);
void ladish_graph_show_connection(ladish_graph_handle graph_handle, uint64_t connection_id);
void ladish_try_connect_hidden_connections(ladish_graph_handle graph_handle);
bool ladish_disconnect_visible_connections(ladish_graph_handle graph_handle);
void ladish_graph_hide_non_virtual(ladish_graph_handle graph_handle);
void ladish_graph_get_port_uuid(ladish_graph_handle graph, ladish_port_handle port, uuid_t uuid_ptr);
bool ladish_graph_client_has_visible_app_port(ladish_graph_handle graph, ladish_client_handle client, const uuid_t app_uuid);
bool ladish_graph_client_has_visible_ports(ladish_graph_handle graph, ladish_client_handle client);

void ladish_graph_dump(ladish_graph_handle graph_handle);

bool
ladish_graph_iterate_nodes(
  ladish_graph_handle graph_handle,
  void * callback_context,
  bool
  (* client_begin_callback)(
    void * context,
    ladish_graph_handle graph_handle,
    bool hidden,
    ladish_client_handle client_handle,
    const char * client_name,
    void ** client_iteration_context_ptr_ptr),
  bool
  (* port_callback)(
    void * context,
    ladish_graph_handle graph_handle,
    bool hidden,
    void * client_iteration_context_ptr,
    ladish_client_handle client_handle,
    const char * client_name,
    ladish_port_handle port_handle,
    const char * port_name,
    uint32_t port_type,
    uint32_t port_flags),
  bool
  (* client_end_callback)(
    void * context,
    ladish_graph_handle graph_handle,
    bool hidden,
    ladish_client_handle client_handle,
    const char * client_name,
    void * client_iteration_context_ptr));

bool
ladish_graph_iterate_connections(
  ladish_graph_handle graph_handle,
  void * callback_context,
  bool (* callback)(
    void * context,
    ladish_graph_handle graph_handle,
    bool connection_hidden,
    ladish_client_handle client1_handle,
    ladish_port_handle port1_handle,
    bool port1_hidden,
    ladish_client_handle client2_handle,
    ladish_port_handle port2_handle,
    bool port2_hidden,
    ladish_dict_handle dict));

bool
ladish_graph_interate_client_ports(
  ladish_graph_handle graph_handle,
  ladish_client_handle client_handle,
  void * callback_context,
  bool
  (* port_callback)(
    void * context,
    ladish_graph_handle graph_handle,
    bool hidden,
    ladish_client_handle client_handle,
    const char * client_name,
    ladish_port_handle port_handle,
    const char * port_name,
    uint32_t port_type,
    uint32_t port_flags));

void ladish_graph_clear_persist(ladish_graph_handle graph_handle);
void ladish_graph_set_persist(ladish_graph_handle graph_handle);
bool ladish_graph_is_persist(ladish_graph_handle graph_handle);
bool ladish_graph_looks_empty(ladish_graph_handle graph_handle);
bool ladish_graph_has_visible_connections(ladish_graph_handle graph_handle);

void ladish_graph_remove_hidden_objects(ladish_graph_handle graph_handle);

void ladish_graph_trick_dicts(ladish_graph_handle graph_handle);

extern const struct cdbus_interface_descriptor g_interface_patchbay;

#endif /* #ifndef PATCHBAY_H__30334B9A_8847_4E8C_AFF9_73DB13406C8E__INCLUDED */
