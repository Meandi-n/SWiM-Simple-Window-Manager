#ifndef XLIB_BORDER_HPP
#define XLIB_BORDER_HPP

extern "C" { 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> 
}


#include <sstream>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdlib>
#include "util.hpp"


class XLib_Border
{
public:
	GC gc;
	::std::string window_title_;

	Window border_window_;
	//Window application_window_;

	::std::unordered_map<Window, XRectangle> bordered_rectangle_map_;

	struct
	{
		unsigned int border_colour_;
		unsigned int text_colour_;
		unsigned int background_colour_;

		Position<int> border_position_; // Inherited from Window w (application)
		Size<int> border_size_; // Inherited from Window frame_ (plus window bar height)

		::std::string window_name_;

	}border_properties_;

	void createWindow(Display* display_, Window root_);
	void createRectangles(Display* display_, Window root_);
	void drawTitle(Display* display_);
	void createGC(Display* display_, Window root_);

	unsigned int border_height = 20;
};

#endif