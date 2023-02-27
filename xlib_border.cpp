#include "xlib_border.hpp"

void XLib_Border::createWindow(Display* display_, Window root_)
{
	XVisualInfo vinfo;
	XMatchVisualInfo(display_, DefaultScreen(display_), 32, TrueColor, &vinfo);
	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(display_, root_, vinfo.visual, AllocNone);
	attr.border_pixel = border_properties_.border_colour_;
	attr.background_pixel = border_properties_.background_colour_;

	border_window_ = XCreateWindow(display_, root_, 
		border_properties_.border_position_.x, 
		border_properties_.border_position_.y, 
		border_properties_.border_size_.width, 
		border_properties_.border_size_.height + border_height, 0, 
		vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);

}
void XLib_Border::createRectangles(Display* display_, Window root_)
{
	createGC(display_, root_);
	XFillRectangle(display_, border_window_, gc, 0, 0, 
		border_properties_.border_size_.width * 2 / 3, border_height);
	drawTitle(display_);
}
void XLib_Border::drawTitle(Display* display_)
{
	XSetForeground(display_, gc, 0x000000);
	XFontStruct* myFont = XLoadQueryFont(display_, 
		"-adobe-helvetica-bold-r-normal--0-0-0-0-p-0-iso8859-15"); // FONT CURRENTLY NOT
	XSetFont(display_, gc, myFont->fid);
	std::string title = border_properties_.window_name_;
	XDrawString(display_, border_window_, gc, border_height/2, 12, title.c_str(), title.length());
}
// how to create graphics context?? what window is needed aside from border_window_???
void XLib_Border::createGC(Display* display_, Window root_)
{
	unsigned long valuemask = 0;
	XGCValues values;	// initial values for the GC
	unsigned int line_width = 1; // line width of the GC
	int line_style = LineSolid;
	int cap_style = CapButt;
	int join_style = JoinBevel;
	int screen_num = DefaultScreen(display_);

	gc = XCreateGC(display_, border_window_, valuemask, &values);

	XSetForeground(display_, gc, 0x3443ea);
	XSetBackground(display_, gc, 0x3443ea);
	XSetLineAttributes(display_, gc, line_width, line_style, cap_style, join_style);
	XSetFillStyle(display_, gc, FillSolid);
}