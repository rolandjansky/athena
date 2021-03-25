# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExSTEP_Propagator )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkExSTEP_Propagator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives TrkGeometry TrkEventPrimitives
		             TrkMaterialOnTrack TrkParameters TrkExInterfaces TrkExUtils GaudiKernel TrkDetDescrUtils TrkSurfaces TrkTrack
		             MagFieldElements MagFieldConditions CxxUtils)


# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/STEP_Propagator.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
