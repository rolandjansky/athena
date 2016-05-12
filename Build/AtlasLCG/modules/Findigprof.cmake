# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findigprof.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - IGPROF_FOUND
#  - IGPROF_INCLUDE_DIR
#  - IGPROF_INCLUDE_DIRS
#  - IGPROF_<component>_FOUND
#  - IGPROF_<component>_LIBRARY
#  - IGPROF_LIBRARIES
#  - IGPROF_LIBRARY_DIR
#  - IGPROF_LIBRARY_DIRS
#
# The script can be steered by IGPROF_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME igprof
   INCLUDE_SUFFIXES include INCLUDE_NAMES igprof/sym-resolve.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS igprof )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( igprof DEFAULT_MSG IGPROF_INCLUDE_DIR
   IGPROF_LIBRARIES )
mark_as_advanced( IGPROF_FOUND IGPROF_INCLUDE_DIR IGPROF_INCLUDE_DIRS
   IGPROF_LIBRARIES IGPROF_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( igprof )
