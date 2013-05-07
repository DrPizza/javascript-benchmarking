INCLUDES=./include

CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -O3 -I$(INCLUDES)
CC=clang
CFLAGS=-lm -O3 -I$(INCLUDES)

EMCC=../emscripten/emcc
EMCCFLAGS=--jcache -O2 -I$(INCLUDES) --llvm-lto 1
EMCCCXXFLAGS=-std=c++11
EMCCCPPFLAGS=

JSFLAGS=--closure 1 -s TOTAL_MEMORY=536870912
ASMJSFLAGS=-s ASM_JS=1 -s TOTAL_MEMORY=536870912

SOURCES=$(wildcard *-generic/*.cpp) $(wildcard *-generic/*.c)
PROGRAMS=$(basename $(SOURCES))

SUFFIXES=-js.html -asmjs.html
HTMLS=$(foreach SUFFIX, $(SUFFIXES), $(addsuffix $(SUFFIX), $(PROGRAMS)))

%-asmjs.html: %.cpp
	$(EMCC) $(EMCCFLAGS) $(EMCCCXXFLAGS) $(ASMJSFLAGS) $< -o $@

%-js.html: %.cpp
	$(EMCC) $(EMCCFLAGS) $(EMCCCXXFLAGS) $(JSFLAGS) $< -o $@

%-asmjs.html: %.c
	$(EMCC) $(EMCCFLAGS) $(EMCCCPPFLAGS) $(ASMJSFLAGS) $< -o $@

%-js.html: %.c
	$(EMCC) $(EMCCFLAGS) $(EMCCCPPFLAGS) $(JSFLAGS) $< -o $@

define PROGRAM_template
$(1)-all: $(1) $(1)-js.html $(1)-asmjs.html
endef

$(foreach prog, $(PROGRAMS), $(eval $(call PROGRAM_template, $(prog))))

clean:
	rm -f $(PROGRAMS)
	rm -f $(HTMLS)
	rm -f ~/public_html/benches/*.html

all: $(addsuffix -all, $(PROGRAMS))
	cp $(HTMLS) ~/public_html/benches/

.PHONY: all clean

