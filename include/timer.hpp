#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#ifdef _WIN32
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

template<typename clock_t>
struct timer {
	typedef typename clock_t::time_point time_point;
	typedef typename clock_t::duration duration;

	timer() : last(clock_t::now()) {
	}

	duration pulse() {
		time_point now = clock_t::now();
		duration dur = now - last;
		last = now;
		return dur;
	}

private:
	time_point last;
};

#ifdef _WIN32

struct qpc_clock {
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<qpc_clock> time_point;
	static const bool is_monotonic = true;
	static const bool is_steady = true;

	static time_point now() {
		LARGE_INTEGER current = {0};
		::QueryPerformanceCounter(&current);
		return time_point(duration(static_cast<rep>((static_cast<double>(current.QuadPart) / frequency().frequency.QuadPart) * (period::den / period::num))));
	}

private:
	struct qpc_frequency {
		qpc_frequency() {
			::QueryPerformanceFrequency(&frequency);
		}

		LARGE_INTEGER frequency;
	};

	static qpc_frequency& frequency() {
		static qpc_frequency frequency;
		return frequency;
	}
};

typedef timer<qpc_clock> high_resolution_timer;

#else

typedef timer<std::chrono::high_resolution_clock> high_resolution_timer;

#endif

#endif
