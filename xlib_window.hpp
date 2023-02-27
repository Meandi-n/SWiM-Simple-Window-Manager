#ifndef XLIB_WINDOW_HPP
#define XLIB_WINDOW_HPP

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
#include <mutex>
#include <glog/logging.h>
#include "util.hpp"
#include "xlib_border.hpp"
#include "xlib_button.hpp"

class XLib_Window 
{
public:
	::std::string xlib_window_id;

	Window frame_;
	Window outer_window_;
	Window application_window_;

	XLib_Border border_;

	XLib_Button move_button_;
	XLib_Button resize_button_;
	XLib_Button close_button_;

	struct 
	{
		Position<int> window_position_;
		Size<int> window_size_;
		unsigned int border_width_;
		unsigned int window_bar_height_;
		unsigned long attrs_mask_;
		XSetWindowAttributes set_attrs;
	}window_properties_;

	XLib_Window();
	~XLib_Window();

	void createWindow(Display* display_, const Window root_);
	void resizeWindow(Display* display_, unsigned int width, unsigned int height, Window root_);
	void moveWindow(Display* display_, unsigned int x, unsigned int y, Window root_);
	void frameWindow(Display* display_, Window root_, Window w);

	::std::string toString();
};

#endif