# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_SeededTrackFinderTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_SeededTrackFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces MagFieldConditions MagFieldElements SiSPSeededTrackFinderData StoreGateLib TrkCaloClusterROI TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkGeometry TrkParameters TrkPseudoMeasurementOnTrack TrkSurfaces TrkToolInterfaces TrkTrack )
