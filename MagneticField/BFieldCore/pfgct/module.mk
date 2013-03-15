# Test for F90 compiler
CXXFLAGS += -DHAVE_NEW_IOSTREAMS -DHAVE_CXX_STDC_HEADERS

# Source files to compile
FILES := src/*.F src/*.cxx bsi/*.F src/*.F90

USEWILDCARD := 1

# Header files to use for dictionary generation
# In this module: none
DICTFILES := 

# Executable files
PROGRAMS := 

# This module needs 
NEEDS_F90  := yes
