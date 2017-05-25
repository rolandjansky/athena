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
  m_trackCaloClusterCreatorTool("TrackCaloClusterRecTools/TrackCaloClusterCreatorTool"),
  m_storeStats(false) {
      
    declareProperty("TrackParticleContainerName"   ,    m_trkParticleName                 = "InDetTrackParticles"   );
    declareProperty("OutputCollectionPostFix"      ,    m_outputPostFix                   = ""                      );
    declareProperty("CaloClustersLocation"         ,    m_caloClusters                    = "CaloCalTopoClusters"   );
    declareProperty("TrackCaloClusterContainerName",    m_trackCaloClusterContainerName   = "TrackCaloClusters"     );
    declareProperty("TrackCaloClusterWeightsTool"  ,    m_trackCaloClusterWeightsTool                               );
    declareProperty("TrackCaloClusterCreatorTool"  ,    m_trackCaloClusterCreatorTool                               );
    declareProperty("StoreStatistics"              ,    m_storeStats                                                );
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
    
    if (m_storeStats) storeStats(associatedClusters, allClusters, allTracks);
    
  return StatusCode::SUCCESS;
}

void TrackCaloClusterRecAlg::storeStats(const xAOD::TrackParticleClusterAssociationContainer* associatedClusters,
                                        const xAOD::CaloClusterContainer* allClusters,
					const xAOD::TrackParticleContainer* /*allTracks*/) {
  
//   std::map < const xAOD::TrackParticle*, int> NumClusters_toTracks;
  std::map < const xAOD::CaloCluster*, std::vector < ElementLink < xAOD::TrackParticleContainer > > > NumTracks_toClusters;
  
  // First loop to fill cluster-to-tracks map
  for ( const auto* assocClusters : *associatedClusters ) {
    // flollow the link to the track particle
    
//     const xAOD::TrackParticle* trk = 0;
//     if (assocClusters->trackParticleLink().isValid())
//       trk = *(assocClusters->trackParticleLink());
//     else if ( !assocClusters->trackParticleLink().isValid() )
//       ATH_MSG_ERROR ("trackParticleLink is not valid! " );
      
    // filling the number of clusters matched to a track
//     NumClusters_toTracks.insert(std::make_pair(trk, assocClusters->caloClusterLinks().size()));
    
    // follow the link to the calorimeter clusters
    if (assocClusters->caloClusterLinks().size()) {
      for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
	const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
	if (NumTracks_toClusters.find(cluster) == NumTracks_toClusters.end()) 
	  NumTracks_toClusters.insert(std::make_pair(cluster, std::vector<ElementLink<xAOD::TrackParticleContainer>>()));
	NumTracks_toClusters.at(cluster).push_back(assocClusters->trackParticleLink());
// 	  NumTracks_toClusters.insert(std::make_pair(cluster, std::make_pair(0, std::vector<ElementLink<xAOD::TrackParticleContainer>>())));
// 	NumTracks_toClusters.at(cluster).first++;
// 	NumTracks_toClusters.at(cluster).second.push_back(assocClusters->trackParticleLink());
      }
    }
  }
  
  
  for ( const auto* cluster : *allClusters ) {
    if (NumTracks_toClusters.find(cluster) == NumTracks_toClusters.end())
      NumTracks_toClusters.insert(std::make_pair(cluster, std::vector<ElementLink<xAOD::TrackParticleContainer>>()));
    cluster->auxdecor<std::vector<ElementLink<xAOD::TrackParticleContainer>>>("MatchingTracks") = NumTracks_toClusters.at(cluster);
//       NumTracks_toClusters.insert(std::make_pair(cluster, std::make_pair(0, std::vector<ElementLink<xAOD::TrackParticleContainer>>())));
//     cluster->auxdecor<int>("NumberMatchingTracks") = NumTracks_toClusters.at(cluster).first;    
//     cluster->auxdecor<std::vector<ElementLink<xAOD::TrackParticleContainer>>>("MatchingTracks") = NumTracks_toClusters.at(cluster).second;    
  }
  
  /*for ( const auto* track : *allTracks ) {
    if (NumClusters_toTracks.find(track) == NumClusters_toTracks.end())
      NumClusters_toTracks.insert(std::make_pair(track, 0));
    track->auxdecor<int>("NumberMatchingClusters") = NumClusters_toTracks.at(track);    
  }*/ 
  
}




















