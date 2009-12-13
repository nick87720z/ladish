/* -*- Mode: C ; c-basic-offset: 2 -*- */
/*
 * LADI Session Handler (ladish)
 *
 * Copyright (C) 2009 Nedko Arnaudov <nedko@arnaudov.name>
 *
 **************************************************************************
 * This file contains implementation of the "start studio" command
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

#include "cmd.h"
#include "studio_internal.h"
#include "loader.h"

#define cmd_ptr ((struct ladish_command *)context)

static bool run(void * context)
{
  bool jack_server_started;
  unsigned int app_count;

  switch (cmd_ptr->state)
  {
  case LADISH_COMMAND_STATE_PENDING:
    if (studio_is_started())
    {
      log_info("Ignoring start request because studio is already started.");
      /* nothing to do, studio is already running */
      cmd_ptr->state = LADISH_COMMAND_STATE_DONE;
      return true;
    }

    app_count = loader_get_app_count();
    if (app_count != 0)
    {
      log_error("Ignoring start request because there are apps running.");
      log_error("This could happen when JACK has crashed or when JACK stopped unexpectedly.");
      log_error("Save your work, then unload and reload the studio.");
      return false;
    }

    log_info("Starting JACK server.");

    ladish_graph_dump(g_studio.studio_graph);

    if (!jack_proxy_start_server())
    {
      log_error("Starting JACK server failed.");
      return false;
    }

    cmd_ptr->state = LADISH_COMMAND_STATE_WAITING;
    /* fall through */
  case LADISH_COMMAND_STATE_WAITING:
    if (!ladish_environment_consume_change(&g_studio.env_store, ladish_environment_jack_server_started, &jack_server_started))
    {
      /* we are still waiting for the JACK server start */
      ASSERT(!ladish_environment_get(&g_studio.env_store, ladish_environment_jack_server_started)); /* someone else consumed the state change? */
      return true;
    }

    log_info("Wait for JACK server start complete.");

    ASSERT(jack_server_started);

    on_event_jack_started();    /* fetch configuration and announce start */

    cmd_ptr->state = LADISH_COMMAND_STATE_DONE;
    return true;
  }

  ASSERT_NO_PASS;
  return false;
}

#undef cmd_ptr

bool ladish_command_start_studio(void * call_ptr, struct ladish_cqueue * queue_ptr)
{
  struct ladish_command * cmd_ptr;

  cmd_ptr = ladish_command_new(sizeof(struct ladish_command));
  if (cmd_ptr == NULL)
  {
    lash_dbus_error(call_ptr, LASH_DBUS_ERROR_GENERIC, "ladish_command_new() failed.");
    goto fail;
  }

  cmd_ptr->run = run;

  if (!ladish_cqueue_add_command(queue_ptr, cmd_ptr))
  {
    lash_dbus_error(call_ptr, LASH_DBUS_ERROR_GENERIC, "ladish_cqueue_add_command() failed.");
    goto fail_destroy_command;
  }

  return true;

fail_destroy_command:
  free(cmd_ptr);

fail:
  return false;
}
