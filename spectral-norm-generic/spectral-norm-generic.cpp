// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// Original C contributed by Sebastien Loisel
// Conversion to C++ by Jon Harrop
// Compile: g++ -O3 -o spectralnorm spectralnorm.cpp


#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

#include "timer.hpp"

using namespace std;

double eval_A(int i, int j) { return 1.0 / ((i+j)*(i+j+1)/2 + i + 1); }

void eval_A_times_u(const vector<double> &u, vector<double> &Au)
{
	const int size = u.size();
	for(int i = 0; i < size; i++) {
		double t = 0.0;
		for(int j = 0; j < size; j++) {
			t += eval_A(i,j) * u[j];
		}
		Au[i] = t;
	}
}

void eval_At_times_u(const vector<double> &u, vector<double> &Au)
{
	const int size = u.size();
	for(int i = 0; i < size; i++) {
		double t = 0.0;
		for(int j = 0; j < size; j++) {
			t += eval_A(j,i) * u[j];
		}
		Au[i] = t;
	}
}

void eval_AtA_times_u(const vector<double> &u, vector<double> &AtAu, vector<double>& vv)
{
	eval_A_times_u(u, vv);
	eval_At_times_u(vv, AtAu);
}

int main(int argc, char *argv[])
{
	high_resolution_timer timer;

	int N = ((argc == 2) ? atoi(argv[1]) : 2000);
	vector<double> u(N), v(N), w(N);

	fill(u.begin(), u.end(), 1.0);

	for(int i = 0; i < 10; i++) {
		eval_AtA_times_u(u, v, w);
		eval_AtA_times_u(v, u, w);
	}

	double vBv = 0.0, vv = 0.0;
	for(int i = 0; i < N; i++) {
		vBv += u[i] * v[i];
		vv  += v[i] * v[i];
	}

	cout << setprecision(10) << sqrt(vBv/vv) << endl;
	high_resolution_timer::duration dur = timer.pulse();
	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}

