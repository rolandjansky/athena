################################################################################
# Package: TrigInDetEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/Navigation
                          DetectorDescription/Identifier
                          Event/EventKernel
                          Event/FourMom
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMinBias
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigInDetEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers Navigation AthenaKernel Identifier EventKernel FourMom GaudiKernel InDetPrepRawData TrkPrepRawData TrkSpacePoint TrkTrack TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking xAODTrigMinBias Particle ParticleTruth TrkParameters TrkSegment TrkTruthData VxSecVertex VxVertex )

atlas_add_sercnv_library( TrigInDetEventSerCnv
                          FILES TrigInDetEvent/TrigInDetTrack.h TrigInDetEvent/TrigInDetTrackCollection.h TrigInDetEvent/TrigInDetTrackFitPar.h TrigInDetEvent/TrigVertex.h TrigInDetEvent/TrigVertexCollection.h TrigInDetEvent/TrigTrackCounts.h TrigInDetEvent/TrigTrackCountsCollection.h TrigInDetEvent/TrigVertexCounts.h TrigInDetEvent/TrigVertexCountsCollection.h TrigInDetEvent/TrigTauTracksInfo.h TrigInDetEvent/TrigTauTracksInfoCollection.h TrigInDetEvent/TrigHisto1D.h TrigInDetEvent/TrigHisto2D.h TrigInDetEvent/TrigSpacePointCounts.h TrigInDetEvent/TrigSpacePointCountsCollection.h TrigInDetEvent/TrigTrtHitCounts.h TrigInDetEvent/TrigTrtHitCountsCollection.h TrkTrack/TrackCollection.h TrkTrack/TrackExtensionMap.h VxVertex/VxContainer.h TrkSegment/SegmentCollection.h Particle/TrackParticleContainer.h
                          TYPES_WITH_NAMESPACE Rec::TrackParticleContainer Trk::SegmentCollection
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation AthenaKernel Identifier EventKernel FourMom GaudiKernel InDetPrepRawData TrkPrepRawData TrkSpacePoint TrkTrack xAODTracking xAODTrigMinBias Particle ParticleTruth TrkParameters TrkSegment TrkTruthData VxSecVertex VxVertex TrigSerializeCnvSvcLib TrigNavigationLib TrigInDetEvent )

atlas_add_sercnv_library( TrigInDetEventxAODSerCnv
                          FILES xAODTracking/VertexContainer.h xAODTracking/VertexAuxContainer.h xAODTracking/TrackParticleContainer.h xAODTracking/TrackParticleAuxContainer.h xAODTrigMinBias/TrigSpacePointCounts.h xAODTrigMinBias/TrigSpacePointCountsContainer.h xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h xAODTrigMinBias/TrigHisto2D.h xAODTrigMinBias/TrigHisto2DContainer.h xAODTrigMinBias/TrigHisto2DAuxContainer.h xAODTrigMinBias/TrigVertexCounts.h xAODTrigMinBias/TrigVertexCountsContainer.h xAODTrigMinBias/TrigVertexCountsAuxContainer.h xAODTrigMinBias/TrigTrackCounts.h xAODTrigMinBias/TrigTrackCountsContainer.h xAODTrigMinBias/TrigTrackCountsAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrackParticle xAOD::TrackParticleContainer xAOD::TrackParticleAuxContainer xAOD::Vertex xAOD::VertexContainer xAOD::VertexAuxContainer xAOD::TrigSpacePointCounts xAOD::TrigSpacePointCountsContainer xAOD::TrigSpacePointCountsAuxContainer xAOD::TrigHisto2D xAOD::TrigHisto2DContainer xAOD::TrigHisto2DAuxContainer xAOD::TrigVertexCounts xAOD::TrigVertexCountsContainer xAOD::TrigVertexCountsAuxContainer xAOD::TrigTrackCounts xAOD::TrigTrackCountsContainer xAOD::TrigTrackCountsAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation AthenaKernel Identifier EventKernel FourMom GaudiKernel InDetPrepRawData TrkPrepRawData TrkSpacePoint TrkTrack xAODTracking xAODTrigMinBias Particle ParticleTruth TrkParameters TrkSegment TrkTruthData VxSecVertex VxVertex TrigSerializeCnvSvcLib TrigNavigationLib TrigInDetEvent )

atlas_add_dictionary( TrigInDetEvent_cDict
                      TrigInDetEvent/TrigInDetEvent_cDict.h
                      TrigInDetEvent/selection_c.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation AthenaKernel Identifier EventKernel FourMom GaudiKernel InDetPrepRawData TrkPrepRawData TrkSpacePoint TrkTrack xAODTracking xAODTrigMinBias Particle ParticleTruth TrkParameters TrkSegment TrkTruthData VxSecVertex VxVertex TrigSerializeCnvSvcLib TrigNavigationLib TrigInDetEvent )

atlas_add_dictionary( TrigInDetEventDict
                      TrigInDetEvent/TrigInDetEventDict.h
                      TrigInDetEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation AthenaKernel Identifier EventKernel FourMom GaudiKernel InDetPrepRawData TrkPrepRawData TrkSpacePoint TrkTrack xAODTracking xAODTrigMinBias Particle ParticleTruth TrkParameters TrkSegment TrkTruthData VxSecVertex VxVertex TrigSerializeCnvSvcLib TrigNavigationLib TrigInDetEvent
                      DATA_LINKS TrigTrackCounts TrigVertexCounts TrigTauTracksInfo TrigSpacePointCounts TrigTrtHitCounts TrigVertexCollection
                      ELEMENT_LINKS TrigInDetTrackCollection )

