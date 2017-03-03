// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODTrackCaloCluster/TrackCaloClusterAuxContainer.h"

TrackCaloClusterRecAlg::TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ){
    declareProperty("TrackParticleContainerName"   ,  m_trkParticleName                 = "InDetTrackParticles");
    declareProperty("OutputCollectionPostFix"      ,  m_outputPostFix                   = "");
    declareProperty("CaloClustersLocation"         ,  m_caloClusters                    = "CaloCalTopoClusters");
    declareProperty("TrackCaloClusterContainerName",  m_trackCaloClusterContainerName   = "TrackCaloClusters");
  }
  
TrackCaloClusterRecAlg::~TrackCaloClusterRecAlg() {}
  
StatusCode TrackCaloClusterRecAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
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
    
    xAOD::TrackCaloClusterContainer* tccContainer = new xAOD::TrackCaloClusterContainer();
    ATH_CHECK( evtStore()->record( tccContainer, m_trackCaloClusterContainerName ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux, m_trackCaloClusterContainerName + "Aux." ) );
    tccContainer->setStore( tccContainerAux );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName );    
    
  

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
    
    // First loop to fill cluster-to-tracks map
    for ( const auto* assocClusters : *associatedClusters ) {
        ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        ATH_MSG_VERBOSE ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    clusterToTracksMap.insert(std::make_pair(cluster, trk));
                    if(TrackTotalClusterPt.find(trk)==TrackTotalClusterPt.end()){
                        TrackTotalClusterPt.insert(std::make_pair(trk, cluster->p4()));
                    }
                    else{
                        TrackTotalClusterPt.at(trk) = TrackTotalClusterPt.at(trk) + cluster->p4();
                    }
            } // for caloClusterLinks
        } // if caloClusterLinks().size
    } // for assoc clusters
    
    // Create cluster-to-tracks weight map
    std::map <const xAOD::CaloCluster*, FourMom_t> clusterToTracksWeightMap;
    for (auto entry : clusterToTracksMap)
    {
        if(clusterToTracksWeightMap.find(entry.first)==clusterToTracksWeightMap.end()){
            clusterToTracksWeightMap.insert(std::make_pair(entry.first, entry.second->p4() * (entry.first->pt()/(TrackTotalClusterPt.at(entry.second).Pt()))));
            ATH_MSG_VERBOSE ("cluster ptr " << entry.first << " track pt " << entry.second->pt());
        }
        else{
            clusterToTracksWeightMap.at(entry.first) = clusterToTracksWeightMap.at(entry.first) + entry.second->p4()* (entry.first->pt()/(TrackTotalClusterPt.at(entry.second).Pt()));
            ATH_MSG_VERBOSE ("cluster ptr " << entry.first << " track pt " << entry.second->pt());
            ATH_MSG_VERBOSE (" clusterToTracksWeightMap.at(entry.first).Pt " << clusterToTracksWeightMap.at(entry.first).Pt());
        }
    }
    
    // Second loop to create charged xAOD::TCC
    for ( const auto* assocClusters : *associatedClusters ) {
        ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
        tccContainer->push_back(tcc);
        FourMom_t tcc_4p(0.,0.,0.,0.);
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        ATH_MSG_VERBOSE ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        FourMom_t mass_caloclusters(0.,0.,0.,0.);
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                   
                    FourMom_t temp (cluster->pt(),cluster->eta(),cluster->phi(),cluster->m());
                    mass_caloclusters += temp;
                    
                    tcc_4p += cluster->p4()*( trk->pt() / ((clusterToTracksWeightMap.at(cluster)).Pt()) );
                    ATH_MSG_VERBOSE ("cluster->pt() " << cluster->pt() << " cluster->eta() " << cluster->eta() << " cluster->phi() " << cluster->phi() << " track pt " << trk->pt() << " (clusterToTracksWeightMap.at(cluster)).Pt() " << (clusterToTracksWeightMap.at(cluster)).Pt());
            } // for caloClusterLinks
        } // if caloClusterLinks().size
        
        tcc->setP4(tcc_4p.Pt(),trk->eta(),trk->phi(),tcc_4p.M());
        tcc->setTaste(xAOD::TrackCaloCluster::Taste::Charged);
        ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() );
        ATH_MSG_VERBOSE ("Original track pt " << trk->pt() << " original calo cluster(s) pt " << mass_caloclusters.Pt() << " original calo cluster(s) mass " << mass_caloclusters.M());
    } // for assoc clusters
    
    // Create neutral xAOD::TCC
    for ( const auto* cluster : *allClusters ) {
        if(clusterToTracksWeightMap.find(cluster)==clusterToTracksWeightMap.end()){
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(cluster->pt(),cluster->eta(),cluster->phi(),cluster->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::Neutral);
        }
    } // for all clusters
    
    // Create trackonly xAOD::TCC
    for ( const auto* track : *allTracks ) {
        if(TrackTotalClusterPt.find(track)==TrackTotalClusterPt.end()){
            xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
            tccContainer->push_back(tcc);
            tcc->setP4(track->pt(),track->eta(),track->phi(),track->m());
            tcc->setTaste(xAOD::TrackCaloCluster::Taste::TrackOnly);
        }
    } // for all clusters
    
    
    
  
  return StatusCode::SUCCESS;
}
