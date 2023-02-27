CXXFLAGS ?= -Wall -g 
CXXFLAGS += -std=c++1y 
CXXFLAGS += `pkg-config --cflags x11 libglog`
CXXFLAGS += `wx-config --cxxflags`

LDFLAGS += `pkg-config --libs x11 libglog`
LDFLAGS += `wx-config --libs`

all: basic_wm

HEADERS = \
	window_manager.hpp \
	util.hpp \
	xlib_window.hpp \
	xlib_border.hpp \
	xlib_button.hpp 
SOURCES = \
	window_manager.cpp \
	util.cpp \
	xlib_window.cpp \
	xlib_border.cpp \
	xlib_button.cpp \
	main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

basic_wm: $(HEADERS) $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f basic_wm $(OBJECTS)