
NEEDS_F90 := yes

CXXFLAGS += -DHAVE_NEW_IOSTREAMS -DHAVE_CXX_STDC_HEADERS

# Source files to compile (without suffix)
FILES := src/*.FF90 src/*.F90 src/*.cxx

USEWILDCARD := 1

# Header files to use for dictionary generation
# In this module: none
DICTFILES := 

# Executable files
PROGRAMS := 

# This module needs LAPACK
#NEEDS_LAPACK := yes
