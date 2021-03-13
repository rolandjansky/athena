# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name: 
atlas_subdir( TrkSurfaces )

# Component(s) in the package:
atlas_add_library( TrkSurfaces
   TrkSurfaces/*.h src/*.cxx
   PUBLIC_HEADERS TrkSurfaces
   LINK_LIBRARIES AthenaKernel AthContainers CxxUtils GeoPrimitives Identifier EventPrimitives TrkParametersBase
   GaudiKernel TrkDetDescrUtils TrkDetElementBase TrkEventPrimitives )

# Some functions here make heavy use of Eigen and are thus very slow in debug
# builds.  Set up to allow forcing them to compile with optimization and
# inlining, even in debug builds.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/Surface.cxx
     ${CMAKE_CURRENT_SOURCE_DIR}/src/DiscSurface.cxx
     ${CMAKE_CURRENT_SOURCE_DIR}/src/PlaneSurface.cxx
     ${CMAKE_CURRENT_SOURCE_DIR}/src/CylinderSurface.cxx
     ${CMAKE_CURRENT_SOURCE_DIR}/src/StraightLineSurface.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

atlas_add_dictionary( TrkSurfacesDict
   TrkSurfaces/TrkSurfacesDict.h
   TrkSurfaces/selection.xml
   LINK_LIBRARIES TrkSurfaces )

atlas_add_test( CylinderSurface_test
                SOURCES test/CylinderSurface_test.cxx
                LINK_LIBRARIES TrkEventPrimitives TrkSurfaces TestTools )
