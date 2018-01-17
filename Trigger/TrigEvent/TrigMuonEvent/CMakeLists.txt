################################################################################
# Package: TrigMuonEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Control/Navigation
                          Event/EventKernel
                          Event/FourMom
                          GaudiKernel
                          Reconstruction/Particle
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigMuon
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Reconstruction/MuonIdentification/MuidEvent
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigMuonEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigMuonEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent )

atlas_add_sercnv_library( TrigMuonEventSerCnv
                          FILES TrigMuonEvent/MuonFeature.h TrigMuonEvent/MuonFeatureContainer.h TrigMuonEvent/CombinedMuonFeature.h TrigMuonEvent/CombinedMuonFeatureContainer.h TrigMuonEvent/IsoMuonFeature.h TrigMuonEvent/IsoMuonFeatureContainer.h TrigMuonEvent/TrigMuonClusterFeature.h TrigMuonEvent/TrigMuonClusterFeatureContainer.h TrigMuonEvent/TrigMuonEF.h TrigMuonEvent/TrigMuonEFContainer.h TrigMuonEvent/TileMuFeature.h TrigMuonEvent/TileMuFeatureContainer.h TrigMuonEvent/TileTrackMuFeature.h TrigMuonEvent/TileTrackMuFeatureContainer.h TrigMuonEvent/TrigMuonEFTrack.h TrigMuonEvent/TrigMuonEFCbTrack.h TrigMuonEvent/TrigMuonEFInfo.h TrigMuonEvent/TrigMuonEFInfoContainer.h TrigMuonEvent/TrigMuonEFInfoTrack.h TrigMuonEvent/TrigMuonEFInfoTrackContainer.h TrigMuonEvent/TrigMuonEFIsolation.h TrigMuonEvent/TrigMuonEFIsolationContainer.h TrigMuonEvent/MuonFeatureDetails.h TrigMuonEvent/MuonFeatureDetailsContainer.h
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigMuonEvent )

atlas_add_sercnv_library( TrigMuonEventxAODSerCnv
                          FILES xAODMuon/MuonContainer.h xAODMuon/MuonAuxContainer.h xAODTrigMuon/L2StandAloneMuonContainer.h xAODTrigMuon/L2StandAloneMuonAuxContainer.h xAODTrigMuon/L2CombinedMuonContainer.h xAODTrigMuon/L2CombinedMuonAuxContainer.h xAODTrigMuon/L2IsoMuonContainer.h xAODTrigMuon/L2IsoMuonAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::MuonContainer xAOD::MuonAuxContainer xAOD::L2StandAloneMuonContainer xAOD::L2StandAloneMuonAuxContainer xAOD::L2CombinedMuonContainer xAOD::L2CombinedMuonAuxContainer xAOD::L2IsoMuonContainer xAOD::L2IsoMuonAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigMuonEvent )

atlas_add_dictionary( TrigMuonEvent_cDict
                      TrigMuonEvent/TrigMuonEvent_cDict.h
                      TrigMuonEvent/selection_c.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigMuonEvent )

atlas_add_dictionary( TrigMuonEventDict
                      TrigMuonEvent/TrigMuonEventDict.h
                      TrigMuonEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigMuonEvent
                      DATA_LINKS MuonFeature CombinedMuonFeature IsoMuonFeature TrigMuonClusterFeature TrigMuonEFContainer TileMuFeature TileTrackMuFeature TrigMuonEFIsolationContainer
                      ELEMENT_LINKS MuonFeatureContainer CombinedMuonFeatureContainer IsoMuonFeatureContainer TrigMuonClusterFeatureContainer TileMuFeatureContainer TileTrackMuFeatureContainer TrigMuonEFInfoContainer )

atlas_add_test( Operators_test
                SOURCES
                test/Operators_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation EventKernel FourMom GaudiKernel Particle TrigInDetEvent TestTools AthenaKernel xAODMuon xAODTrigMuon MuonPattern MuonSegment MuidEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigMuonEvent
                EXTRA_PATTERNS ".*" )

