#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <initializer_list>

#include "timer.hpp"

namespace {

const char alu[] =
	"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
	"GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
	"CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
	"ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
	"GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
	"AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
	"AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

const int LENGTH = 60;

struct IUB
{
	float p;
	char c;

	IUB(float p_, char c_) : p{p_}, c{c_}
	{
	}
};

// VS2012 November CTP has compiler support for initializer_list but no library support.
template<typename C>
C initialize(std::initializer_list<typename C::value_type> items)
{
	return C(items.begin(), items.end());
}

std::vector<IUB> iub = initialize<std::vector<IUB>>(
{
	// it also seems wonky at initializer lists of aggregates
	IUB(0.27f, 'a'),
	IUB(0.12f, 'c'),
	IUB(0.12f, 'g'),
	IUB(0.27f, 't'),
	IUB(0.02f, 'B'),
	IUB(0.02f, 'D'),
	IUB(0.02f, 'H'),
	IUB(0.02f, 'K'),
	IUB(0.02f, 'M'),
	IUB(0.02f, 'N'),
	IUB(0.02f, 'R'),
	IUB(0.02f, 'S'),
	IUB(0.02f, 'V'),
	IUB(0.02f, 'W'),
	IUB(0.02f, 'Y')
});

std::vector<IUB> homosapiens = initialize<std::vector<IUB>>(
{
	IUB(0.3029549426680f, 'a'),
	IUB(0.1979883004921f, 'c'),
	IUB(0.1975473066391f, 'g'),
	IUB(0.3015094502008f, 't')
});

inline float gen_random(float max = 1.0f)
{
	static const int IM = 139968, IA = 3877, IC = 29573;
	static int last = 42;
	last = (last * IA + IC) % IM;
	return max * last * (1.0f / IM);
}

class Repeat {
public:
	Repeat(const char* alu)
		: alu(alu), size(std::strlen(alu)), i(0)
	{ }
	char operator()()
	{
		if (i >= size)
			i = 0;
		return alu[i++];
	}
private:
	Repeat& operator=(const Repeat&);

	const char* alu;
	const std::size_t size;
	std::size_t i;
};

class Random {
public:
	Random(const std::vector<IUB>& i)
		: i(i)
	{ }
	char operator()()
	{
		const float p = gen_random(1.0f);
		const std::size_t count = std::count_if(i.begin(), i.end(),
				[p] (IUB i) { return p >= i.p; });
		return i[count].c;
	}
private:
	Random& operator=(const Random&);

	const std::vector<IUB>& i;
};

void make_cumulative(std::vector<IUB>& i)
{
	std::partial_sum(std::begin(i), std::end(i), std::begin(i),
			[] (IUB l, IUB r) -> IUB { r.p += l.p; return r; });
}

template <class F>
void make(const char* id, const char* desc, int n, F functor)
{
	std::printf(">%s %s\n", id, desc);
	char line[LENGTH + 1] = { 0 };
	int i = 0;
	while (n-- > 0)
	{
		line[i++] = functor();
		if (i >= LENGTH)
		{
			std::puts(line);
			i = 0;
		}
	}
	line[i] = 0;
	if (std::strlen(line) != 0)
		std::puts(line);
}

} // end namespace

int main(int argc, char *argv[])
{
	high_resolution_timer timer;

	const int n = argc > 1 ? atoi(argv[1]) : 1;

	make_cumulative(iub);
	make_cumulative(homosapiens);

	make("ONE"  , "Homo sapiens alu"      , n * 2, Repeat(alu));
	make("TWO"  , "IUB ambiguity codes"   , n * 3, Random(iub));
	make("THREE", "Homo sapiens frequency", n * 5, Random(homosapiens));

	high_resolution_timer::duration dur = timer.pulse();

	std::cerr << std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() << std::endl;
}
