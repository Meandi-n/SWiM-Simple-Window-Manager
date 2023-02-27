#include "xlib_window.hpp"

XLib_Window::XLib_Window()
{
	auto end = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(end);
	
	::std::stringstream ss;
	ss << "XLib_Window[" << std::ctime(&time) << rand() % 10000;

	xlib_window_id = ss.str(); 
}

XLib_Window::~XLib_Window()
{

}

void XLib_Window::frameWindow(Display* display_, Window root_, Window w)
{
/** getting attributes of application window **/
	XWindowAttributes x_window_attrs;
	CHECK(XGetWindowAttributes(display_, w, &x_window_attrs));
	// generating colourmap for windows
	int screen = DefaultScreen(display_);
	Colormap colormap = DefaultColormap(display_, screen);
/** Defining Colours of Window **/
	// bar_colour
	XColor background_colour_;
	background_colour_.red = 45000; background_colour_.green = 45000; background_colour_.blue = 45000;
	background_colour_.flags = DoRed | DoGreen | DoBlue;
	
	// Allocating colour to display_
	XAllocColor(display_, colormap, &background_colour_); 

/** Defining frame_ **/
	application_window_ = w;
	window_properties_.window_position_.x 	= x_window_attrs.x;
	window_properties_.window_position_.y 	= x_window_attrs.y;
	window_properties_.window_size_.height 	= x_window_attrs.height;
	window_properties_.window_size_.width 	= x_window_attrs.width;
	window_properties_.border_width_		= 1;
	window_properties_.window_bar_height_	= 15;

	// Setting window basic attributes
	XSetWindowAttributes attrs_;

	attrs_.event_mask = SubstructureRedirectMask| //handle child window requests (MapRequest)
						SubstructureNotifyMask 	| //handle child window notifications (DestroyNotify)
						StructureNotifyMask		| //handle container notifications (ConfigureNotify)
						ExposureMask			; //handle container redraw (Expose)

	attrs_.do_not_propagate_mask = 0; // dont hide events from child window
	attrs_.background_pixel = background_colour_.pixel; // background colour

	unsigned long attrs_mask_ = CWEventMask  | // enable attrs.event_mask
								NoEventMask  | // enable attrs.do_not_propagate_mask
								CWBackPixel ; // enable attrs.background_pixel

	window_properties_.attrs_mask_ = attrs_mask_;
	window_properties_.set_attrs = attrs_;

/** creating window **/
	createWindow(display_, root_);

	// b. resize windows with the alt+right button
	XGrabButton(
			display_,
			Button1,
			None,
			resize_button_.button_window_,
			false,
			ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None);

	XGrabButton(
			display_,
			Button1,
			None,
			move_button_.button_window_,
			false,
			ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None);

	XGrabButton(
			display_,
			Button1,
			None,
			close_button_.button_window_,
			false,
			ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None);
	
	// c. kill windows with the alt+f4 button
	XGrabKey(
			display_,
			XKeysymToKeycode(display_, XK_F4),
			Mod1Mask,
			border_.border_window_,
			false,
			GrabModeAsync,
			GrabModeAsync);

	// d. switch windows with the alt tab button
	XGrabKey(
			display_,
			XKeysymToKeycode(display_, XK_Tab),
			Mod1Mask,
			border_.border_window_,
			false,
			GrabModeAsync,
			GrabModeAsync);
}

void XLib_Window::resizeWindow(Display* display_, unsigned int width, unsigned int height, Window root_)
{
	XResizeWindow(display_, frame_, width, height);
	XResizeWindow(display_, border_.border_window_, width, height+window_properties_.window_bar_height_);
	XResizeWindow(display_,	application_window_, width, height);
}
void XLib_Window::moveWindow(Display* display_, unsigned int x, unsigned int y, Window root_)
{
	XMoveWindow(display_, border_.border_window_, x, y);
}


