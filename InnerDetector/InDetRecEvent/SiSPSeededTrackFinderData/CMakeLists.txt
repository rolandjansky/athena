# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSPSeededTrackFinderData )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( SiSPSeededTrackFinderData
                   src/*.cxx
                   PUBLIC_HEADERS SiSPSeededTrackFinderData
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel InDetPrepRawData InDetReadoutGeometry SiSpacePointsSeed TrkEventPrimitives TrkExInterfaces TrkGeometry TrkPatternParameters TrkSurfaces TrkTrack TrkCaloClusterROI TrkToolInterfaces TrkEventUtils MagFieldElements MagFieldConditions InDetConditionsSummaryService ActsTrkEvent
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} InDetRIO_OnTrack StoreGateLib TrkExUtils TrkMaterialOnTrack TrkPrepRawData TrkRIO_OnTrack TrkSpacePoint)
