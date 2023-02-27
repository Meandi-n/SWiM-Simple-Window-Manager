#ifndef UTIL_HPP
#define UTIL_HPP

extern "C" {
	#include <X11/Xlib.h>
}
#include <sstream>
#include <string>

/*-----------------------------------------------
 * Template Struct: 2D SIZE
 *-----------------------------------------------*/
template <typename T>
struct Size
{
	T width, height; 						// Member definition
	Size() = default; 						// blank constructor
	Size(T w, T h) : width(w), height(h) {} // constructor method
	/* Constant functions
	 * - constant functions are read only functions that dont modify the object
	 *   from which is was called.
	 * - beneftis = able to be called from non mutable object (constant object). 
	 **/
	::std::string ToString() const;			// ToString() function definition
};// END Size

/*-----------------------------------------------
 * (1A) Template: operator (Size)
 * - outputs Size<T> as a string to a std::ostream
 *-----------------------------------------------*/
template <typename T>
::std::ostream& operator << (::std::ostream& out, const Size<T>& size);

/*-----------------------------------------------
 * Template Struct: 2D POSITION
 *-----------------------------------------------*/
template <typename T>
struct Position
{
	T x, y;
	Position() = default;
	Position(T _x, T _y) : x(_x), y(_y) {}
	
	::std::string ToString() const;
};

/*-----------------------------------------------
 * Template: operator (Position)
 * - outputs Size<T> as a string to a std::ostream
 *-----------------------------------------------*/
template <typename T>
::std::ostream& operator << (::std::ostream& out, const Position<T>& pos);

/*-----------------------------------------------
 * Template Struct: 2D VECTOR
 *-----------------------------------------------*/
template <typename T>
struct Vector2D
{
	T x, y;

	Vector2D() = default;
	Vector2D(T _x, T _y) : x(_x), y(_y) {}

	::std::string ToString() const;
};


/*-----------------------------------------------
 * POSITION AND VECTOR OPERATORS
 *-----------------------------------------------*/
template <typename T>
Vector2D<T> operator - (const Position<T>& a, const Position<T>& b);
template <typename T>
Position<T> operator + (const Position<T>& a, const Vector2D<T>& v);
template <typename T>
Position<T> operator + (const Vector2D<T>& v, const Position<T>& a);
template <typename T>
Position<T> operator - (const Position<T>& a, const Vector2D<T>& v);

/*-----------------------------------------------
 * SIZE OPERATORS
 *-----------------------------------------------*/
template <typename T>
Vector2D<T> operator - (const Size<T>& a, const Size<T>& b);
template <typename T>
Size<T> operator + (const Size<T>& a, const Vector2D<T> &v);
template <typename T>
Size<T> operator + (const Vector2D<T>& v, const Size<T>& a);
template <typename T>
Size<T> operator - (const Size<T>& a, const Vector2D<T> &v);

/*-----------------------------------------------
 * Template: Container delimiter
 * - Joins a container of elements into a single string 
 *   seperated by a delimiter.
 *-----------------------------------------------*/
template <typename Container>
::std::string Join(const Container& container, const ::std::string& delimiter);

/*-----------------------------------------------
 * Template: Container delimiter + converter. (converted to a string). 
 *-----------------------------------------------*/
template <typename Container, typename Converter>
::std::string Join(const Container& container, const ::std::string& delimiter, Converter converter);

/*-----------------------------------------------
 * Template: ToString
 * - returns a string representation of a built in type (with ostream support)
 *-----------------------------------------------*/
template <typename T>
::std::string ToString(const T& x);

/*-----------------------------------------------
 * Function: ToString (For XEvent)
 * - returns string of XEvent (for debugging purposes)
 *-----------------------------------------------*/
extern ::std::string ToString(const XEvent& e);

/*-----------------------------------------------
 * Function: XConfigureWindowValueMaskToString
 * - returns a string describing the X window configuration value mask
 *-----------------------------------------------*/
extern ::std::string XConfigureWindowValueMaskToString(unsigned long value_mask);

/*-----------------------------------------------
 * Function: XRequestCodeToString
 *-----------------------------------------------*/
extern ::std::string XRequestCodeToString(unsigned char request_code);

/*----------------------------------------------------------------------------
 * IMPLEMENTATION STAGE
 *----------------------------------------------------------------------------*/
#include <algorithm>
#include <vector>
#include <sstream>

/**
 * ALL ToString() TEMPLATES...
 **/
// For Size Struct:
template <typename T>
::std::string Size<T>::ToString() const 
{
	::std::ostringstream out;
	out << width << 'x' << height;
	return out.str();
}
// For Position Struct:
template <typename T>
::std::string Position<T>::ToString() const 
{
	::std::ostringstream out;
	out << "(" << x << ", " << y << ")";
	return out.str();
}
// For Vector2D Struct:
template <typename T>
::std::string Vector2D<T>::ToString() const
{
	::std::ostringstream out;
	out << "(" << x << ", " << y << ")";
	return out.str();
}
// END "ALL ToString" TEMPLATES

/**
 * ALL << Operator TEMPLATES
 **/
// Size ToString << Operator
template <typename T>
::std::ostream& operator << (::std::ostream& out, const Size<T>& size)
{
	return out << size.ToString();
}
// Position ToString << Operator
template <typename T>
::std::ostream& operator << (::std::ostream& out, const Position<T>& size)
{
	return out << size.ToString();
}
// Vector2D ToString << Operator
template <typename T>
::std::ostream& operator << (::std::ostream& out, const Vector2D<T>& size)
{
	return out << size.ToString();
}

/**
 * ALL +/- Operator TEMPLATES
 **/
template <typename T>
Vector2D<T> operator - (const Position<T>& a, const Position<T>& b)
{
	return Vector2D<T>(a.x - b.x, a.y - b.y);
}

template <typename T>
Position<T> operator + (const Position<T>& a, const Vector2D<T> &v)
{
	return Position<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Position<T> operator - (const Position<T>& a, const Vector2D<T> &v)
{
	return Position<T>(a.x - v.x, a.y - v.y);
}

template <typename T>
Vector2D<T> operator - (const Size<T>& a, const Size<T>& b)
{
	return Vector2D<T>(a.width - b.width, a.height - b.height);
}

template <typename T>
Size<T> operator + (const Size<T>& a, const Vector2D<T> &v)
{
	return Size<T>(a.width + v.x, a.height + v.y);
}

template <typename T>
Size<T> operator - (const Size<T>& a, const Vector2D<T> &v)
{
	return Size<T>(a.width - v.x, a.height - v.y);
}

/**
 * Conatiner Delimiter without conversion
 **/
template <typename Container>
::std::string Join(const Container& container, const ::std::string& delimiter)
{
	::std::ostringstream out;
	for(auto i = container.cbegin(); i != container.cend(); ++i)
	{
		if (i != container.cbegin())
			out << delimiter;
		out << *i;
	}
	return out.str();
}

/**
 * Container Delimiter with conversion
 **/
template <typename Container, typename Converter>
::std::string Join(const Container& container, const ::std::string& delimiter, Converter converter)
{
	::std::vector<::std::string> converted_container(container.size());
	::std::transform(container.cbegin(), container.cend(), converted_container.begin(), converter);
	return Join(converted_container, delimiter);
}

template <typename T>
::std::string ToString(const T& x)
{
	::std::ostringstream out;
	out << x;
	return out.str();
}

#endif
