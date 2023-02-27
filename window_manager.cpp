#include "window_manager.hpp"

bool WindowManager::wm_detected_;
::std::mutex WindowManager::wm_detected_mutex_;

/*------------------------------------------------------------------- 
 * Function: Create
 * - Establishes connection to X server
 * - Creates window manager instance
 * - unique_ptr = a smart pointer that owns and manages 
 * 					the WindowManager object through a pointer. 
 * 					The object is disposed when the uniqie pointer 
 * 					leaves the scope. 
 *-------------------------------------------------------------------*/
::std::unique_ptr<WindowManager> WindowManager::Create(const ::std::string& display_str)
{	

	const char* display_c_str = display_str.empty() ? nullptr : display_str.c_str();

	// 1. open X display
	Display* display = XOpenDisplay(display_c_str);


	// If the display is a NULLPOINTER means we didnt fetch a display
	if (display == nullptr)
	{
		LOG(ERROR) << "Failed to open X Display " << XDisplayName(display_c_str);
		return nullptr;
	}
	
	return ::std::unique_ptr<WindowManager>(new WindowManager(display));
} // END OF Create

/** C++ NOTE:
 *  The colon is an initialisation list: it can be used for
 *  (1) calling base class constructors 
 *  (2) initialising member variables before the body of the constructor executes. 
 **/

/*------------------------------------------------------------------- 
 * Function: Constructor
 * - This is the constructor for WindowManager
 * - It initiates the display_ and root_ private variabes 
 *-------------------------------------------------------------------*/
WindowManager::WindowManager(Display* display) 
		: display_(CHECK_NOTNULL(display)), //initialising display variable before body
		  root_(DefaultRootWindow(display_)), // initialising root before body
		  WM_PROTOCOLS(XInternAtom(display_, "WM_PROTOCOLS", false)),
		  WM_DELETE_WINDOW(XInternAtom(display_, "WM_DELETE_WINDOW", false))
{

}// END OF Constructor 


/*-------------------------------------------------------------------
 * Function: Destructor
 * - This closes the connection to the X display
 *-------------------------------------------------------------------*/
WindowManager::~WindowManager()
{
	XCloseDisplay(display_);
}// END OF Destructor

/*-------------------------------------------------------------------
 * Function: run
 *-------------------------------------------------------------------*/
