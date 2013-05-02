#include <iostream>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "timer.hpp"

int main() {
	high_resolution_timer timer;
#ifdef WIN32
	Sleep(1000);
#else
	usleep(1000 * 1000);
#endif
	high_resolution_timer::duration dur = timer.pulse();
	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}
