# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCrmc.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Crmc / Epos in the LCG release. Defines:
#  - CRMC_FOUND
#  - CRMC_INCLUDE_DIR
#  - CRMC_INCLUDE_DIRS
#  - CRMC_LIBRARIES
#  - CRMC_LIBRARY_DIRS
#
# Can be steered by CRMC_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Crmc
   INCLUDE_SUFFIXES src INCLUDE_NAMES CRMCconfig.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS CrmcBasic HepEvtDummy )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Crmc DEFAULT_MSG CRMC_INCLUDE_DIR
   CRMC_LIBRARIES )
mark_as_advanced( CRMC_FOUND CRMC_INCLUDE_DIR CRMC_INCLUDE_DIRS
   CRMC_LIBRARIES CRMC_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( crmc )