void WindowManager::run()
{
	LOG(INFO) << "Inside run() function";
	/** (1) Initialisation
	* - This stage first checks for another WM running on the X server. 
	* - if (yes) -> LOG error and gracefully close. 
	* - if (no) -> Initialisation & Start the main event loop.
	**/
	wm_detected_ = false;
	XSetErrorHandler(&WindowManager::OnWMDetected);
	/** ^ This specifies program supplied error handler. 
	*  When an error occurs, the address to the WindowManger::OnWMDetected
	* is called. 
	**/
	XSelectInput(display_, root_, 
		SubstructureRedirectMask | SubstructureNotifyMask);
	/** ^ Requests that the X server report the events associated with the 
	 *  specified event mask. 
	 *  ( We've chosen the substructure redirect mask and notify mask )
	 **/
	XSync(display_, false);
	/** XSync is configured not to discard events in the queue.
	 **/

	// If a WM is amready running
	if(wm_detected_)
	{
		LOG(ERROR) 	<< "Detected another window manager" 
					<< XDisplayString(display_)
					<< " (in window_manager.cpp -> run())";
		return; //Gracefully exit. 
	}
	// If a WM isnt already running...

	XSetErrorHandler(&OnXError);
	/** Now that we arent worried about WM already running, we
	 * move the Errorhandler from the OnWMDetected handler to the 
	 * OnXError handler
	 **/

	XGrabServer(display_);
	Window returned_root, returned_parent;
	Window* top_level_windows;
	unsigned int num_top_level_windows;
	CHECK(XQueryTree(
			display_,
			root_,
			&returned_root,
			&returned_parent,
			&top_level_windows,
			&num_top_level_windows));
	CHECK_EQ(returned_root, root_);

/*** FRAMING NON XLIB WINDOWS (BORING OLD WINDOWS) ***/
	for(unsigned int i = 0; i < num_top_level_windows; ++i)
	{
		XLib_Window window_;
		window_.frameWindow(display_, root_, top_level_windows[i]);
	}

	XFree(top_level_windows);
	XUngrabServer(display_);


	// (2) Main Event loop
	for (;;) // Infinite loop
	{
		/** 
         * Fetching the next event
		 **/
		XEvent e;
		XNextEvent(display_, &e); 
			/** fetch the next event from the display and assign the value of 
			 *  the event to e 
			 **/

		/**
		 * Dispatching the Event
		 **/
		switch(e.type) 
		{
		// BASIC OPERATIONS
		case CreateNotify:
			OnCreateNotify(e.xcreatewindow);
			break;
		case DestroyNotify:
			OnDestroyNotify(e.xdestroywindow);
			break;
		case ReparentNotify:
			OnReparentNotify(e.xreparent);
			break;
		case MapNotify:
			OnMapNotify(e.xmap);
			break;
		case UnmapNotify:
			OnUnmapNotify(e.xunmap);
			break;
		case ConfigureNotify:
			OnConfigureNotify(e.xconfigure);
			break;
		case MotionNotify:
			while(XCheckTypedWindowEvent(
				display_, e.xmotion.window, MotionNotify, &e)) {}
			OnMotionNotify(e.xmotion);
			break;

		case MapRequest:
			OnMapRequest(e.xmaprequest);
			break;
		case ConfigureRequest:
			OnConfigureRequest(e.xconfigurerequest);
			break;
		
		case ButtonPress:
			OnButtonPress(e.xbutton);
			break;
		case ButtonRelease:
			OnButtonRelease(e.xbutton);
			break;
		case KeyPress:
			OnKeyPress(e.xkey);
			break;
		case KeyRelease:
			OnKeyRelease(e.xkey);
			break;
		/**
		 * Interaction with application windows
		 * - in general, window manager must handle actions initiated by client
		 *   and users (automatic things, and user interface things...)
		 * - A window manager communicates with cleint applications via events 
		 * - actions by users and clients invoke events. 
		 * 
		 * - Request Events = events where a client application wants
		 *   to do something to a window and substructure redirection occurs. 
		 *   They are called "request events" becuase the window manager has
		 *   to actually devide to invoke an event.
		 * 
		 * - Notify Events = events that are already executed by the X server. 
		 *   The WM can respond to these events but cannot change that theyve 
		 *   already happened. 
		 **/
		default:
			LOG(WARNING) << "Ignored event";
		}// END switch
	}// END for
}// END run

/*-------------------------------------------------------------------
 *  Function: Unframe
 *-------------------------------------------------------------------*/
void WindowManager::Unframe(Window w)
{
	const XLib_Window frame_ = frame_map_[w];

	XUnmapWindow(display_, frame_.frame_);
	XReparentWindow(
		display_,
		w,
		root_,
		0,0);

	XRemoveFromSaveSet(display_, w);
	XDestroyWindow(display_, frame_.frame_);
	frame_map_.erase(w);

	LOG(INFO) << "Unframed window " << w << " [" << frame_.frame_ << "] ";
}

/*-------------------------------------------------------------------
 *  Function: Frame 
 *-------------------------------------------------------------------*/
void WindowManager::Frame(Window w, bool created_before_window_manager)
{

}

GC WindowManager::create_gc(Display* display_, Window w)
{
	GC gc;
	unsigned long valuemask = 0;

	XGCValues values;	// initial values for the GC
	unsigned int line_width = 1; // line width of the GC
	int line_style = LineSolid;
	int cap_style = CapButt;
	int join_style = JoinBevel;
	int screen_num = DefaultScreen(display_);

	gc = XCreateGC(display_, w, valuemask, &values);

	XSetForeground(display_, gc, WhitePixel(display_, screen_num));
	XSetBackground(display_, gc, BlackPixel(display_, screen_num));

	XSetLineAttributes(display_, gc, line_width, line_style, cap_style, join_style);

	XSetFillStyle(display_, gc, FillSolid);

	return gc;
}

