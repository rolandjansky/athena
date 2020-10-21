# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
