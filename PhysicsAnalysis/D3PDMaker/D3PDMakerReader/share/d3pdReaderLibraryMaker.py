#!/usr/bin/env python
# $Id: d3pdReaderLibraryMaker.py 600807 2014-06-08 15:26:51Z krasznaa $
#
# This script can be used to generate the LinkDef.h and Makefile for compiling some
# D3PDReader source files into a standalone (shared and static) library.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

# Base module(s):
import sys
import os
import optparse

# Athena module(s):
from AthenaCommon.Logging import logging

# Local module(s):
from D3PDMakerReader.Helpers import *

#
# Template contents of the makefile which is generated. It only needs 4 "parameters" to become a
# usable makefile.
#
makefile_template = \
    "# Dear emacs, this is a -*- Makefile -*-\n" \
    "# Makefile for compiling a set of D3PDReader source files into a shared and/or static\n" \
    "# library. Generated using the D3PDMakerReader package...\n\n" \
    "#\n" \
    "# Include the architecture definitions from the ROOT sources\n" \
    "#\n" \
    "#  Makefile.arch can be in a number of different locations depending on the system\n" \
    "#  you're compiling on. The Fink installed version of ROOT for instance has this file\n" \
    "#  in a different location than the \"normally installed\" ROOT versions...\n" \
    "#\n" \
    "ARCH_LOC_1 := $(wildcard $(shell root-config --prefix)/test/Makefile.arch)\n" \
    "ARCH_LOC_2 := $(wildcard $(shell root-config --prefix)/share/root/test/Makefile.arch)\n" \
    "ARCH_LOC_3 := $(wildcard $(shell root-config --prefix)/share/doc/root/test/Makefile.arch)\n" \
    "ARCH_LOC_4 := $(wildcard $(shell root-config --prefix)/etc/Makefile.arch)\n" \
    "ARCH_LOC_5 := $(wildcard $(shell root-config --prefix)/etc/root/Makefile.arch)\n" \
    "ifneq ($(strip $(ARCH_LOC_1)),)\n" \
    "  $(info Using $(ARCH_LOC_1))\n" \
    "  include $(ARCH_LOC_1)\n" \
    "else\n" \
    "  ifneq ($(strip $(ARCH_LOC_2)),)\n" \
    "    $(info Using $(ARCH_LOC_2))\n" \
    "    include $(ARCH_LOC_2)\n" \
    "  else\n" \
    "    ifneq ($(strip $(ARCH_LOC_3)),)\n" \
    "      $(info Using $(ARCH_LOC_3))\n" \
    "      include $(ARCH_LOC_3)\n" \
    "    else\n" \
    "      ifneq ($(strip $(ARCH_LOC_4)),)\n" \
    "        $(info Using $(ARCH_LOC_4))\n" \
    "        include $(ARCH_LOC_4)\n" \
    "      else\n" \
    "        ifneq ($(strip $(ARCH_LOC_5)),)\n" \
    "          $(info Using $(ARCH_LOC_5))\n" \
    "          include $(ARCH_LOC_5)\n" \
    "        else\n" \
    "          $(error Could not find Makefile.arch!)\n" \
    "        endif\n" \
    "      endif\n" \
    "    endif\n" \
    "  endif\n" \
    "endif\n\n" \
    "# Name of the library to create:\n" \
    "LIBRARY = %s\n\n" \
    "# Set the location of some files:\n" \
    "OBJDIR    = obj\n" \
    "DEPDIR    = $(OBJDIR)/dep\n" \
    "VPATH    += $(OBJDIR) ./\n" \
    "DICTHEAD  = $(LIBRARY)_Dict.h\n" \
    "DICTFILE  = $(LIBRARY)_Dict.$(SrcSuf)\n" \
    "DICTOBJ   = $(OBJDIR)/$(LIBRARY)_Dict.$(ObjSuf)\n" \
    "DICTLDEF  = %s\n" \
    "SHLIBFILE = lib$(LIBRARY).$(DllSuf)\n" \
    "LIBFILE   = lib$(LIBRARY).a\n\n" \
    "# Additional command line options:\n" \
    "CXXFLAGS += -Wall -DCOLLECT_D3PD_READING_STATISTICS\n\n" \
    "# List of all header and source files to build:\n" \
    "HLIST   = %s\n" \
    "CPPLIST = %s\n\n" \
    "# List of all files to build:\n" \
    "OLIST   = $(patsubst %%.$(SrcSuf),%%.o,$(notdir $(CPPLIST)))\n" \
    "DEPLIST = $(foreach var,$(patsubst %%.$(SrcSuf),%%.d,$(notdir $(CPPLIST))),$(DEPDIR)/$(var))\n\n" \
    "# The default is to create both libraries:\n" \
    "default: $(SHLIBFILE) $(LIBFILE)\n\n" \
    "# Rule to compile the source files:\n" \
    "%%.$(ObjSuf): %%.$(SrcSuf)\n" \
    "\t@echo \"Compiling $<\"\n" \
    "\t@mkdir -p $(OBJDIR)\n" \
    "\t@$(CXX) $(CXXFLAGS) -c $< -o $(OBJDIR)/$(notdir $@) $(INCLUDES)\n\n" \
    "# Rule to create the dictionary\n" \
    "$(DICTFILE): $(HLIST) $(DICTLDEF)\n" \
    "\t@echo \"Generating dictionary $@\"\n" \
    "\t@$(shell root-config --exec-prefix)/bin/rootcint -f $(DICTFILE) -c -p $(INCLUDES) $^\n\n" \
    "# Rule to comile the dictionary\n" \
    "$(DICTOBJ): $(DICTFILE) \n" \
    "\t@echo \"Compiling $<\"\n" \
    "\t@mkdir -p $(OBJDIR)\n" \
    "\t@$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $<\n\n" \
    "# Rule to link the shared library\n" \
    "$(SHLIBFILE): $(OLIST) $(DICTOBJ)\n" \
    "\t@echo \"Making shared library: $(SHLIBFILE)\"\n" \
    "\t@rm -f $(SHLIBFILE)\n" \
    "\t@$(LD) $(LDFLAGS) $(SOFLAGS) -O2 $(addprefix $(OBJDIR)/,$(OLIST)) $(DICTOBJ) -o $(SHLIBFILE)\n\n" \
    "# Rule to create the static library\n" \
    "$(LIBFILE): $(OLIST) $(DICTOBJ)\n" \
    "\t@echo \"Making static library: $(LIBFILE)\"\n" \
    "\t@rm -f $(LIBFILE)\n" \
    "\t@ar rcs $@ $(addprefix $(OBJDIR)/,$(OLIST)) $(DICTOBJ)\n\n" \
    "# Rule to clean the directory\n" \
    "clean:\n" \
    "\t@rm -f $(OBJDIR)/*.$(ObjSuf)\n" \
    "\t@rm -f $(DICTOBJ)\n" \
    "\t@rm -f $(DICTFILE) $(DICTHEAD)\n" \
    "\t@rm -f $(LIBFILE)\n" \
    "\t@rm -f $(SHLIBFILE)\n\n" \
    "distclean: clean\n" \
    "\t@rm -rf $(OBJDIR)\n\n" \
    "#\n" \
    "# Dependency generation. This is just copy-pasted from the TMVA makefile...\n" \
    "#\n" \
    "ifneq ($(MAKECMDGOALS),clean)\n" \
    "ifneq ($(MAKECMDGOALS),distclean)\n" \
    "-include $(DEPLIST)\n" \
    "endif\n" \
    "endif\n\n" \
    "$(DEPDIR)/%%.d: %%.$(SrcSuf)\n" \
    "\t@mkdir -p $(DEPDIR)\n" \
    "\t@if test -f $< ; then \\\n" \
    "\t\techo \"Making $(@F)\"; \\\n" \
    "\t\t$(SHELL) -ec '`root-config --exec-prefix`/bin/rmkdepend -f- -Y -w 3000 -- -I./ -- $< 2> /dev/null 1| sed -e \"s-src/\\(.*\\).o\\(: .*\\)-$(DEPDIR)\\/\\1.d $(OBJDIR)/\\1.o\\2-\" > $@'; \\\n" \
    "\tfi\n"

