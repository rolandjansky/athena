// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODTrackCaloCluster/TrackCaloCluster.h"

TrackCaloClusterRecAlg::TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ){
    declareProperty("TrackParticleContainerName"   ,  m_trkParticleName = "InDetTrackParticles");
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

    // Look at associated clusters
    if ( !evtStore()->contains<xAOD::TrackParticleClusterAssociationContainer>( m_trkParticleName+"ClusterAssociations") ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleClusterAssociations with key: " << m_trkParticleName+"ClusterAssociations" );
                return StatusCode::SUCCESS;
        }
    
    const auto associatedClusters = getContainer<xAOD::TrackParticleClusterAssociationContainer>(m_trkParticleName+"ClusterAssociations");
    
    // First loop to fill cluster-to-tracks map
    for ( const auto* assocClusters : *associatedClusters ) {
        ATH_MSG_DEBUG ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        ATH_MSG_DEBUG ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    clusterToTracksMap.insert(std::make_pair(cluster, trk));
            } // for caloClusterLinks
        } // if caloClusterLinks().size
    } // for assoc clusters
    
    // Create cluster-to-tracks weight map
    std::map <const xAOD::CaloCluster*, FourMom_t> clusterToTracksWeightMap;
    for (auto entry : clusterToTracksMap)
    {
        if(clusterToTracksWeightMap.find(entry.first)==clusterToTracksWeightMap.end()){
            clusterToTracksWeightMap.insert(std::make_pair(entry.first, entry.second->p4()));
        }
        else{
            clusterToTracksWeightMap.at(entry.first) = clusterToTracksWeightMap.at(entry.first) + entry.second->p4();
        }
    }
    
    // Second loop to create charged xAOD::TCC
    for ( const auto* assocClusters : *associatedClusters ) {
        ATH_MSG_DEBUG ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
        FourMom_t tcc_4p(0.,0.,0.,0.);
        if(!tcc){
            ATH_MSG_WARNING( "WARNING: Problem creating tcc");
        }
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        ATH_MSG_DEBUG ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
        double pt_caloclusters = 0.;
        FourMom_t mass_caloclusters(0.,0.,0.,0.);
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    pt_caloclusters += cluster->pt();
                    FourMom_t temp (cluster->pt(),cluster->eta(),cluster->phi(),cluster->m());
                    mass_caloclusters += temp;
                    
                    tcc_4p += cluster->p4()*(trk->pt()/(clusterToTracksWeightMap.at(cluster)).Pt());
            } // for caloClusterLinks
        } // if caloClusterLinks().size
        tcc->setP4((float) (tcc_4p.Pt()/trk->pt()), (float)trk->eta(),(float)trk->phi(),(float)(tcc_4p.M()/trk->pt()));
        ATH_MSG_DEBUG ("Created TCC with pt " << tcc_4p.Pt()/trk->pt() << " eta " << trk->eta() << " phi " << trk->phi() << " mass " << tcc_4p.M()/trk->pt() );
        ATH_MSG_DEBUG ("Original track pt " << trk->pt() << " original calo cluster(s) pt " << pt_caloclusters << " original calo cluster(s) mass " << mass_caloclusters.M());
    } // for assoc clusters
  
  return StatusCode::SUCCESS;
}
