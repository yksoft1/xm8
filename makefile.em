#
# eXellent Multi-platform emulator type 8 - 'XM8'
# based on ePC-8801MA
#
# Author (ePC-8801MA) : Takeda.Toshiya
# Author (XM8) : Tanaka.Yasushi
#
# [ makefile for Linux ]
#

#
# tool chain
#

CXX = em++
SDL_CONFIG ?= C:/emsdk/emscripten/1.38.12/system/bin/sdl-config
#
# options for SDL
#

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags) -DUSE_SDL_CONFIG -s USE_SDL=2
SDL_LDFLAGS := $(shell $(SDL_CONFIG) --libs)


#WebAssembly seemed to be a bit faster than asm.js
WASM=1

#Set maximum RAM that Emscripten use (in bytes).
EMSCRIPTEN_TOTAL_MEMORY=67108864

#
# compile options
#

CXXFLAGS = -DSDL -s USE_SDL=2  -D_PC8801MA -c -O3 -Wall

#
# include directories
#

INCFLAGS = -IUI -IePC-8801MA -IePC-8801MA/vm -IePC-8801MA/vm/pc8801 -IePC-8801MA/vm/fmgen

#
# default rule
#
.cpp.o: 
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) $(INCFLAGS) $< -o $@

#
# target name
#
TARGET = xm8.bc

#
# source files
#
SRCS = \
	UI/app.cpp \
	UI/audio.cpp \
	UI/converter.cpp \
	UI/diskmgr.cpp \
	UI/emu.cpp \
	UI/emu_sdl.cpp \
	UI/file.cpp \
	UI/fileio.cpp \
	UI/font.cpp \
	UI/input.cpp \
	UI/main.cpp \
	UI/menu.cpp \
	UI/menuitem.cpp \
	UI/menulist.cpp \
	UI/platform.cpp \
	UI/setting.cpp \
	UI/softkey.cpp \
	UI/tapemgr.cpp \
	UI/video.cpp \
	ePC-8801MA/common.cpp \
	ePC-8801MA/config.cpp \
	ePC-8801MA/fifo.cpp \
	ePC-8801MA/vm/disk.cpp \
	ePC-8801MA/vm/event.cpp \
	ePC-8801MA/vm/i8251.cpp \
	ePC-8801MA/vm/i8253.cpp \
	ePC-8801MA/vm/i8255.cpp \
	ePC-8801MA/vm/disksub.cpp \
	ePC-8801MA/vm/pcm1bit.cpp \
	ePC-8801MA/vm/upd765a.cpp \
	ePC-8801MA/vm/upd1990a.cpp \
	ePC-8801MA/vm/fmsound.cpp \
	ePC-8801MA/vm/z80.cpp \
	ePC-8801MA/vm/fmgen/opna.cpp \
	ePC-8801MA/vm/fmgen/psg.cpp \
	ePC-8801MA/vm/fmgen/fmgen.cpp \
	ePC-8801MA/vm/fmgen/fmtimer.cpp \
	ePC-8801MA/vm/pc8801/pc88.cpp \
	ePC-8801MA/vm/pc8801/pc8801.cpp

OBJS = $(SRCS:%.cpp=%.o)

#
# make or make all:
# make target
#
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(SDL_LDFLAGS) -static
	

html: $(TARGET)
ifeq ($(PRELOAD), 1)
	$(CXX) -O3 -s USE_SDL=2 -s WASM=$(WASM) -s TOTAL_MEMORY=$(EMSCRIPTEN_TOTAL_MEMORY)  -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 \
	-s EMTERPRETIFY_WHITELIST=@d3.txt $(TARGET) --profiling-funcs \
	--preload-file $(PREFILE) -o $(basename $(TARGET)).html 
else
	$(CXX) -O3 -s USE_SDL=2 -s WASM=$(WASM) -s TOTAL_MEMORY=$(EMSCRIPTEN_TOTAL_MEMORY)  -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 \
	-s EMTERPRETIFY_WHITELIST=@d3.txt $(TARGET) -o $(basename $(TARGET)).html 
endif

#
# make clean:
# clean object and executable file
#
clean:
	rm $(OBJS)
	rm $(TARGET)

