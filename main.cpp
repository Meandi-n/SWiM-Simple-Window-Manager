#include <cstdlib>
#include <glog/logging.h>
#include "window_manager.hpp"


int main(int argc, char** argv)
{
	::google::InitGoogleLogging(argv[0]);
	::google::SetLogDestination(google::ERROR, "glog/ERROR.log");
	::google::SetLogDestination(google::INFO, "glog/INFO.log");

	// Creating a smart pointer to the window manager of type WindowManager
	// This method invokes the constructor of the WindowManager object
	::std::unique_ptr<WindowManager> window_manager = WindowManager::Create();

	// If window manager is NULL
	if (!window_manager)
	{
		LOG(ERROR) << "main.cpp (19): Failed to initialise window manager";
		return EXIT_FAILURE; 
	}
	LOG(INFO) << "main.cpp (22): Initialised Window Manager, progressing to run()";

	window_manager->run(); // This is an infinite loop until broken

	return EXIT_SUCCESS;
}
