# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuidTrackBuilder )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuidTrackBuilder
                     src/CombinedMuonTrackBuilder.cxx
                     src/MuidMuonRecovery.cxx
                     src/MuonMatchQuality.cxx
                     src/MuonTrackQuery.cxx
                     src/OutwardsCombinedMuonTrackBuilder.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel EventPrimitives FourMomUtils GaudiKernel InDetRIO_OnTrack MagFieldConditions MuidEvent MuidInterfaces 
                                                       MuonCombinedToolInterfaces MuonCompetingRIOsOnTrack MuonIdHelpersLib MuonRIO_OnTrack MuonReadoutGeometry MuonRecHelperToolsLib 
                                                       MuonRecToolInterfaces StoreGateLib TrkCompetingRIOsOnTrack TrkDetDescrInterfaces TrkEventPrimitives TrkEventUtils TrkExInterfaces 
                                                       TrkExUtils TrkFitterInterfaces TrkGeometry TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkSegment 
                                                       TrkSurfaces TrkToolInterfaces TrkTrack TrkTrackSummary TrkiPatFitterUtils VxVertex muonEvent  MuonTrackMakerUtils )
