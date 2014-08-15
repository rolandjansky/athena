PACKAGE=extractor.exe
CC=g++

BASEDIR = .
SRCDIR  = $(BASEDIR)/src
INCDIR  = $(BASEDIR)/*
OBJDIR  = $(BASEDIR)/tmp
EXEDIR  = $(BASEDIR)/bin

TARGET  = $(EXEDIR)/$(PACKAGE)

SKIPLIST = RootDictionary.cxx 
CCLIST = $(filter-out $(SKIPLIST),$(patsubst $(SRCDIR)/%cxx, %cxx, $(wildcard $(SRCDIR)/*.cxx)))
#CCLIST += RootDictionary.cxx

OBJECTS = $(patsubst %.cxx,%.o,$(CCLIST))

vpath %.h   $(INCDIR) $(SRCDIR)
vpath %.cxx $(SRCDIR)
vpath %.o   $(OBJDIR)

INCFLAGS = -I$(INCDIR)
INCFLAGS += $(shell root-config --cflags) 

ROOTLIBS = $(shell root-config --libs) 

LIBS  = $(shell root-config --ldflags)
LIBS += $(ROOTLIBS) -lboost_regex

all: CC += -O3
all: $(TARGET)

debug: CC += -g3
debug: $(TARGET)

$(TARGET): dirs $(OBJECTS) $(INCLUDES)
	@echo $(CCLIST)
	@echo "**"
	@echo "** Linking Executable"
	@echo "**"
	$(CC) $(addprefix $(OBJDIR)/, $(notdir $(OBJECTS))) $(addprefix $(OBJDIR)/, $(notdir $(HISTOBJ))) $(LIBS) -o $(TARGET)

dirs:
	@mkdir -p $(SRCDIR)
	@mkdir -p $(EXEDIR)
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)
	@rm -f $(SRCDIR)/*~
	@rm -f $(SRCDIR)/.*.swp
	
veryclean: clean
	@rm -f $(SRCDIR)/RootDictionary.*
	@rm -rf $(EXEDIR)

%.o: %.cxx  
	@echo "**"
	@echo "** Compiling C++ Source" 
	@echo "**"
	$(CC) -c $(INCFLAGS) $< -o $(OBJDIR)/$(notdir $@)
