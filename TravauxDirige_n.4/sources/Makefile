#include Make_mac.inc
include Make_other.inc
LIBS = -lm -ltbb
CXXFLAGS = -std=c++17 -fPIC
ifdef DEBUG
CXXFLAGS += -g -O0 -Wall -fbounds-check -pedantic -Weffc++ -D_GLIBCXX_DEBUG
CXXFLAGS2 = CXXFLAGS
else
CXXFLAGS += -O3 -march=native
CXXFLAGS2 = ${CXXFLAGS} -O2 -march=native -Wall
endif

ALL = raytracer.exe mandelbrot.exe

default: help

all: $(ALL)

clean:
	@rm -fr *.o *.exe *~

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# dependances particulieres :
raytracer.exe: raytracer.cpp
	$(CXX) $(CXXFLAGS2) $^ -o $@ $(LIBS)	

mandelbrot.exe: mandelbrot.cpp
	$(CXXMPI) $(CXXFLAGS) $^ -o $@ $(LIBMPI) $(LIBS)

help:
	@echo "Available targets:"
	@echo "    all: compile all executables"
	@echo "       $(ALL)"
	@echo "Configuration:"
	@echo "    CXX     : $(CXX)"
	@echo "    CXXFLAGS: $(CXXFLAGS)"
	@echo "Add DEBUG=yes to compile in debug"


# Rapport
%.html: %.md
	pandoc -s --toc $< --css=./github-pandoc.css -o $@
