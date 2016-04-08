# --------------------------------------------------------------
# GNUmakefile for Endcap code within LArG4Sim
# --------------------------------------------------------------

name := LArG4EC
G4TARGET := $(name)
G4EXLIB := true

# Note that we're only compiling a library; this GNUmakefile cannot
# create a binary on its own.

.PHONY: all
all: lib

# Standard G4 architecture.
include $(G4INSTALL)/config/architecture.gmk

CPPFLAGS += $(LARG4CFLAGS)

# Standard G4 compilation process.
include $(G4INSTALL)/config/binmake.gmk

# Include a "realclean" to get rid of excess baggage.
.PHONY: realclean
realclean: 
	@echo "Also deleting old Endcap backups..."
	@rm -f *~ *.bak LArG4EC/*~ LArG4EC/*.bak src/*~ src/*.bak
