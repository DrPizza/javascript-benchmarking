/* The Computer Language Benchmarks Game
* http://benchmarksgame.alioth.debian.org/
*
* contributed by Jon Harrop
* modified by Alex Mizrahi
* modified by Andreas Sch�fer
* very minor omp tweak by The Anh Tran
*/

#define _CRT_SECURE_NO_WARNINGS 1
#define BOOST_DISABLE_THREADS 1

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <boost/pool/object_pool.hpp>

#include "timer.hpp"

const size_t	LINE_SIZE = 64;

struct Node
{
	Node *l, *r;
	int i;

	Node(int i2) : l(0), r(0), i(i2) 
	{}
	Node(Node *l2, int i2, Node *r2) : l(l2), r(r2), i(i2) 
	{}

	int check() const 
	{
		if (l)
			return l->check() + i - r->check();
		else return i;
	}
};

typedef boost::object_pool<Node> NodePool;

Node *make(int i, int d, NodePool &store) 
{
	if (d > 0) 
		return store.construct(	make(2*i-1, d-1, store), 
		i, 
		make(2*i, d-1, store)	);
	return store.construct(i);
}

int main(int argc, char *argv[]) 
{
	high_resolution_timer timer;

	int min_depth = 4;
	int max_depth = std::max(min_depth+2,
		(argc == 2 ? atoi(argv[1]) : 20));
	int stretch_depth = max_depth+1;

	// Alloc then dealloc stretchdepth tree
	{
		NodePool store;
		Node *c = make(0, stretch_depth, store);
		std::cout << "stretch tree of depth " << stretch_depth << "\t "
			<< "check: " << c->check() << std::endl;
	}

	NodePool long_lived_store;
	Node *long_lived_tree = make(0, max_depth, long_lived_store);

	// buffer to store output of each thread
	char *outputstr = (char*)malloc(LINE_SIZE * (max_depth +1) * sizeof(char));

	for (int d = min_depth; d <= max_depth; d += 2) 
	{
		int iterations = 1 << (max_depth - d + min_depth);
		int c = 0;

		for (int i = 1; i <= iterations; ++i) 
		{
			NodePool store;
			Node *a = make(i, d, store), *b = make(-i, d, store);
			c += a->check() + b->check();
		}

		// each thread write to separate location
		sprintf(outputstr + LINE_SIZE * d, "%d\t trees of depth %d\t check: %d\n", (2 * iterations), d, c);
	}

	// print all results
	for (int d = min_depth; d <= max_depth; d += 2) 
		printf("%s", outputstr + (d * LINE_SIZE) );
	free(outputstr);

	std::cout << "long lived tree of depth " << max_depth << "\t "
		<< "check: " << (long_lived_tree->check()) << "\n";

	high_resolution_timer::duration dur = timer.pulse();

	std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << std::endl;
	return 0;
}
