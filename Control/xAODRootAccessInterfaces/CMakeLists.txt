# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Build configuration for the xAODRootAccessInterfaces package.
#

# Declare the name of this package:
atlas_subdir( xAODRootAccessInterfaces )

# This package uses ROOT:
find_package( ROOT COMPONENTS Core )

# Build a library that other components can link against:
atlas_add_library( xAODRootAccessInterfaces
   xAODRootAccessInterfaces/*.h xAODRootAccessInterfaces/*.icc Root/*.cxx
   PUBLIC_HEADERS xAODRootAccessInterfaces
   LINK_LIBRARIES ${ROOT_LIBRARIES}
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )
