/* This file is part of Ingen.
 * Copyright (C) 2007 Dave Robillard <http://drobilla.net>
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

#include "Configuration.hpp"
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include "client/PortModel.hpp"
#include "client/PluginModel.hpp"
#include "client/PatchModel.hpp"
#include "serialisation/Loader.hpp"
#include "App.hpp"

using std::cerr; using std::cout; using std::endl;
using std::map; using std::string;
using Ingen::Client::PatchModel;

namespace Ingen {
namespace GUI {

using namespace Ingen::Client;


Configuration::Configuration()
	: _patch_path("/usr/share/ingen/patches:/usr/local/share/ingen/patches")
	// Agave FTW
	, _audio_port_color(  0x0D597FFF)
	, _control_port_color(0x2F7F0DFF)
	, _midi_port_color(   0x7F240DFF)
	, _osc_port_color(    0x5D0D7FFF)
{
}


Configuration::~Configuration()
{
}


/** Loads settings from the rc file.  Passing no parameter will load from
 * the default location.
 */
void
Configuration::load_settings(string filename)
{	
#if 0
	if (filename == "")
		filename = string(getenv("HOME")).append("/.omgtkrc");
	
	std::ifstream is;
	is.open(filename.c_str(), std::ios::in);

	if ( ! is.good()) {
		cout << "[Configuration] Unable to open settings file " << filename << endl;
		return;
	} else {
		cout << "[Configuration] Loading settings from " << filename << endl;
	}

	string s;

	is >> s;
	if (s != "file_version") {
		cerr << "[Configuration] Corrupt settings file, load aborted." << endl;
		is.close();
		return;
	}
	
	is >> s;
	if (s != "1") {
		cerr << "[Configuration] Unknown settings file version number, load aborted." << endl;
		is.close();
		return;
	}
	
	is >> s;
	if (s != "patch_path") {
		cerr << "[Configuration] Corrupt settings file, load aborted." << endl;
		is.close();
		return;
	}

	is >> s;
	_patch_path = s;
	
	is.close();
#endif
}


/** Saves settings to rc file.  Passing no parameter will save to the
 * default location.
 */
void
Configuration::save_settings(string filename)
{
#if 0
	if (filename == "")
		filename = string(getenv("HOME")).append("/.omgtkrc");
	
	std::ofstream os;
	os.open(filename.c_str(), std::ios::out);
	
	if ( ! os.good()) {
		cout << "[Configuration] Unable to write to setting file " << filename << endl;
		return;
	} else {
		cout << "[Configuration] Saving settings to " << filename << endl;
	}
	
	os << "file_version 1" << endl;
	os << "patch_path " << _patch_path << endl;
	
	os.close();
#endif
}


/** Applies the current loaded settings to whichever parts of the app
 * need updating.
 */
void
Configuration::apply_settings()
{
	//App::instance().loader()->set_patch_path(_patch_path);
}


uint32_t
Configuration::get_port_color(const PortModel* pi)
{
	assert(pi != NULL);
	
	if (pi->is_control()) {
		return _control_port_color;
	} else if (pi->is_audio()) {
		return _audio_port_color;
	} else if (pi->is_midi()) {
		return _midi_port_color;
	} else if (pi->is_osc()) {
		return _osc_port_color;
	}
	
	cerr << "[Configuration] Unknown port type " << pi->type() << ", port will appear bright red."
		<< endl;
	
	return 0xFF0000B0;
}

/*
Coord
Configuration::get_window_location(const string& id) 
{
	return _window_locations[id];
}


void
Configuration::set_window_location(const string& id, Coord loc) 
{
	_window_locations[id] = loc;
}


Coord
Configuration::get_window_size(const string& id) 
{
	return _window_sizes[id];
}


void
Configuration::set_window_size(const string& id, Coord size) 
{
	_window_sizes[id] = size;
}*/


} // namespace GUI
} // namespace Ingen
