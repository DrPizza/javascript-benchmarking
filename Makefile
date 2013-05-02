INCLUDES=./include

CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -O3 -I$(INCLUDES)

EMCC=../emscripten/emcc
EMCCFLAGS=-std=c++11 -O2 -I$(INCLUDES) --llvm-lto 1

#JSFLAGS=--closure 1
JSFLAGS=
ASMJSFLAGS=-s ASM_JS=1

SOURCES=$(wildcard *-generic/*.cpp)
PROGRAMS=$(basename $(SOURCES))

SUFFIXES=-js.html -asmjs.html
HTMLS=$(foreach SUFFIX, $(SUFFIXES), $(addsuffix $(SUFFIX), $(PROGRAMS)))

%-asmjs.html: %.cpp
	$(EMCC) $(EMCCFLAGS) $(ASMJSFLAGS) $< -o $@

%-js.html: %.cpp
	$(EMCC) $(EMCCFLAGS) $(JSFLAGS) $< -o $@

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

