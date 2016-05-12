# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHepMCAnalysis.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding HepMCAnalysis in the LCG release. Defines:
#  - HEPMCANALYSIS_FOUND
#  - HEPMCANALYSIS_INCLUDE_DIR
#  - HEPMCANALYSIS_INCLUDE_DIRS
#  - HEPMCANALYSIS_LIBRARIES
#  - HEPMCANALYSIS_LIBRARY_DIRS
#
# Can be steered by HEPMCANALYSIS_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME HepMCAnalysis
   INCLUDE_SUFFIXES include INCLUDE_NAMES baseAnalysis.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS HepMCAnalysis )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( HepMCAnalysis DEFAULT_MSG
   HEPMCANALYSIS_INCLUDE_DIR HEPMCANALYSIS_LIBRARIES )
mark_as_advanced( HEPMCANALYSIS_FOUND HEPMCANALYSIS_INCLUDE_DIR
   HEPMCANALYSIS_INCLUDE_DIRS HEPMCANALYSIS_LIBRARIES
   HEPMCANALYSIS_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( hepmcanalysis )