void XLib_Window::createWindow(Display* display_, const Window root_)
{
	/* Create Frame Window */
	frame_ = XCreateWindow(
		display_,
		root_,
		window_properties_.window_position_.x,
		window_properties_.window_position_.y,
		window_properties_.window_size_.width,
		window_properties_.window_size_.height,
		window_properties_.border_width_, 
		CopyFromParent, // depth
		InputOutput,	// class
		CopyFromParent, // visual
		window_properties_.attrs_mask_,	
		&window_properties_.set_attrs);

	/* Create Border Window */
	border_.border_properties_.border_size_.width = window_properties_.window_size_.width;
	border_.border_properties_.border_size_.height = window_properties_.window_size_.height;
	border_.border_properties_.border_position_.x = window_properties_.window_position_.x;
	border_.border_properties_.border_position_.y = window_properties_.window_position_.y;
	border_.border_properties_.background_colour_ = 0;
	border_.border_properties_.border_colour_ = 0;

	char* name = NULL;
	if(XFetchName(display_, application_window_, &name) == 0)
		border_.border_properties_.window_name_ = "Window";
	else
		border_.border_properties_.window_name_ = name;

	border_.createWindow(display_, root_);
	
	const unsigned int button_size_ = 8;

	/* Create Move Button Window */
	move_button_.button_properties_.button_size_.width = button_size_;
	move_button_.button_properties_.button_size_.height = button_size_;
	move_button_.button_properties_.button_position_.x = 
		window_properties_.window_size_.width - button_size_ - 5;
	move_button_.button_properties_.button_position_.y = 5;
	move_button_.button_properties_.button_colour_ = 0x00ff00;

	move_button_.createWindow(display_, border_.border_window_); // was root_ ???/

	/* Create Resize Button Window */
	resize_button_.button_properties_.button_size_.width = button_size_;
	resize_button_.button_properties_.button_size_.height = button_size_;
	resize_button_.button_properties_.button_position_.x = 
		window_properties_.window_size_.width - (button_size_*2) - (5*2);
	resize_button_.button_properties_.button_position_.y = 5;
	resize_button_.button_properties_.button_colour_ = 0x0000ff;

	resize_button_.createWindow(display_, border_.border_window_);

	/* Create Close Button Window */
	close_button_.button_properties_.button_size_.width = button_size_;
	close_button_.button_properties_.button_size_.height = button_size_;
	close_button_.button_properties_.button_position_.x = 
		window_properties_.window_size_.width - (button_size_*3) - (5*3);
	close_button_.button_properties_.button_position_.y = 5;
	close_button_.button_properties_.button_colour_ = 0xff0000;

	close_button_.createWindow(display_, border_.border_window_);

	/* Arrange Windows */

	XReparentWindow(display_, application_window_, frame_, 0, 0);
	XReparentWindow(display_, frame_, border_.border_window_, 0, 0);
	
	XMoveWindow(display_, frame_, window_properties_.window_position_.x, 
		window_properties_.window_position_.y + border_.border_height);

	XSelectInput(display_, border_.border_window_, SubstructureRedirectMask | SubstructureNotifyMask);		     

	XAddToSaveSet(display_, application_window_);

	XMapWindow(display_, border_.border_window_);
	XMapWindow(display_, move_button_.button_window_);
	XMapWindow(display_, resize_button_.button_window_);
	XMapWindow(display_, close_button_.button_window_);
	XMapWindow(display_, frame_);

	border_.createRectangles(display_, root_);
	close_button_.createRectangles(display_, root_);
	move_button_.createRectangles(display_, root_);
	resize_button_.createRectangles(display_, root_);
}

::std::string XLib_Window::toString()
{
	::std::ostringstream oss;

	oss << xlib_window_id << "\n"
		<< "\tWindow (application_window_) = " << application_window_ << "\n"
		<< "\tWindow (frame_) = " << frame_ << "\n"
		<< "\tWindow (outer_window_) = " << border_.border_window_ << "\n"
		<< "\tDimensions = " 
		<< window_properties_.window_size_.width << "x" << window_properties_.window_size_.height 
		<< "\n";

	return oss.str();
}