/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloClusterRecTools/TrackCaloClusterWeightsTool.h"


TrackCaloClusterWeightsTool::TrackCaloClusterWeightsTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
    m_useEnergy(false)
{
    declareProperty("UseEnergy",        m_useEnergy );
}

TrackCaloClusterWeightsTool::~TrackCaloClusterWeightsTool() {}

StatusCode TrackCaloClusterWeightsTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterWeightsTool::finalize() {
  return StatusCode::SUCCESS;
}

void TrackCaloClusterWeightsTool::fillWeightMaps( const xAOD::TrackParticleClusterAssociationContainer* assocContainer, 
						  std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*>* clusterToTracksMap, 
						  std::map <const xAOD::TrackParticle*, FourMom_t>* TrackTotalClusterPt, 
						  std::map <const xAOD::CaloCluster*, FourMom_t>* clusterToTracksWeightMap ) {

    // First loop to fill cluster-to-tracks map
    for ( const auto* assocClusters : *assocContainer ) {
        // flollow the link to the track particle
        const xAOD::TrackParticle* trk = 0;
        if (assocClusters->trackParticleLink().isValid()) {
            trk = *(assocClusters->trackParticleLink());
        }
        else if ( !assocClusters->trackParticleLink().isValid() ){
            ATH_MSG_ERROR ("trackParticleLink is not valid! " );
        }
        // follow the link to the calorimeter clusters
        if (assocClusters->caloClusterLinks().size()) {
            for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
                    const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
                    clusterToTracksMap->insert(std::make_pair(cluster, trk));
                    if(TrackTotalClusterPt->find(trk)==TrackTotalClusterPt->end()){
                        TrackTotalClusterPt->insert(std::make_pair(trk, cluster->p4()));
                    }
                    else{
                        TrackTotalClusterPt->at(trk) = TrackTotalClusterPt->at(trk) + cluster->p4();
                    }
            } // for caloClusterLinks
        } // if caloClusterLinks().size
    } // for assoc clusters
    
    // Create cluster-to-tracks weight map
    for (auto entry : *clusterToTracksMap)
    {
        double cluster_pt       = m_useEnergy ? entry.first->e() : entry.first->pt();
        double totalcluster_pt  = m_useEnergy ? TrackTotalClusterPt->at(entry.second).E() : TrackTotalClusterPt->at(entry.second).Pt();
        if(clusterToTracksWeightMap->find(entry.first)==clusterToTracksWeightMap->end()){
            clusterToTracksWeightMap->insert(std::make_pair(entry.first, entry.second->p4() * (cluster_pt/totalcluster_pt)));
        }
        else{
            clusterToTracksWeightMap->at(entry.first) = clusterToTracksWeightMap->at(entry.first) + entry.second->p4() * (cluster_pt/totalcluster_pt);
        }
    }
    
}