def main():
    # Create a logger object first of all:
    logger = logging.getLogger( os.path.splitext( os.path.basename( sys.argv[ 0 ] ) )[ 0 ] )

    #
    # Specify the command line options:
    #
    desc = "This script can be used to generate a LinkDef.h header and a Makefile " \
           "to compile some D3PDReader source files into a standalone (shared and " \
           "static) library."
    vers = "$Revision: 600807 $"
    parser = optparse.OptionParser( description = desc,
                                    version = vers,
                                    usage = "%prog [options] <source files>" )
    parser.add_option( "-v", "--verbosity", dest = "verbosity",
                       action = "store", type = "int", default = 3,
                       help = "Message verbosity level" )
    parser.add_option( "-o", "--output", dest = "output",
                       action = "store", type = "string", default = "./",
                       help = "Output directory for the generated files" )
    parser.add_option( "-m", "--makefile", dest = "makefile",
                       action = "store", type = "string", default = "Makefile",
                       help = "File name for the generated makefile" )
    parser.add_option( "-l", "--linkdef", dest = "linkdef",
                       action = "store", type = "string", default = "LinkDef.h",
                       help = "File name for the generated linkdef header" )
    parser.add_option( "-n", "--libname", dest = "libname",
                       action = "store", type = "string", default = "D3PDReader",
                       help = "Name of the library to be compiled" )

    # Parse the command line options:
    ( options, files ) = parser.parse_args()

    # Do a small sanity check:
    if not len( files ):
        logger.error( "You have to define at least one source file!" )
        parser.print_help()
        return 255

    # Set the default logging level:
    logging.getLogger( "" ).setLevel( options.verbosity )

    # Print some welcome message:
    logger.info( "***********************************************************" )
    logger.info( "*                D3PDReader Library Maker" )
    logger.info( "* Version: $Revision: 600807 $" )
    logger.info( "*" )
    logger.info( "* Output level: " + str( options.verbosity ) )
    logger.info( "* Output directory: " + options.output )
    logger.info( "* Makefile name: " + options.makefile )
    logger.info( "* LinkDef name: " + options.linkdef )
    logger.info( "* Library name: " + options.libname )
    logger.info( "* Source files: " + ", ".join( files ) )
    logger.info( "***********************************************************" )

    # Filter out the header and source files:
    ( dummy_header_files, source_files ) = separateSources( files )
    header_files = dictionaryHeaders( files )

    # Open the makefile:
    makefile = open( options.output + "/" + options.makefile, "w" )
    if not makefile:
        logger.error( "Couldn't open " + options.output + "/" + options.makefile + " for writing!" )
        return 255
    else:
        logger.info( "Writing makefile to: " + options.output + "/" + options.makefile )
        pass

    # Write the makefile's contents:
    makefile.write( makefile_template % ( options.libname, options.linkdef,
                                          " ".join( header_files ),
                                          " ".join( source_files ) ) )
    makefile.close()

    # Extract the class names from the header files:
    classnames = dictionaryClasses( header_files )
    logger.debug( "The class names found: " + ", ".join( classnames ) )

    # Write the LinkDef file:
    if writeLinkDefFile( options.output + "/" + options.linkdef,
                         classnames ):
        logger.error( "Failed writing the LinkDef file!" )
        return 255

    return 0

if __name__ == "__main__":
    sys.exit( main() )
