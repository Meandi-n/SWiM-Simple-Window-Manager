#include "xlib_button.hpp"

void XLib_Button::createWindow(Display* display_, Window root_)
{
	button_window_ = XCreateSimpleWindow(
		display_,
		root_,
		button_properties_.button_position_.x,
		button_properties_.button_position_.y,
		button_properties_.button_size_.width,
		button_properties_.button_size_.height,
		0, //Border width 
		0,
		0
		);
}

void XLib_Button::createRectangles(Display* display_, Window root_)
{
	createGC(display_, root_);
	XFillRectangle(display_, button_window_, gc, 0, 0, 
		button_properties_.button_size_.width, button_properties_.button_size_.height);
}

void XLib_Button::createGC(Display* display_, Window root_)
{
	unsigned long valuemask = 0;

	XGCValues values;	// initial values for the GC
	unsigned int line_width = 1; // line width of the GC
	int line_style = LineSolid;
	int cap_style = CapButt;
	int join_style = JoinBevel;
	int screen_num = DefaultScreen(display_);

	gc = XCreateGC(display_, button_window_, valuemask, &values);

	XSetForeground(display_, gc, button_properties_.button_colour_);
	XSetBackground(display_, gc, button_properties_.button_colour_);

	XSetLineAttributes(display_, gc, line_width, line_style, cap_style, join_style);

	XSetFillStyle(display_, gc, FillSolid);
}