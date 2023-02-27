#ifndef XLIB_BUTTON_HPP
#define XLIB_BUTTON_HPP

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


class XLib_Button
{
public:
	GC gc;
	
	Window button_window_;
	//Window application_window_;

	struct
	{
		unsigned int button_colour_;
		Position<int> button_position_; // Inherited from Window w (application)
		Size<int> button_size_; // Inherited from Window frame_ (plus window bar height)

	}button_properties_;

	void createWindow(Display* display_, Window root_);
	void createRectangles(Display* display_, Window root_);
	void createGC(Display* display_, Window root_);
};

#endif