/*-------------------------------------------------------------------
 *  Function: OnUnmapNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnUnmapNotify(const XUnmapEvent& e)
{
	if(!frame_map_.count(e.window))
	{
		LOG(INFO) << "Ignore UnmapNotify for non-client window " 
				<< e.window;
		return;
	}
	else if(e.event == root_)
	{
		LOG(INFO) << "Ignore unmapnotify for reparented pre-exting window " 
				<< e.window;
		return;
	}
	Unframe(e.window);
}
/*-------------------------------------------------------------------
 *  Function: OnButtonPress
 *-------------------------------------------------------------------*/
void WindowManager::OnButtonPress(const XButtonEvent& e)
{
	if(true)
	{
		//XLib_Window window_ = frame_map_[e.window];
		XLib_Window window_ = button_map_[e.window];
		Window outer_window_ = window_.border_.border_window_;

		drag_start_pos_ = Position<int>(e.x_root, e.y_root);

		Window returned_root;
		int x, y;
		unsigned width, height, border_width, depth;

		CHECK(XGetGeometry(
			display_,
			outer_window_,
			&returned_root,
			&x, &y,
			&width, &height,
			&border_width,
			&depth));

		drag_start_frame_pos_ = Position<int>(x, y);
		drag_start_frame_size_ = Size<int>(width, height);

		XRaiseWindow(display_, window_.border_.border_window_);
	}
}
/*-------------------------------------------------------------------
 *  Function: OnButtonRelease
 *-------------------------------------------------------------------*/
void WindowManager::OnButtonRelease(const XButtonEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnMotionNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnMotionNotify(const XMotionEvent& e)
{
	//CHECK(clients_.count(e.window));

	//XLib_Window window_ = frame_map_[e.window]; //ISSUE: ASSUMES WINDOW IS AN XLIB_WINDOW...
	XLib_Window window_ = button_map_[e.window]; 

	const Position<int> drag_pos(e.x_root, e.y_root);
	const Vector2D<int> delta = drag_pos - drag_start_pos_;

	if(e.state & Button1Mask) // Moving
	{
		if(e.window == window_.move_button_.button_window_)
		{
			const Position<int> dest_frame_pos = drag_start_frame_pos_ + delta;
			
			window_.moveWindow(display_, dest_frame_pos.x, dest_frame_pos.y, root_);
		}
		else if(e.window == window_.resize_button_.button_window_)
		{
			const Vector2D<int> size_delta(
				std::max(delta.x, -drag_start_frame_size_.width),
				std::max(delta.y, -drag_start_frame_size_.height));
			const Size<int> dest_frame_size = drag_start_frame_size_ + size_delta;

			window_.resizeWindow(display_, dest_frame_size.width, dest_frame_size.height, root_);
		}
		else if(e.window == window_.close_button_.button_window_)
		{
			std::cout << "DOESNT CLOSE WINDOW YET" << std::endl;
		}
	}
	
	else
		LOG(INFO) << "Error: Window has no children." << e.window;

	redrawAllWindows();
}

void WindowManager::redrawAllWindows()
{
	// traverse frame map
	for(auto& it: frame_map_)
	{
		XLib_Window xlib_window = frame_map_[it.first];
		xlib_window.border_.createRectangles(display_, root_);
		xlib_window.close_button_.createRectangles(display_, root_);
		xlib_window.move_button_.createRectangles(display_, root_);
		xlib_window.resize_button_.createRectangles(display_, root_);
	}
}

/*-------------------------------------------------------------------
 *  Function: OnKeyPress
 *-------------------------------------------------------------------*/
void WindowManager::OnKeyPress(const XKeyEvent& e)
{
	// ALT + F4 CLOSING THE WINDOW
	if((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_F4)))
	{
		Atom* supported_protocols;
		int num_supported_protocols;
		if(XGetWMProtocols(
			display_,
			e.window,
			&supported_protocols,
			&num_supported_protocols) &&
		  (::std::find(
			supported_protocols, 
			supported_protocols + num_supported_protocols,
			WM_DELETE_WINDOW) != 
		  	supported_protocols + num_supported_protocols))
		{
			LOG(INFO) << "Gracefully closing the window " << e.window;
			XEvent msg;
			memset(&msg, 0, sizeof(msg));
			msg.xclient.type 			= ClientMessage;
			msg.xclient.message_type 	= WM_PROTOCOLS;
			msg.xclient.window 			= e.window;
			msg.xclient.format   		= 32;
			msg.xclient.data.l[0] 		= WM_DELETE_WINDOW;

			CHECK(XSendEvent(display_, e.window, false, 0, &msg));
		}
		else
		{
			LOG(INFO) << "Killing Window " << e.window;
			XKillClient(display_, e.window);
		}
	}
	// ALT + TAB SWITCH WINDOW
	else if ((e.state & Mod1Mask) &&
				(e.keycode == XKeysymToKeycode(display_, XK_Tab)))
	{
		auto i = frame_map_.find(e.window);
		CHECK(i != frame_map_.end());
		++i;
		if(i == frame_map_.end())
		{
			i = frame_map_.begin();
		}

		XRaiseWindow(display_, (i->second).frame_);
		XSetInputFocus(display_, (i->first), RevertToPointerRoot, CurrentTime);
	}
}

