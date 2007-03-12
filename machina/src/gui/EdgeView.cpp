/* This file is part of Machina.
 * Copyright (C) 2007 Dave Robillard <http://drobilla.net>
 * 
 * Machina is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * Machina is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <iomanip>
#include <sstream>
#include <flowcanvas/FlowCanvas.h>
#include <machina/Edge.hpp>
#include "EdgeView.hpp"
#include "NodeView.hpp"

using namespace LibFlowCanvas;

EdgeView::EdgeView(SharedPtr<FlowCanvas>    canvas,
                   SharedPtr<NodeView>      src,
                   SharedPtr<NodeView>      dst,
                   SharedPtr<Machina::Edge> edge)
	: LibFlowCanvas::Connection(canvas, src, dst, 0x9999AAff, true)
	, _edge(edge)
{
}


void
EdgeView::update_label()
{
	char label[4];
	snprintf(label, 4, "%3f", _edge->probability());
	set_label(label);
}


bool
EdgeView::on_event(GdkEvent* ev)
{
	using namespace std;

	if (ev->type == GDK_BUTTON_PRESS) {
		if (ev->button.button == 1) {
			_edge->set_probability(_edge->probability() - 0.1);
			update_label();
			return true;
		} else if (ev->button.button == 3) {
			_edge->set_probability(_edge->probability() + 0.1);
			update_label();
			return true;
		}
	}

	return false;
}


