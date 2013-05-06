/* The Computer Language Benchmarks Game
http://benchmarksgame.alioth.debian.org/

contributed by Alexey Zolotov
without openmp, without assert
*/

#define _SCL_SECURE_NO_WARNINGS 1

#include <iostream>
#include <fstream>
#include <cstdio>

// for some reason, VC++'s <regex> has catastrophic performance 
// (or bugs, who even knows). boost::xpressive serves as, amongst 
// other things, a more or less drop-in replacement.
//#include <regex>
#include <boost/xpressive/xpressive.hpp>

#include "timer.hpp"

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

int main(int argc, char* argv[])
{
	high_resolution_timer timer;

	if(argc < 2) {
		return -1;
	}
	regex_dna(argv[1]);
	high_resolution_timer::duration dur = timer.pulse();
	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}
