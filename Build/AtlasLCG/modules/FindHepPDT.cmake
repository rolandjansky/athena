# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHepPDT.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Locate HepPDT library
# Defines:
#
#  HEPPDT_FOUND
#  HEPPDT_INCLUDE_DIR
#  HEPPDT_INCLUDE_DIRS
#  HEPPDT_<component>_LIBRARY
#  HEPPDT_<component>_FOUND
#  HEPPDT_LIBRARIES
#  HEPPDT_LIBRARY_DIRS
#
# Can be steered by HEPPDT_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# First just find the headers:
lcg_external_module( NAME HepPDT
  INCLUDE_SUFFIXES include INCLUDE_NAMES HepPDT/ParticleData.hh
  LIBRARY_SUFFIXES lib
  COMPULSORY_COMPONENTS HepPDT HepPID )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( HepPDT DEFAULT_MSG
  HEPPDT_INCLUDE_DIR HEPPDT_LIBRARIES )
mark_as_advanced( HEPPDT_FOUND HEPPDT_INCLUDE_DIR HEPPDT_INCLUDE_DIRS
  HEPPDT_LIBRARIES HEPPDT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( HepPDT )
