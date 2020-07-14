# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigInDetEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   PRIVATE_LINK_LIBRARIES ParticleTruth TrigNavigationLib TrkEventUtils TrkParameters TrkTruthData VxSecVertex VxVertex
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthenaKernel EventKernel FourMom GaudiKernel Identifier InDetPrepRawData Navigation Particle TrigSerializeCnvSvcLib TrkPrepRawData TrkSegment TrkSpacePoint TrkTrack )

atlas_add_sercnv_library( TrigInDetEventSerCnv
                          FILES TrigInDetEvent/TrigInDetTrack.h TrigInDetEvent/TrigInDetTrackCollection.h TrigInDetEvent/TrigInDetTrackFitPar.h TrigInDetEvent/TrigVertex.h TrigInDetEvent/TrigVertexCollection.h TrigInDetEvent/TrigTrackCounts.h TrigInDetEvent/TrigTrackCountsCollection.h TrigInDetEvent/TrigVertexCounts.h TrigInDetEvent/TrigVertexCountsCollection.h TrigInDetEvent/TrigTauTracksInfo.h TrigInDetEvent/TrigTauTracksInfoCollection.h TrigInDetEvent/TrigHisto1D.h TrigInDetEvent/TrigHisto2D.h TrigInDetEvent/TrigSpacePointCounts.h TrigInDetEvent/TrigSpacePointCountsCollection.h TrigInDetEvent/TrigTrtHitCounts.h TrigInDetEvent/TrigTrtHitCountsCollection.h TrkTrack/TrackCollection.h TrkTrack/TrackExtensionMap.h VxVertex/VxContainer.h TrkSegment/SegmentCollection.h Particle/TrackParticleContainer.h
                          TYPES_WITH_NAMESPACE Rec::TrackParticleContainer Trk::SegmentCollection
                          LINK_LIBRARIES TrigInDetEvent )

atlas_add_sercnv_library( TrigInDetEventxAODSerCnv
                          FILES xAODTracking/VertexContainer.h xAODTracking/VertexAuxContainer.h xAODTracking/TrackParticleContainer.h xAODTracking/TrackParticleAuxContainer.h xAODTrigMinBias/TrigSpacePointCounts.h xAODTrigMinBias/TrigSpacePointCountsContainer.h xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h xAODTrigMinBias/TrigHisto2D.h xAODTrigMinBias/TrigHisto2DContainer.h xAODTrigMinBias/TrigHisto2DAuxContainer.h xAODTrigMinBias/TrigVertexCounts.h xAODTrigMinBias/TrigVertexCountsContainer.h xAODTrigMinBias/TrigVertexCountsAuxContainer.h xAODTrigMinBias/TrigTrackCounts.h xAODTrigMinBias/TrigTrackCountsContainer.h xAODTrigMinBias/TrigTrackCountsAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrackParticle xAOD::TrackParticleContainer xAOD::TrackParticleAuxContainer xAOD::Vertex xAOD::VertexContainer xAOD::VertexAuxContainer xAOD::TrigSpacePointCounts xAOD::TrigSpacePointCountsContainer xAOD::TrigSpacePointCountsAuxContainer xAOD::TrigHisto2D xAOD::TrigHisto2DContainer xAOD::TrigHisto2DAuxContainer xAOD::TrigVertexCounts xAOD::TrigVertexCountsContainer xAOD::TrigVertexCountsAuxContainer xAOD::TrigTrackCounts xAOD::TrigTrackCountsContainer xAOD::TrigTrackCountsAuxContainer
                          CNV_PFX xAOD
                          LINK_LIBRARIES TrigInDetEvent xAODTracking xAODTrigMinBias )

atlas_add_dictionary( TrigInDetEvent_cDict
                      TrigInDetEvent/TrigInDetEvent_cDict.h
                      TrigInDetEvent/selection_c.xml
                      LINK_LIBRARIES TrigInDetEvent )

atlas_add_dictionary( TrigInDetEventDict
                      TrigInDetEvent/TrigInDetEventDict.h
                      TrigInDetEvent/selection.xml
                      LINK_LIBRARIES TrigInDetEvent
                      DATA_LINKS TrigTrackCounts TrigVertexCounts TrigTauTracksInfo TrigSpacePointCounts TrigTrtHitCounts TrigVertexCollection
                      ELEMENT_LINKS TrigInDetTrackCollection )

