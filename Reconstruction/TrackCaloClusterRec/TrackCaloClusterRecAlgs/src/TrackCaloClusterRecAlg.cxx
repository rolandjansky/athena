// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODTrackCaloCluster/TrackCaloClusterAuxContainer.h"

// #include "xAODJet/JetContainer.h"
// #include "xAODJet/JetAuxContainer.h"

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
    
    // Charged
    xAOD::TrackCaloClusterContainer* tccContainer = new xAOD::TrackCaloClusterContainer();
    ATH_CHECK( evtStore()->record( tccContainer, m_trackCaloClusterContainerName + "Charged" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux, m_trackCaloClusterContainerName + "Charged" + "Aux." ) );
    tccContainer->setStore( tccContainerAux );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "Charged" );    

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
    
    // Create charged xAOD::TCC
    m_trackCaloClusterCreatorTool->createChargedTCCs(tccContainer, associatedClusters, &TrackTotalClusterPt, &clusterToTracksWeightMap);
    
    // Charged + Neutral
    //xAOD::TrackCaloClusterContainer* tccContainer1 = new xAOD::TrackCaloClusterContainer(*tccContainer);
    xAOD::TrackCaloClusterContainer* tccContainer1 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer1, m_trackCaloClusterContainerName ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux1 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux1, m_trackCaloClusterContainerName + "Aux." ) );
    tccContainer1->setStore( tccContainerAux1 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName ); 
    
    for (xAOD::TrackCaloCluster* charged: *tccContainer) {
      tccContainer1->push_back(new xAOD::TrackCaloCluster);
      *tccContainer1->back() = *charged;
    }
    
    // Create neutral xAOD::TCC
    m_trackCaloClusterCreatorTool->createNeutralTCCs(tccContainer1, allClusters, &clusterToTracksWeightMap);
    
    // Charged + Neutral + TrackOnly
    //xAOD::TrackCaloClusterContainer* tccContainer2 = new xAOD::TrackCaloClusterContainer(*tccContainer1);
    xAOD::TrackCaloClusterContainer* tccContainer2 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer2, m_trackCaloClusterContainerName + "All" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux2 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux2, m_trackCaloClusterContainerName + "All" + "Aux." ) );
    tccContainer2->setStore( tccContainerAux2 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "All" ); 
    
    for (xAOD::TrackCaloCluster* chargedAndNeutral: *tccContainer1) {
      tccContainer2->push_back(new xAOD::TrackCaloCluster);
      *tccContainer2->back() = *chargedAndNeutral;
    }
    
    // Create trackonly xAOD::TCC
    m_trackCaloClusterCreatorTool->createTrackOnlyTCCs(tccContainer2, allTracks, &TrackTotalClusterPt);
    
    // Charged + TrackOnly
    xAOD::TrackCaloClusterContainer* tccContainer3 = new xAOD::TrackCaloClusterContainer;
    ATH_CHECK( evtStore()->record( tccContainer3, m_trackCaloClusterContainerName + "AllTrack" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux3 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux3, m_trackCaloClusterContainerName + "AllTrack" + "Aux." ) );
    tccContainer3->setStore( tccContainerAux3 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "AllTrack" ); 
    
    for (xAOD::TrackCaloCluster* chargedAndNeutral: *tccContainer) {
      tccContainer3->push_back(new xAOD::TrackCaloCluster);
      *tccContainer3->back() = *chargedAndNeutral;
    }
    
    // Create trackonly xAOD::TCC
    m_trackCaloClusterCreatorTool->createTrackOnlyTCCs(tccContainer3, allTracks, &TrackTotalClusterPt);
    
  return StatusCode::SUCCESS;
}
