# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: CMakeLists.txt 744450 2016-05-03 12:38:18Z krasznaa $

# Declare the package name:
atlas_subdir( GeoPrimitives )

# External dependencies:
find_package( CLHEP QUIET )
find_package( Eigen )

# Decide whether to use CLHEP:
if( CLHEP_FOUND )
   set( clhep_includes ${CLHEP_INCLUDE_DIRS} )
   set( clhep_libs ${CLHEP_LIBRARIES} )
   set( clhep_defs DEFINITIONS ${CLHEP_DEFINITIONS} )
endif()

# The main interface library of the package:
atlas_add_library( GeoPrimitives
   GeoPrimitives/*.h
   INTERFACE
   PUBLIC_HEADERS GeoPrimitives
   INCLUDE_DIRS ${clhep_includes} ${EIGEN_INCLUDE_DIRS}
   ${clhep_defs}
   LINK_LIBRARIES ${clhep_libs} ${EIGEN_LIBRARIES} CxxUtils EventPrimitives )

# The dictionary can only be built if CLHEP is available:
if( CLHEP_FOUND )
   atlas_add_dictionary( GeoPrimitivesDict
      GeoPrimitives/GeoPrimitivesDict.h
      GeoPrimitives/selection.xml
      LINK_LIBRARIES GeoPrimitives )
endif()
