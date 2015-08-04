
CC = gcc
.SUFFIXES=.c .o

#==============================================================================
# Define compilation target
#==============================================================================
  TARGET := dino-panic
# If the DEBUG flag was set, generate another binary (so it doesn't collide
# with the release one)
  ifeq ($(DEBUG), yes)
    TARGET := $(TARGET)_dbg
  endif
#==============================================================================

#==============================================================================
# Define every object required by compilation
#==============================================================================
  OBJS =                             \
          $(OBJDIR)/collision.o      \
          $(OBJDIR)/doc.o            \
          $(OBJDIR)/state_intro.o    \
          $(OBJDIR)/main.o           \
          $(OBJDIR)/player.o         
#==============================================================================

#==============================================================================
# Set OS flag
#==============================================================================
  OS := $(shell uname)
  ifeq ($(OS), MINGW32_NT-6.1)
    OS := Win
#   Also, set the icon
    ICON = $(WINICON)
  endif
#==============================================================================

#==============================================================================
# Define CFLAGS (compiler flags)
#==============================================================================
# Add all warnings and default include path
  CFLAGS := -Wall -I"./include/"
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(ARCH), x86_64)
    CFLAGS := $(CFLAGS) -m64
  else
    CFLAGS := $(CFLAGS) -m32
  endif
# Add debug flags
  ifneq ($(RELEASE), yes)
    CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
  else
    CFLAGS := $(CFLAGS) -O3
  endif
# Set flags required by OS
  ifeq ($(OS), Win)
    CFLAGS := $(CFLAGS) -I"/d/windows/mingw/include" -I/c/GFraMe/include
  else
    CFLAGS := $(CFLAGS) -fPIC
  endif
#==============================================================================

#==============================================================================
# Define LFLAGS (linker flags)
#==============================================================================
# Add the framework
  ifeq ($(RELEASE), yes)
    LFLAGS := -lGFraMe
  else
    LFLAGS := -lGFraMe_dbg
  endif
# Append SDL2
  LFLAGS := $(LFLAGS) -lSDL2
# Add libs and paths required by an especific OS
  ifeq ($(OS), Win)
    LFLAGS := -mwindows -lmingw32 $(LFLAGS) -lSDL2main
    LFLAGS := -L/d/windows/mingw/mingw32/lib $(LFLAGS)
# Prepend the framework search path
    LFLAGS := -L/c/GFraMe/lib/ $(LFLAGS)
# TODO Add OpenGL
  else
# Prepend the framework search path
    LFLAGS := -L/usr/lib/GFraMe/ $(LFLAGS)
# TODO Add OpenGL
  endif
# Add pthread (pthread is used for loading stuff)
  LFLAGS := $(LFLAGS) -lpthread
#==============================================================================

#==============================================================================
# Define where source files can be found and where objects & binary are output
#==============================================================================
 VPATH := src
 OBJDIR := obj/$(OS)
 BINDIR := bin/$(OS)
#==============================================================================

#==============================================================================
# Define the generated icon
#==============================================================================
 WINICON := assets/icon.o
#==============================================================================

#==============================================================================
# Make the objects list constant (and the icon, if any)
#==============================================================================
 OBJS := $(OBJS)
#==============================================================================

#==============================================================================
# Define default compilation rule
#==============================================================================
all: MAKEDIRS $(BINDIR)/$(TARGET)
	date
#==============================================================================

#==============================================================================
# Define a rule to generated the icon
#==============================================================================
$(WINICON):
	windres assets/icon.rc $(WINICON)
#==============================================================================

#==============================================================================
# Rule for actually building the game
#==============================================================================
$(BINDIR)/$(TARGET): MAKEDIRS $(OBJS)  $(ICON)
	gcc $(CFLAGS) -o $@ $(OBJS) $(ICON) $(LFLAGS)
#==============================================================================

#==============================================================================
# Rule for compiling any .c in its object
#==============================================================================
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
#==============================================================================

#==============================================================================
# Rule for creating every directory
#==============================================================================
MAKEDIRS: | $(OBJDIR)
#==============================================================================

#==============================================================================
# Rule for actually creating every directory
#==============================================================================
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
#==============================================================================

.PHONY: clean mostlyclean
clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)/$(TARGET)

mostlyclean: clean
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

