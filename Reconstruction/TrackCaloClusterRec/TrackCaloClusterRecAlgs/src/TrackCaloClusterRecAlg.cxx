/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"

TrackCaloClusterRecAlg::TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_trackCaloClusterWeightsTool("TrackCaloClusterRecTools/TrackCaloClusterWeightsTool"),
  m_trackCaloClusterCreatorTool("TrackCaloClusterRecTools/TrackCaloClusterCreatorTool") {
      
    declareProperty("TrackParticleContainerName"   ,    m_trkParticleName                 = "InDetTrackParticles"   );
    declareProperty("OutputCollectionPostFix"      ,    m_outputPostFix                   = ""                      );
    declareProperty("CaloClustersLocation"         ,    m_caloClusters                    = "CaloCalTopoClusters"   );
    declareProperty("TrackCaloClusterContainerName",    m_trackCaloClusterContainerName   = "TrackCaloClusters"     );
    declareProperty("TrackCaloClusterWeightsTool"  ,    m_trackCaloClusterWeightsTool                               );
    declareProperty("TrackCaloClusterCreatorTool"  ,    m_trackCaloClusterCreatorTool                               );
  }
  
TrackCaloClusterRecAlg::~TrackCaloClusterRecAlg() {}
  
StatusCode TrackCaloClusterRecAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(m_trackCaloClusterWeightsTool.retrieve());
  ATH_CHECK(m_trackCaloClusterCreatorTool.retrieve());
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::execute() {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");
    // Create cluster-to-tracks map
    std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*> clusterToTracksMap;
    std::map <const xAOD::TrackParticle*, FourMom_t> TrackTotalClusterPt;
    std::map <const xAOD::CaloCluster*, FourMom_t> clusterToTracksWeightMap;
    
    // Combined
    xAOD::TrackCaloClusterContainer* tccContainer = new xAOD::TrackCaloClusterContainer();
    ATH_CHECK( evtStore()->record( tccContainer, m_trackCaloClusterContainerName + "Combined" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux, m_trackCaloClusterContainerName + "Combined" + "Aux." ) );
    tccContainer->setStore( tccContainerAux );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "Combined" );    

    // Look at associated clusters
    if ( !evtStore()->contains<xAOD::TrackParticleClusterAssociationContainer>( m_trkParticleName+"ClusterAssociations"+m_outputPostFix) ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleClusterAssociations with key: " << m_trkParticleName+"ClusterAssociations"+m_outputPostFix );
                return StatusCode::SUCCESS;
        }
    
    const auto associatedClusters = getContainer<xAOD::TrackParticleClusterAssociationContainer>(m_trkParticleName+"ClusterAssociations"+m_outputPostFix);
    
    // Look at all clusters
    if ( !evtStore()->contains<xAOD::CaloClusterContainer>( m_caloClusters) ) {
                ATH_MSG_WARNING ("Couldn't retrieve CaloClusterContainer with key: " << m_caloClusters );
                return StatusCode::SUCCESS;
        }
    
    const auto allClusters = getContainer<xAOD::CaloClusterContainer>(m_caloClusters);
    
    // Look at all tracks
    if ( !evtStore()->contains<xAOD::TrackParticleContainer>( m_trkParticleName) ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleContainer with key: " << m_trkParticleName );
                return StatusCode::SUCCESS;
        }
    
    const auto allTracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    
    // Create the weights
    m_trackCaloClusterWeightsTool->fillWeightMaps(associatedClusters, &clusterToTracksMap, &TrackTotalClusterPt, &clusterToTracksWeightMap);
    
    // Create Combined xAOD::TCC
    m_trackCaloClusterCreatorTool->createCombinedTCCs(tccContainer, associatedClusters, &TrackTotalClusterPt, &clusterToTracksWeightMap);
    
    // Combined + Neutral
    xAOD::TrackCaloClusterContainer* tccContainer1 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer1, m_trackCaloClusterContainerName + "CombinedAndNeutral" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux1 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux1, m_trackCaloClusterContainerName + "CombinedAndNeutral" + "Aux." ) );
    tccContainer1->setStore( tccContainerAux1 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName ); 
    
    for (xAOD::TrackCaloCluster* combined: *tccContainer) {
      tccContainer1->push_back(new xAOD::TrackCaloCluster);
      *tccContainer1->back() = *combined;
    }
    
    // Create neutral xAOD::TCC
    m_trackCaloClusterCreatorTool->createNeutralTCCs(tccContainer1, allClusters, &clusterToTracksWeightMap);
    
    // Combined + Neutral + Charged
    xAOD::TrackCaloClusterContainer* tccContainer2 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer2, m_trackCaloClusterContainerName + "All" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux2 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux2, m_trackCaloClusterContainerName + "All" + "Aux." ) );
    tccContainer2->setStore( tccContainerAux2 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "All" ); 
    
    for (xAOD::TrackCaloCluster* combinedAndNeutral: *tccContainer1) {
      tccContainer2->push_back(new xAOD::TrackCaloCluster);
      *tccContainer2->back() = *combinedAndNeutral;
    }
    
    // Create trackonly xAOD::TCC
    m_trackCaloClusterCreatorTool->createChargedTCCs(tccContainer2, allTracks, &TrackTotalClusterPt);
    
    // Combined + Charged
    xAOD::TrackCaloClusterContainer* tccContainer3 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer3, m_trackCaloClusterContainerName + "CombinedAndCharged" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux3 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux3, m_trackCaloClusterContainerName + "CombinedAndCharged" + "Aux." ) );
    tccContainer3->setStore( tccContainerAux3 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "AllTrack" ); 
    
    for (xAOD::TrackCaloCluster* combined: *tccContainer) {
      tccContainer3->push_back(new xAOD::TrackCaloCluster);
      *tccContainer3->back() = *combined;
    }
    
    // Create trackonly xAOD::TCC
    m_trackCaloClusterCreatorTool->createChargedTCCs(tccContainer3, allTracks, &TrackTotalClusterPt);
    
  return StatusCode::SUCCESS;
}
