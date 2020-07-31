# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonEvent )

# Component(s) in the package:
atlas_add_library( TrigMuonEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigMuonEvent
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel EventKernel FourMom GaudiKernel Navigation Particle TrigInDetEvent
                   PRIVATE_LINK_LIBRARIES MuidEvent MuonPattern MuonSegment TrigNavigationLib )

atlas_add_sercnv_library( TrigMuonEventSerCnv
                          FILES TrigMuonEvent/MuonFeature.h TrigMuonEvent/MuonFeatureContainer.h TrigMuonEvent/CombinedMuonFeature.h TrigMuonEvent/CombinedMuonFeatureContainer.h TrigMuonEvent/IsoMuonFeature.h TrigMuonEvent/IsoMuonFeatureContainer.h TrigMuonEvent/TrigMuonClusterFeature.h TrigMuonEvent/TrigMuonClusterFeatureContainer.h TrigMuonEvent/TrigMuonEF.h TrigMuonEvent/TrigMuonEFContainer.h TrigMuonEvent/TileMuFeature.h TrigMuonEvent/TileMuFeatureContainer.h TrigMuonEvent/TileTrackMuFeature.h TrigMuonEvent/TileTrackMuFeatureContainer.h TrigMuonEvent/TrigMuonEFTrack.h TrigMuonEvent/TrigMuonEFCbTrack.h TrigMuonEvent/TrigMuonEFInfo.h TrigMuonEvent/TrigMuonEFInfoContainer.h TrigMuonEvent/TrigMuonEFInfoTrack.h TrigMuonEvent/TrigMuonEFInfoTrackContainer.h TrigMuonEvent/TrigMuonEFIsolation.h TrigMuonEvent/TrigMuonEFIsolationContainer.h TrigMuonEvent/MuonFeatureDetails.h TrigMuonEvent/MuonFeatureDetailsContainer.h
                          LINK_LIBRARIES TrigMuonEvent )

atlas_add_sercnv_library( TrigMuonEventxAODSerCnv
                          FILES xAODMuon/MuonContainer.h xAODMuon/MuonAuxContainer.h xAODTrigMuon/L2StandAloneMuonContainer.h xAODTrigMuon/L2StandAloneMuonAuxContainer.h xAODTrigMuon/L2CombinedMuonContainer.h xAODTrigMuon/L2CombinedMuonAuxContainer.h xAODTrigMuon/L2IsoMuonContainer.h xAODTrigMuon/L2IsoMuonAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::MuonContainer xAOD::MuonAuxContainer xAOD::L2StandAloneMuonContainer xAOD::L2StandAloneMuonAuxContainer xAOD::L2CombinedMuonContainer xAOD::L2CombinedMuonAuxContainer xAOD::L2IsoMuonContainer xAOD::L2IsoMuonAuxContainer
                          CNV_PFX xAOD
                          LINK_LIBRARIES TrigMuonEvent xAODMuon xAODTrigMuon )

atlas_add_dictionary( TrigMuonEvent_cDict
                      TrigMuonEvent/TrigMuonEvent_cDict.h
                      TrigMuonEvent/selection_c.xml
                      LINK_LIBRARIES TrigMuonEvent )

atlas_add_dictionary( TrigMuonEventDict
                      TrigMuonEvent/TrigMuonEventDict.h
                      TrigMuonEvent/selection.xml
                      LINK_LIBRARIES TrigMuonEvent
                      DATA_LINKS MuonFeature CombinedMuonFeature IsoMuonFeature TrigMuonClusterFeature TrigMuonEFContainer TileMuFeature TileTrackMuFeature TrigMuonEFIsolationContainer
                      ELEMENT_LINKS MuonFeatureContainer CombinedMuonFeatureContainer IsoMuonFeatureContainer TrigMuonClusterFeatureContainer TileMuFeatureContainer TileTrackMuFeatureContainer TrigMuonEFInfoContainer )

atlas_add_test( Operators_test
                SOURCES
                test/Operators_test.cxx
                LINK_LIBRARIES TrigMuonEvent
                POST_EXEC_SCRIPT nopost.sh )
