# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindSherpa.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Sherpa in the LCG release. Defines:
#  - SHERPA_FOUND
#  - SHERPA_INCLUDE_DIR
#  - SHERPA_INCLUDE_DIRS
#  - SHERPA_<component>_FOUND
#  - SHERPA_<component>_LIBRARY
#  - SHERPA_LIBRARIES
#  - SHERPA_LIBRARY_DIRS
#
# Can be steered by SHERPA_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Sherpa
   INCLUDE_SUFFIXES include/SHERPA-MC INCLUDE_NAMES SHERPA/Main/Sherpa.H
   LIBRARY_SUFFIXES lib/SHERPA-MC
   COMPULSORY_COMPONENTS SherpaMain ToolsMath ToolsOrg )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Sherpa DEFAULT_MSG SHERPA_INCLUDE_DIR
   SHERPA_LIBRARIES )
mark_as_advanced( SHERPA_FOUND SHERPA_INCLUDE_DIR SHERPA_INCLUDE_DIRS
   SHERPA_LIBRARIES SHERPA_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( sherpa )
