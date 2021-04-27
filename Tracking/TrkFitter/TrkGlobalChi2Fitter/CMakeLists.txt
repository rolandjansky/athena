# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir(TrkGlobalChi2Fitter)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component (
    TrkGlobalChi2Fitter
    src/*.cxx
    src/components/*.cxx
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AtlasDetDescr EventPrimitives GaudiKernel IdDictDetDescr InDetReadoutGeometry MagFieldConditions MagFieldElements TrkCompetingRIOsOnTrack TrkDetDescrInterfaces TrkEventPrimitives TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkFitterUtils TrkGeometry TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkSurfaces TrkToolInterfaces TrkTrack TrkTrackSummary TrkVertexOnTrack TrkVolumes )


# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/GlobalChi2Fitter.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
