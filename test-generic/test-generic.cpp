#include <iostream>

#include <unistd.h>

#include "timer.hpp"

int main() {
	high_resolution_timer timer;
	usleep(1000 * 1000);
	high_resolution_timer::duration dur = timer.pulse();
	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}
