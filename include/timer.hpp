#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
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

#elif defined(EMSCRIPTEN)

#include <iostream>

// emscripten's clock_gettime is broken (returns constant zero),
// but gettimeofday works. Unfortunately, the built-in clocks use
// the former, not the latter.

struct gtod_clock {
	typedef std::chrono::microseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<gtod_clock> time_point;
	static const bool is_monotonic = false;
	static const bool is_steady = false; // I assume

	static time_point now() {
		::timeval t = {0};
		::gettimeofday(&t, nullptr);
		return time_point(duration(static_cast<rep>(((t.tv_sec * period::den) / period::num) + t.tv_usec)));
	}
};

typedef timer<gtod_clock> high_resolution_timer;

#else

typedef timer<std::chrono::high_resolution_clock> high_resolution_timer;

#endif

#endif
