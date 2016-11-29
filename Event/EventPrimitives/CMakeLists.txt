# $Id: CMakeLists.txt 744645 2016-05-03 19:12:58Z krasznaa $
################################################################################
# Package: EventPrimitives
################################################################################

# Declare the package name:
atlas_subdir( EventPrimitives )

# External dependencies:
find_package( CLHEP QUIET )
find_package( Eigen )

# Decide whether to use CLHEP:
if( CLHEP_FOUND )
   set( clhep_includes ${CLHEP_INCLUDE_DIRS} )
   set( clhep_libs ${CLHEP_LIBRARIES} )
   set( clhep_defs DEFINITIONS ${CLHEP_DEFINITIONS} )
endif()

# Component(s) in the package:
atlas_add_library( EventPrimitives
   EventPrimitives/*.h
   INTERFACE
   PUBLIC_HEADERS EventPrimitives
   INCLUDE_DIRS ${clhep_includes} ${EIGEN_INCLUDE_DIRS}
   ${clhep_defs}
   LINK_LIBRARIES ${clhep_libs} ${EIGEN_LIBRARIES} )
