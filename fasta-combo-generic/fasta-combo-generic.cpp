/* The Computer Language Benchmarks Game
http://benchmarksgame.alioth.debian.org/

converted to C++ from D by Rafal Rusin
modified by Vaclav Haisman
modified by The Anh to compile with g++ 4.3.2
modified by Branimir Maksimovic
modified by Kim Walisch
modified by Peter Bright
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <initializer_list>

#include <boost/xpressive/xpressive.hpp>

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
	void make(std::ostream& output, const char* id, const char* desc, int n, F functor)
	{
		output << ">" << id << " " << desc << "\n";
		char line[LENGTH + 1] = { 0 };
		int i = 0;
		while (n-- > 0)
		{
			line[i++] = functor();
			if (i >= LENGTH)
			{
				output << line << "\n";
				i = 0;
			}
		}
		line[i] = 0;
		if(std::strlen(line) != 0)
		{
			output << line << "\n";
		}
		output << std::flush;
	}

} // end namespace

void fasta(int iterations, const char* filename) {

	make_cumulative(iub);
	make_cumulative(homosapiens);

	std::ofstream output(filename, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

	make(output, "ONE"  , "Homo sapiens alu"      , iterations * 2, Repeat(alu));
	make(output, "TWO"  , "IUB ambiguity codes"   , iterations * 3, Random(iub));
	make(output, "THREE", "Homo sapiens frequency", iterations * 5, Random(homosapiens));
}

const int LINELENGTH = 60;

typedef std::string Header;
typedef std::string Segment;

inline char complement(char element)
{
	static const char charMap[] =
	{
		'T', 'V', 'G', 'H', '\0', '\0', 'C', 'D', '\0', '\0', 'M', '\0', 'K',
		'N', '\0', '\0', '\0', 'Y', 'S', 'A', 'A', 'B', 'W', '\0', 'R', '\0'
	};

	return charMap[toupper(element) - 'A'];
}

void print_revcomp(Header const& header, Segment const& seg, std::ostream& out = std::cout)
{
	out << header << "\n";

	Segment comp(seg.rbegin(),seg.rend());
	transform(comp.begin(),comp.end(), comp.begin(), complement);

	size_t i = 0;
	size_t stop = comp.length()/LINELENGTH + ((comp.length()%LINELENGTH)?1:0);

	while(i < stop)
		out << comp.substr(i++*LINELENGTH,LINELENGTH) << "\n";
}

void reverse_complement(const char* filename) {
	std::ios_base::sync_with_stdio(false);

	std::ifstream input(filename, std::ios_base::binary);
	std::ofstream output("fasta-reverse.txt", std::ios_base::binary);

	Segment line, segment; 
	Header header;

	while (std::getline(input, line))
	{
		if(line[0] == '>')
		{
			if (!segment.empty())
				print_revcomp(header, segment, output);
			header = line;
			segment.clear();
		}
		else
			segment += line;
	}
	print_revcomp(header, segment, output);
}

namespace x = boost::xpressive;

void regex_dna(const char* filename) {
	std::ifstream fin(filename);
	std::string str, line;
	while(std::getline(fin, line)) {
		str += line;
		str += "\n";
	}
	// more or less a drop-in replacement, but not exactly. boost::xpressive "." matches "\n". std::regex does not. Nasty little incompatibility.
	x::sregex header_pattern = x::sregex::compile(">.*\n|\n", x::regex_constants::ECMAScript | x::regex_constants::optimize | x::regex_constants::not_dot_newline);
	size_t original_length = str.length();
	std::string no_headers;
	x::regex_replace(std::back_inserter(no_headers), std::begin(str), std::end(str), header_pattern, "");
	size_t no_header_length = no_headers.length();
	std::string pattern1[] = {
		"agggtaaa|tttaccct"        ,
		"[cgt]gggtaaa|tttaccc[acg]",
		"a[act]ggtaaa|tttacc[agt]t",
		"ag[act]gtaaa|tttac[agt]ct",
		"agg[act]taaa|ttta[agt]cct",
		"aggg[acg]aaa|ttt[cgt]ccct",
		"agggt[cgt]aa|tt[acg]accct",
		"agggta[cgt]a|t[acg]taccct",
		"agggtaa[cgt]|[acg]ttaccct"
	};

	struct regex_substitution {
		x::sregex ex;
		std::string sub;
	};

	regex_substitution pattern2[] = {
		{ x::sregex::compile("B", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(c|g|t)"   },
		{ x::sregex::compile("D", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|g|t)"   },
		{ x::sregex::compile("H", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|c|t)"   },
		{ x::sregex::compile("K", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(g|t)"     },
		{ x::sregex::compile("M", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|c)"     },
		{ x::sregex::compile("N", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|c|g|t)" },
		{ x::sregex::compile("R", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|g)"     },
		{ x::sregex::compile("S", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(c|g)"     },
		{ x::sregex::compile("V", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|c|g)"   },
		{ x::sregex::compile("W", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(a|t)"     },
		{ x::sregex::compile("Y", x::regex_constants::ECMAScript | x::regex_constants::optimize), "(c|t)"     },
	};

	{
		for(signed i = 0; i < (sizeof(pattern1) / sizeof(*pattern1)); ++i) {
			x::sregex expr = x::sregex::compile(pattern1[i], x::regex_constants::ECMAScript | x::regex_constants::optimize);
			x::sregex_iterator it(std::begin(no_headers), std::end(no_headers), expr);
			std::cout << pattern1[i] << " " << std::distance(it, x::sregex_iterator()) << "\n";
		}

		for(size_t i = 0; i < (sizeof(pattern2) / sizeof(*pattern2)); ++i) {
			no_headers = x::regex_replace(no_headers, pattern2[i].ex, pattern2[i].sub);
		}
	}

	std::cout << original_length << "\n";
	std::cout << no_header_length << "\n";
	std::cout << no_headers.length() << std::endl;
}

int main(int argc, char *argv[])
{
	high_resolution_timer timer;
	const int n = argc > 1 ? atoi(argv[1]) : 1000 * 1000;
	const char* filename = "fasta.txt";
	fasta(n, filename);
	reverse_complement(filename);
	regex_dna(filename);

	high_resolution_timer::duration dur = timer.pulse();

	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}
