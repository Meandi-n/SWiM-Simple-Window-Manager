#include "util.hpp"
#include <algorithm>
#include <sstream>
#include <vector>

::std::string ToString(const XEvent& e)
{
	//https://tronche.com/gui/x/xlib/events/types.html
	static const char* const X_EVENT_TYPE_NAMES[] = 
	{
								"",
								"", 
/* Keyboard Events */			"KeyPress",
								"KeyRelease", 
/* Pointer Events */			"ButtonPress",
								"ButtonRelease",
								"MotionNotify", // 1 to 5 button motion bits are masked in the event mask attribute of the window. 
/* Window crossing Events */	"EnterNotify",		
								"LeaveNotify",
/* Input Focus Events */		"FocusIn",
								"FocusOut",
/* Keymap state notification */	"KeymapNotify", // can report events to clients that want info about changes to keyboard state. 
/* Exposure events	*/			"Expose", // Brings the window to the forefront when information is need but not in focus (i think)
								"GraphicsExpose", // confusing... when things cannot be rendered?
								"NoExpose", // confusing
/* (Not specifically grouped) */"VisibilityNotify",
								"CreateNotify",
								"DestroyNotify",
								"UnmapNotify",
								"MapNotify",
								"MapRequest",
								"ReparentNotify",
								"ConfigureNotify",
								"ConfigureRequest",
								"GravityNotify",
								"ResizeRequest",
								"CirculteNotify",
								"CirculteRequest", // does what it sounds like. 
								"PropertyNotify",
								"SelectionClear",
								"SelectionRequest",
								"SelectionNotify",
								"ColormapNotify",
								"ClientMessage",
								"MappingNotify",
								"GeneralEvent",
	};

	if (e.type < 2 || e.type >= LASTEvent)
	{
		::std::ostringstream out;
		out << "Unknown (" << e.type << ")";
		return out.str();
	}

	// Compile properties we care about. 

	::std::vector<::std::pair<::std::string, ::std::string>> properties;
	switch(e.type)
	{
	case CreateNotify:
		properties.emplace_back("window", ToString(e.xcreatewindow.window));
		properties.emplace_back("parent", ToString(e.xcreatewindow.parent));
		properties.emplace_back("size", Size<int>(e.xcreatewindow.width, e.xcreatewindow.height).ToString());
		properties.emplace_back("position", Position<int>(e.xcreatewindow.x, e.xcreatewindow.y).ToString());
		properties.emplace_back("border_width", ToString(e.xcreatewindow.border_width));
		properties.emplace_back("override_redirect", ToString(static_cast<bool>(e.xcreatewindow.override_redirect)));
		break;

	case DestroyNotify:
		properties.emplace_back("window", ToString(e.xdestroywindow.window));
		break;

	case MapNotify:
		properties.emplace_back("window", ToString(e.xmap.window));
		properties.emplace_back("event", ToString(e.xmap.event));
		properties.emplace_back("override_redirect", ToString(static_cast<bool>(e.xmap.override_redirect)));
		break;

	case UnmapNotify:
		properties.emplace_back("window", ToString(e.xunmap.window));
		properties.emplace_back("event", ToString(e.xunmap.event));
		properties.emplace_back("from_configure", ToString(static_cast<bool>(e.xunmap.from_configure)));
		break;

	case ConfigureNotify:
		properties.emplace_back("window", ToString(e.xconfigure.window));
		properties.emplace_back("size", Size<int>(e.xconfigure.width, e.xconfigure.height).ToString());
		properties.emplace_back("position", Position<int>(e.xconfigure.x, e.xconfigure.y).ToString());
		properties.emplace_back("border_width", ToString(e.xconfigure.border_width));
		properties.emplace_back("override_redirect", ToString(static_cast<bool>(e.xconfigure.override_redirect)));
		break;

	case ReparentNotify:
		properties.emplace_back("window", ToString(e.xreparent.window));
		properties.emplace_back("parent", ToString(e.xreparent.parent));
		properties.emplace_back("position", Position<int>(e.xreparent.x, e.xreparent.y).ToString());
		properties.emplace_back("override_redirect", ToString(static_cast<bool>(e.xreparent.override_redirect)));
		break;

	case MapRequest:
		properties.emplace_back("window", ToString(e.xmaprequest.window));
		break;

	case ConfigureRequest:
		properties.emplace_back("window", ToString(e.xconfigurerequest.window));
		properties.emplace_back("parent", ToString(e.xconfigurerequest.parent));
		properties.emplace_back("value_mask", XConfigureWindowValueMaskToString(e.xconfigurerequest.value_mask));
		properties.emplace_back("position", Position<int>(e.xconfigurerequest.x, e.xconfigurerequest.y).ToString());
		properties.emplace_back("size", Size<int>(e.xconfigurerequest.width, e.xconfigurerequest.height).ToString());
		properties.emplace_back("border_width", ToString(e.xconfigurerequest.border_width));
		break;

	case ButtonPress:
	case ButtonRelease:
		properties.emplace_back("window", ToString(e.xbutton.window));
		properties.emplace_back("button", ToString(e.xbutton.button));
		properties.emplace_back("position_root", Position<int>(e.xbutton.x_root, e.xbutton.y_root).ToString());
		break;

	case MotionNotify:
		properties.emplace_back("window", ToString(e.xmotion.window));
		properties.emplace_back("position_root", Position<int>(e.xmotion.x_root, e.xmotion.y_root).ToString());
		properties.emplace_back("state", ToString(e.xmotion.state));
		properties.emplace_back("time", ToString(e.xmotion.time));
		break;

	case KeyPress:
	case KeyRelease:
		properties.emplace_back("window", ToString(e.xkey.window));
		properties.emplace_back("state", ToString(e.xkey.state));
		properties.emplace_back("keycode", ToString(e.xkey.keycode));
		break;

	default:
		// no properties are printer for unused events
		break;
	}

	// Build final string
	const ::std::string properties_string = 
		Join(properties, ", ", [] (const ::std::pair<::std::string, ::std::string> &pair) 
		{
			return pair.first + ": " + pair.second;
		});

	::std::ostringstream out;
	out << X_EVENT_TYPE_NAMES[e.type] << " {" << properties_string << " }";
	return out.str();
}

