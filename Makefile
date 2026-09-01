USE_64BIT = NO
USE_UNICODE = NO
USE_CLANG = NO
# use -static for clang and cygwin/mingw
USE_STATIC = NO

include der_libs\tool_select.mak

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
LFLAGS=
else
CFLAGS=-Wall -O3
LFLAGS=-s -mwindows
endif
CFLAGS += -Wno-write-strings
# CFLAGS += -Weffc++
CFLAGS += -Ider_libs
LiFLAGS += -Ider_libs

LINTFILES=lintdefs.cpp lintdefs.ref.h 

BASE=binclock
BINX=$(BASE).exe

# link application-specific sources
CSRC=$(BASE).cpp about.cpp bclk_elements.cpp config.cpp \
der_libs/hyperlinks.cpp \
der_libs/common_funcs.cpp \
der_libs/winmsgs.cpp

OBJS = $(CSRC:.cpp=.o) rc.o

# Automatically parse the latest version block
VERSION := $(shell grep -oE '\[[0-9]+\.[0-9]+\]' CHANGELOG.md | head -n 1 | tr -d '[]')
DIST_ZIP := $(BASE)V$(VERSION).zip

# Force these action-only targets to always run
.PHONY: dist release update

#************************************************************
%.o: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) $< -o $@

all: $(BINX)

clean:
	rm -f *.exe *.zip *.bak $(OBJS)

dist:
	rm -f *.zip
	zip $(DIST_ZIP) $(BASE).exe *.bmp readme.md LICENSE.txt CHANGELOG.md

# Your new automated release workflow
release: dist
	@cmd /C "@echo Preparing GitHub release for v$(VERSION)..."
	sed -n '/## \['$(VERSION)'\]/,/## \[/p' CHANGELOG.md | sed '$$d' > temp_notes.md
	gh release create v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --notes-file temp_notes.md
	rm temp_notes.md
	@cmd /C "@echo Release v$(VERSION) successfully uploaded to GitHub!"
	
# Your corrected, bulletproof update-in-place pipeline
update: dist
	@cmd /C "@echo Updating assets for existing release v$(VERSION)..."
	gh release upload v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --clobber
	@cmd /C "@echo Release v$(VERSION) assets successfully updated on GitHub!"

wc:
	wc -l $(CSRC) *.rc

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 mingw.lnt -os(_lint.tmp) $(LINTFILES) $(CSRC)"

check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CSRC)"

depend:
	makedepend $(CFLAGS) $(CSRC)

#************************************************************

$(BINX): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BINX) $(LIBS) 


rc.o: $(BASE).rc
	$(TOOLS)\$(WRNAME) $< -O COFF -o $@

# DO NOT DELETE

binclock.o: version.h resource.h der_libs/common.h binclock.h bclk_elements.h
about.o: resource.h version.h der_libs/hyperlinks.h
bclk_elements.o: der_libs/common.h bclk_elements.h
config.o: der_libs/common.h binclock.h
der_libs/hyperlinks.o: der_libs/iface_32_64.h der_libs/hyperlinks.h
der_libs/common_funcs.o: der_libs/common.h
