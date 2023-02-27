#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

// X11/Xlib.h uses C langauge calling. 
extern "C" { 
#include <X11/Xlib.h> 
}
// General utilities to manage dynamic memory
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <cstring>
#include <algorithm>
#include <glog/logging.h>
#include <iostream>

#include "util.hpp"
#include "xlib_window.hpp"

class WindowManager
{
public:
	::std::unordered_map<Window, XLib_Window> frame_map_;
	::std::unordered_map<Window, XLib_Window> button_map_;

	/** Function: Create
	 * - Establishes connection to X server.
	 * - Creates a WindowManager instance. 
	 **/
	static ::std::unique_ptr<WindowManager> Create(const std::string& display_str = std::string());
	/** Function: Destructor
	 * - Disconnects from the X server.
	 **/
	~WindowManager();
	/** Function: run
	 * - Entry point, enters main event loop for window manager.
	 **/
	void run();

private:
	WindowManager(Display* display);
	void Frame(Window w, bool created_before_window_manager);
	void Unframe(Window w);

	GC create_gc(Display* display_, Window w);

	void redrawAllWindows();

	void OnCreateNotify(const XCreateWindowEvent& e);
	void OnDestroyNotify(const XDestroyWindowEvent& e);
	void OnReparentNotify(const XReparentEvent& e);

	void OnMapNotify(const XMapEvent& e);
	void OnUnmapNotify(const XUnmapEvent& e);

	void OnConfigureNotify(const XConfigureEvent& e);
	
	
	void OnMapRequest(const XMapRequestEvent& e);
	void OnConfigureRequest(const XConfigureRequestEvent& e);
	

	void OnButtonPress(const XButtonEvent& e);
	void OnButtonRelease(const XButtonEvent& e);

	void OnMotionNotify(const XMotionEvent& e);

	void OnKeyPress(const XKeyEvent& e);
	void OnKeyRelease(const XKeyEvent& e); 

	static int OnXError(Display* display, XErrorEvent* e);
	/** Function: OnWMDetected
	 * - Must be static as its address is passed to XLib
	 * - handles the graceful closure of the WM if a WM is already
	 *   connected to the X server. 
	 * - this is selected as the ErrorHandler right before substructure 
	 *   redirection mask on the root window (what i said above)
	 **/
	static int OnWMDetected(Display* display, XErrorEvent* e);

	/** 
	 * Variable is set by OnWMDetected and hence must be static **/
	static bool wm_detected_;
	static ::std::mutex wm_detected_mutex_;

	Display* display_;
	const Window root_;

	Position<int> drag_start_pos_;
	Position<int> drag_start_frame_pos_;
	Size<int> drag_start_frame_size_;

	// Atom constants 
	const Atom WM_PROTOCOLS;
	const Atom WM_DELETE_WINDOW;
};

#endif