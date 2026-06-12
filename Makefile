USE_64BIT = NO
USE_UNICODE = NO
USE_CLANG = NO
# use -static for clang and cygwin/mingw
USE_STATIC = NO

#  clang++ vs tdm g++
#  clang gives *much* clearer compiler error messages...
#  However, programs built with clang++ will require libc++.dll.
#  in order to be used elsewhere.
#  That is why the executable files are smaller than TDM ...
#  (This can be eliminated by linking with -static, 
#  but that will massively increase file size.)
ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
else
ifeq ($(USE_CLANG),YES)
TOOLS=d:\clang\bin
else
TOOLS=d:\tdm32\bin
endif
endif

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
LFLAGS=
else
CFLAGS=-Wall -O3
LFLAGS=-s -mwindows
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Weffc++
CFLAGS += -Ider_libs
LiFLAGS += -Ider_libs

LINTFILES=lintdefs.cpp lintdefs.ref.h 

BIN=binclock
BINX=$(BIN).exe

# link application-specific sources
CSRC=$(BIN).cpp about.cpp bclk_elements.cpp config.cpp \
der_libs/hyperlinks.cpp \
der_libs/common_funcs.cpp \
der_libs/winmsgs.cpp

OBJS = $(CSRC:.cpp=.o) rc.o

#************************************************************
%.o: %.cpp
	$(TOOLS)/g++ $(CFLAGS) -c $< -o $@

all: $(BINX)

clean:
	rm -f *.exe *.zip *.bak $(OBJS)

dist:
	rm -f *.zip
	zip $(BIN).zip $(BIN).exe *.bmp readme.md LICENSE

wc:
	wc -l $(CSRC) *.rc

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 mingw.lnt -os(_lint.tmp) $(LINTFILES) $(CSRC)"

depend:
	makedepend $(CFLAGS) $(CSRC)

#************************************************************

$(BIN).exe: $(OBJS)
	$(TOOLS)/g++ $(LFLAGS) $(OBJS) -o $@

#	\\InnoSetup5\iscc /Q winagrams.iss

rc.o: $(BIN).rc
	$(TOOLS)\windres $< -O coff -o $@

# DO NOT DELETE

binclock.o: version.h resource.h der_libs/common.h binclock.h bclk_elements.h
about.o: resource.h version.h der_libs/hyperlinks.h
bclk_elements.o: der_libs/common.h bclk_elements.h
config.o: der_libs/common.h binclock.h
der_libs/hyperlinks.o: der_libs/iface_32_64.h der_libs/hyperlinks.h
der_libs/common_funcs.o: der_libs/common.h
