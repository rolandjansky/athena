# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHepMC.cmake 727592 2016-03-04 07:47:11Z krasznaa $
#
# - Locate HepMC package
# Defines:
#
#  HEPMC_FOUND
#  HEPMC_INCLUDE_DIR
#  HEPMC_INCLUDE_DIRS
#  HEPMC_<component>_FOUND
#  HEPMC_<component>_LIBRARY
#  HEPMC_LIBRARIES
#  HEPMC_LIBRARY_DIRS
#
# Can be steered by HEPMC_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME HepMC
   INCLUDE_SUFFIXES include INCLUDE_NAMES HepMC/HepMCDefs.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS HepMC )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( HepMC DEFAULT_MSG HEPMC_INCLUDE_DIR
   HEPMC_LIBRARIES )
mark_as_advanced( HEPMC_FOUND HEPMC_INCLUDE_DIR HEPMC_INCLUDE_DIRS
   HEPMC_LIBRARIES HEPMC_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( HepMC )
