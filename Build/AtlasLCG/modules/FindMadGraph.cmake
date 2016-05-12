# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindMadGraph.cmake 733913 2016-04-05 08:46:10Z krasznaa $
#
# Module for finding MadGraph.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME MadGraph
   INCLUDE_NAMES madgraph/core/base_objects.py
   SEARCH_PATHS ${MADGRAPH5AMC_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MadGraph DEFAULT_MSG MADGRAPH_INCLUDE_DIR )
mark_as_advanced( MADGRAPH_FOUND MADGRAPH_INCLUDE_DIR MADGRAPH_INCLUDE_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( madgraph5amc FOUND_NAME MADGRAPH VERSION_NAME MADGRAPH5AMC )

# Set the MADPATH environment variable:
if( MADGRAPH_FOUND )
   set( MADGRAPH_ENVIRONMENT
      SET MADPATH ${MADGRAPH5AMC_ROOT} )
endif()

# Unset the include directories. They would just pollute the runtime
# environment.
unset( MADGRAPH_INCLUDE_DIR )
unset( MADGRAPH_INCLUDE_DIRS )
