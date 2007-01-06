/* This file is part of Ingen.  Copyright (C) 2006 Dave Robillard.
 * 
 * Ingen is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * Ingen is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef PORT_H
#define PORT_H

#include <cassert>
#include <string>
#include <flowcanvas/Port.h>
#include "raul/SharedPtr.h"

namespace Ingen { namespace Client { class PortModel; } }
using Ingen::Client::PortModel;

namespace Ingenuity {


/** A Port on an Module.
 * 
 * \ingroup Ingenuity
 */
class Port : public LibFlowCanvas::Port
{
public:
	Port(boost::shared_ptr<LibFlowCanvas::Module> module, SharedPtr<PortModel> pm, bool flip = false, bool destroyable = false);

	virtual ~Port() {}

	SharedPtr<PortModel> model() const { return _port_model; }
	
private:

	void on_menu_destroy();

	SharedPtr<PortModel> _port_model;
};


} // namespace Ingenuity

#endif // PORT_H
