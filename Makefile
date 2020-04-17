CXX_OBJS = \
	source/3dftex.o \
	source/shader.o \
    source/splash.o \
    source/splashdat.o \

CXX=g++

CXXFLAGS += -std=c++14 -Wall -Wextra -Wold-style-cast
CXXFLAGS += -O0 -g3
CXXFLAGS += -I.
CXXFLAGS += -I../

DEP = $(CXX_OBJS:%.o=%.d)

PROGRAM += 3dfx_splash
OUTPUT += 3dfx_splash

PROGRAM : $(CXX_OBJS)
	@echo "LD $@"; $(CXX) $(CXX_OBJS) -o $(OUTPUT) -lGL -lGLEW -lSDL2

.cpp.o:
	@echo "CXX $@"; $(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(PROGRAM)
	rm -f $(CXX_OBJS)