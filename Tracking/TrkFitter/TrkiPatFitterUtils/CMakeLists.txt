# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkiPatFitterUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkiPatFitterUtils
                   src/ExtrapolationType.cxx
                   src/FitMatrices.cxx
                   src/FitMeasurement.cxx
                   src/FitParameters.cxx
                   src/FitProcedure.cxx
                   src/FitProcedureQuality.cxx
                   src/MeasurementProcessor.cxx
                   src/MeasurementType.cxx
                   src/MessageHelper.cxx
                   src/ParameterType.cxx
                   PUBLIC_HEADERS TrkiPatFitterUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthContainers GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives TrkGeometry TrkParameters CxxUtils
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkTrack TrkExInterfaces TrkExUtils )


# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/FitMatrices.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
