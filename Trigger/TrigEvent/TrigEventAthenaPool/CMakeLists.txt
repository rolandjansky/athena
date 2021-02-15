# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrigParticle/TrigL2BphysContainer.h
         TrigParticle/TrigElectronContainer.h TrigParticle/TrigTauContainer.h
         TrigParticle/TrigTau.h TrigParticle/TrigL2BjetContainer.h
         TrigParticle/TrigEFBjetContainer.h TrigParticle/TrigPhotonContainer.h
         TrigParticle/TrigEFBphysContainer.h TrigDecisionEvent/TrigDecision.h
         TrigInDetEvent/TrigInDetTrackCollection.h
         TrigInDetEvent/TrigVertexCollection.h
         TrigInDetEvent/TrigTauTracksInfo.h
         TrigInDetEvent/TrigTauTracksInfoCollection.h
         TrigInDetEvent/TrigHisto1D.h TrigInDetEvent/TrigHisto2D.h
         TrigInDetEvent/TrigSpacePointCounts.h
         TrigInDetEvent/TrigSpacePointCountsCollection.h
         TrigInDetEvent/TrigTrtHitCounts.h
         TrigInDetEvent/TrigTrtHitCountsCollection.h
         TrigInDetEvent/TrigTrackCounts.h
         TrigInDetEvent/TrigTrackCountsCollection.h
         TrigInDetEvent/TrigVertexCounts.h
         TrigInDetEvent/TrigVertexCountsCollection.h
         TrigCombinedEvent/TrigComposite.h
         TrigCombinedEvent/TrigCompositeContainer.h TrigMuonEvent/MuonFeature.h
         TrigMuonEvent/MuonFeatureContainer.h TrigMuonEvent/MuonFeatureDetails.h
         TrigMuonEvent/MuonFeatureDetailsContainer.h
         TrigMuonEvent/CombinedMuonFeature.h
         TrigMuonEvent/CombinedMuonFeatureContainer.h
         TrigMuonEvent/IsoMuonFeature.h TrigMuonEvent/IsoMuonFeatureContainer.h
         TrigMuonEvent/TrigMuonClusterFeature.h
         TrigMuonEvent/TrigMuonClusterFeatureContainer.h
         TrigMuonEvent/TileMuFeature.h TrigMuonEvent/TileMuFeatureContainer.h
         TrigMuonEvent/TileTrackMuFeature.h
         TrigMuonEvent/TileTrackMuFeatureContainer.h
         TrigMuonEvent/TrigMuonEFContainer.h
         TrigMuonEvent/TrigMuonEFInfoContainer.h
         TrigMuonEvent/TrigMuonEFIsolationContainer.h
         TrigMuonEvent/TrigCombDiMuonContainer.h TrigBphysicsEvent/TrigJpsi.h
         TrigBphysicsEvent/TrigJpsiContainer.h TrigCaloEvent/TrigEMCluster.h
         TrigCaloEvent/TrigEMClusterContainer.h TrigCaloEvent/TrigTauCluster.h
         TrigCaloEvent/TrigTauClusterContainer.h
         TrigCaloEvent/TrigTauClusterDetails.h
         TrigCaloEvent/TrigTauClusterDetailsContainer.h
         TrigCaloEvent/TrigT2MbtsBits.h TrigCaloEvent/TrigT2MbtsBitsContainer.h
         TrigCaloEvent/TrigT2ZdcSignals.h
         TrigCaloEvent/TrigT2ZdcSignalsContainer.h TrigCaloEvent/TrigT2Jet.h
         TrigCaloEvent/TrigT2JetContainer.h TrigMissingEtEvent/TrigMissingET.h
         TrigMissingEtEvent/TrigMissingETContainer.h TrigCaloEvent/RingerRings.h
         TrigCaloEvent/RingerRingsContainer.h TrigCaloEvent/TrigRNNOutput.h
         TrigCaloEvent/TrigRNNOutputContainer.h TrigCaloEvent/TrigCaloCluster.h
         TrigCaloEvent/TrigCaloClusterContainer.h
         TrigTopoEvent/ElectronMuonTopoInfo.h
         TrigTopoEvent/ElectronMuonTopoInfoContainer.h
         TrigMonitoringEvent/TrigMonEvent.h
         TrigMonitoringEvent/TrigMonEventCollection.h
         TrigMonitoringEvent/TrigMonConfig.h
         TrigMonitoringEvent/TrigMonConfigCollection.h 
   TYPES_WITH_NAMESPACE TrigDec::TrigDecision
   LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib
                  RootConversions TrigBphysicsEvent TrigCaloEvent
                  TrigCaloEventTPCnv TrigCombinedEvent TrigCombinedEventTPCnv
                  TrigDecisionEvent TrigDecisionEventTPCnv TrigInDetEvent
                  TrigInDetEventTPCnv TrigMissingEtEvent TrigMissingEtEventTPCnv
                  TrigMonitoringEvent TrigMonitoringEventTPCnv TrigMuonEvent
                  TrigMuonEventTPCnv TrigParticle TrigParticleTPCnv
                  TrigTopoEvent TrigTopoEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRIGEVENTATHENAPOOL_REFERENCE_TAG
       TrigEventAthenaPoolReference-01-00-01 )
  run_tpcnv_test( TrigEventAthenaPool_15.6.1    ESD-15.6.1
                   REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigEventAthenaPool_15.6.13.2 AOD-15.6.13.2-full
                   REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigEventAthenaPool_16.0.3.3  AOD-16.0.3.3-full
                   REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigEventAthenaPool_17.0.6    AOD-17.0.6-full
                   REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigEventAthenaPool_18.0.0    AOD-18.0.0-full
                   REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigEventAthenaPool_20.1.7.2  ESD-20.1.7.2
                   REFERENCE_TAG ${TRIGEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