/*-------------------------------------------------------------------
 *  Function: OnKeyRelease
 *-------------------------------------------------------------------*/
void WindowManager::OnKeyRelease(const XKeyEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnDestroyNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnConfigureNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnConfigureNotify(const XConfigureEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnReparentNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnReparentNotify(const XReparentEvent& e) {}
/*-------------------------------------------------------------------
 *  Function: OnMapNotify
 *-------------------------------------------------------------------*/
void WindowManager::OnMapNotify(const XMapEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnCreateNotify 
 *  - Creates top level window. 
 *  - Window manager will recieve CreateNotify event, however newly
 *    created windows are invisible, so nothing to do inside this function. 
 *-------------------------------------------------------------------*/
void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) {}

/*-------------------------------------------------------------------
 *  Function: OnConfigureRequest 
 *  - request to the WM to configure a window. 
 *-------------------------------------------------------------------*/
void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e)
{
	if(e.window != root_)
	{
		
		XLib_Window window_ = frame_map_[e.window];
		
		unsigned int window_bar_height_ = 
			window_.window_properties_.window_bar_height_;
		
		XWindowChanges changes;
		// copy fields from e to changes
		changes.x = e.x;
		changes.y = e.y;
		changes.width = e.width;
		changes.height = e.height;
		changes.border_width = e.border_width;
		changes.sibling = e.above;
		changes.stack_mode = e.detail;

		if(frame_map_.count(e.window))
		{
			XConfigureWindow(display_, window_.border_.border_window_, e.value_mask, &changes);
			XConfigureWindow(display_, window_.frame_, e.value_mask, &changes);
			LOG(INFO) << "Resize [" << window_.frame_ << "] to " << Size<int>(e.width, e.height);
		}

		// grant request by calling XConfigureWindow
		XConfigureWindow(display_, e.window, e.value_mask, &changes);
		LOG(INFO) << "Resize " << e.window << " to " << Size<int>(e.width, e.height);
	}
}

/*-------------------------------------------------------------------
 *  Function: OnMapRequest 
 *-------------------------------------------------------------------*/
void WindowManager::OnMapRequest(const XMapRequestEvent& e)
{
	XLib_Window window_;
	window_.frameWindow(display_, root_, e.window);

	frame_map_[window_.border_.border_window_] = window_;
	button_map_[window_.move_button_.button_window_] = window_; // map the move button
	button_map_[window_.resize_button_.button_window_] = window_; // map the resize button
	button_map_[window_.close_button_.button_window_] = window_; // map the close button
	// Now map the window 
	XMapWindow(display_, e.window);
}

/***************************
 * ERROR HANDLERS
 **************************/
/*-------------------------------------------------------------------
 *  Function: OnWMDetected [ERROR HANDLER]
 *-------------------------------------------------------------------*/
int WindowManager::OnWMDetected(Display* display, XErrorEvent* e)
{
	CHECK_EQ(static_cast<int>(e->error_code), BadAccess);
	wm_detected_ = true;
	return 0;
}// END OnWMDetected

/*------------------------------------------------------------------- 
 * Function: OnXError [ERROR HANDLER]
 *-------------------------------------------------------------------*/
int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
	return 0;
}// END OnXError

