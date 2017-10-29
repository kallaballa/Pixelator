CXX      := g++
CXXFLAGS := -pthread -fno-strict-aliasing -std=c++0x -pedantic -Wall `pkg-config --cflags cairo x11 SDL_gfx` -I/usr/local/include/opencv2
LDFLAGS  := -L/opt/local/lib -L../SDL_gfx-2.0.26 -L/usr/local/lib/
LIBS     := -lm `pkg-config --libs x11 sdl SDL_image SDL_gfx libpng16` -lSDL_gfx -lboost_serialization -lboost_system -lboost_filesystem -lopencv_flann -lopencv_core -lopencv_legacy -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_ocl -lopencv_features2d 
.PHONY: all release debian-release info debug clean debian-clean distclean 
DESTDIR := /
PREFIX := /usr/local
MACHINE := $(shell uname -m)
WHERECXX := $(shell whereis g++-7)

ifneq ($(WHERECXX), g++-7:)
  CXX = g++-7
endif 

ifeq ($(MACHINE), x86_64)
  LIBDIR = lib64
endif
ifeq ($(MACHINE), i686)
  LIBDIR = lib
endif

ifdef JAVASCRIPT
CXX			 := em++
CXXFLAGS += -I/usr/local/include
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -m32 
endif

ifdef STATIC
LDFLAGS += -static-libgcc -Wl,-Bstatic
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -static-libgcc -m32 -Wl,-Bstatic
endif 

ifdef WITHOUT_OPENCV
CXXFLAGS += -D_NO_OPENCV
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/
else
 CXXFLAGS += -march=native
endif

all: release

ifneq ($(UNAME_S), Darwin)
release: LDFLAGS += -s
endif
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O0
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

profile: CXXFLAGS += -g3 -O1
profile: LDFLAGS += -Wl,--export-dynamic -rdynamic
profile: dirs

hardcore: CXXFLAGS += -g0 -Ofast -DNDEBUG
ifeq ($(UNAME_S), Darwin)
hardcore: LDFLAGS += -s
endif
hardcore: dirs

clean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}

debian-release:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" release

debian-clean:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" clean

install: ${TARGET}
	true

distclean:
	true

