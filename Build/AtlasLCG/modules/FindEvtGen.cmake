# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindEvtGen.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding EvtGen in the LCG release. Defines:
#  - EVTGEN_FOUND
#  - EVTGEN_INCLUDE_DIR
#  - EVTGEN_INCLUDE_DIRS
#  - EVTGEN_LIBRARIES
#  - EVTGEN_LIBRARY_DIRS
#
# Can be steered by EVTGEN_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME EvtGen
   INCLUDE_SUFFIXES include INCLUDE_NAMES EvtGen/EvtGen.hh
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS EvtGen )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( EvtGen DEFAULT_MSG EVTGEN_INCLUDE_DIR
   EVTGEN_LIBRARIES )
mark_as_advanced( EVTGEN_FOUND EVTGEN_INCLUDE_DIR EVTGEN_INCLUDE_DIRS
   EVTGEN_LIBRARIES EVTGEN_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( evtgen )
