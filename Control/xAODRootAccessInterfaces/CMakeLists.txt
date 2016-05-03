# $Id: CMakeLists.txt 744380 2016-05-03 09:18:10Z krasznaa $
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
