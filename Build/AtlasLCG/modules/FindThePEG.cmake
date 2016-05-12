# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindThePEG.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding ThePEG in the LCG release. Defines:
#  - THEPEG_FOUND
#  - THEPEG_INCLUDE_DIR
#  - THEPEG_INCLUDE_DIRS
#  - THEPEG_LIBRARIES
#  - THEPEG_LIBRARY_DIRS
#
# Can be steered by THEPEG_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME ThePEG
   INCLUDE_SUFFIXES include INCLUDE_NAMES ThePEG/EventRecord/Event.h
   LIBRARY_SUFFIXES lib/ThePEG
   COMPULSORY_COMPONENTS ThePEG )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ThePEG DEFAULT_MSG THEPEG_INCLUDE_DIR
   THEPEG_LIBRARIES )
mark_as_advanced( THEPEG_FOUND THEPEG_INCLUDE_DIR THEPEG_INCLUDE_DIRS
   THEPEG_LIBRARIES THEPEG_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( thepeg )
