# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindMadGraph.cmake 793619 2017-01-24 12:01:13Z krasznaa $
#
# Module for finding MadGraph.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME MadGraph
   INCLUDE_NAMES madgraph/core/base_objects.py
   SEARCH_PATHS ${MADGRAPH5AMC_ROOT} )

# MadGraph just holds python code. So now set the correct variable for the
# proper environment setting:
set( MADGRAPH_PYTHON_PATH ${MADGRAPH_INCLUDE_DIRS} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MadGraph DEFAULT_MSG MADGRAPH_INCLUDE_DIR )
mark_as_advanced( MADGRAPH_FOUND MADGRAPH_INCLUDE_DIR MADGRAPH_INCLUDE_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( madgraph5amc FOUND_NAME MADGRAPH VERSION_NAME MADGRAPH5AMC )

# Set the MADPATH environment variable:
if( MADGRAPH_FOUND )
   set( MADGRAPH_ENVIRONMENT
      FORCESET MADPATH ${MADGRAPH5AMC_ROOT} )
endif()

# Unset the include directories. They would just pollute the runtime
# environment.
unset( MADGRAPH_INCLUDE_DIR )
unset( MADGRAPH_INCLUDE_DIRS )