::std::string XConfigureWindowValueMaskToString(unsigned long value_mask)
{
	::std::vector<::std::string> masks;
	if(value_mask & CWX) // Config window X
		masks.emplace_back("X");
	if(value_mask & CWY) // Config window Y
		masks.emplace_back("Y");
	if(value_mask & CWWidth) // Config window width
		masks.emplace_back("Width");
	if(value_mask & CWHeight) // Config window height
		masks.emplace_back("Height");
	if(value_mask & CWBorderWidth) // Config window border width
		masks.emplace_back("BorderWidth");
	if(value_mask & CWSibling)
		masks.emplace_back("Sibling");
	if(value_mask & CWStackMode)
		masks.emplace_back("StackMode");
	return Join(masks, "|");
}

::std::string XRequestCodeToString(unsigned char request_code)
{
	static const char* const X_REQUEST_CODE_NAMES[] = {
		"", 
		"CreateWindow", 
		"ChangeWindowAttributes",
		"GetWindowAttributes",
		"DestroyWindow", 
		"DestroySubwindows",
		"ChangeSaveSet",
		"ReparentWindow",
		"MapWindow",
		"MapSubwindows",
		"UnmapWindow",
		"UnmapSubwindows",
		"ConfigureWindow",
		"CirculateWindow",
		"GetGeometry",
		"QueryTree",
		"InternAtom",
		"GetAtomName",
		"ChangeProperty",
		"DeleteProperty",
		"GetProperty",
		"ListProperties",
		"SetSelectionOwner",
		"GetSelectionOwner",
		"ConvertSelection",
		"SendEvent",
		"GrabPointer",
		"UngrabPointer",
		"GrabButton",
		"UngrabButton",
		"ChangeActivePointerGrab",
		"GrabKeyboard",
		"UngrabKeyboard",
		"GrabKey",
		"UngrabKey",
		"AllowEvents",
		"GrabServer",
		"UngrabServer",
		"QueryPointer",
		"GetMotionEvents",
		"TranslateCoords",
		"WarpPointer",
		"SetInputFocus",
		"GetInputFocus",
		"QueryKeymap",
		"OpenFont",
		"CloseFont",
		"QueryFont",
		"QueryTextExtents",
		"ListFonts",
		"ListFontsWithInfo",
		"SetFontPath",
		"GetFontPath",
		"CreatePixmap",
		"FreePixmap",
		"CreateGC",
		"ChangeGC",
		"CopyGC",
		"SetDashes",
		"SetClipRectangles",
		"FreeGC",
		"ClearArea",
		"CopyArea",
		"CopyPlane",
		"PolyPoint",
		"PolyLine",
		"PolySegment",
		"PolyRectangle",
		"PolyArc",
		"FillPoly",
		"PolyFillRectangle",
		"PollyFillArc",
		"PutImage",
		"GetImage",
		"PolyText8",
		"PolyText16",
		"ImageText8",
		"ImageText16",
		"CreateColormap",
		"FreeColormap",
		"CopyColormapAndFree",
		"InstallColormap",
		"UninstallColormap",
		"ListInstalledColormaps",
		"AllocColor",
		"AllocNamedColor",
		"AllocColorCells",
		"AllocColorPlanes",
		"FreeColors",
		"StoreColors",
		"StoreNamedColor",
		"QueryColors",
		"LookupColor",
		"CreateCursor",
		"CreateGlyphCursor",
		"FreeCursor",
		"RecolorCursor",
		"QueryBestSize",
		"QueryExtensions",
		"ListExtensions",
		"ChangeKeyboardMapping",
		"GetKeyboardMapping",
		"ChangekeyboardControl",
		"GetKeyboardControl",
		"Bell",
		"ChangePointerControl",
		"GetPointerControl",
		"SetScreenSaver",
		"GetScreenSaver",
		"ChangeHosts",
		"ListHosts",
		"SetAccessControl",
		"SetCloseDownMode",
		"KillClient",
		"RotateProperties",
		"ForceScreenSaver",
		"SetPointerMapping",
		"GetPointerMapping",
		"SetModifiedMapping",
		"GetModifiedMapping",
		"NoOperatoin"
	};
	return X_REQUEST_CODE_NAMES[request_code];
}

