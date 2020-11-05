/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TrackCaloClusterInfoAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterInfoAlg.h"

#include "StoreGate/ReadDecorHandle.h"

#include "TrackCaloClusterRecTools/TCCHelpers.h"

TrackCaloClusterInfoAlg::TrackCaloClusterInfoAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ) {}
  
  
StatusCode TrackCaloClusterInfoAlg::initialize() {
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  ATH_CHECK(m_inputClustersHandle.initialize() );
  ATH_CHECK(m_inputTracksHandle.initialize() );
  ATH_CHECK(m_inputVertexHandle.initialize());
  ATH_CHECK(m_tccInfoHandle.initialize());

  ATH_CHECK(m_assoClustersKey.initialize());

  return StatusCode::SUCCESS;
}
  

StatusCode TrackCaloClusterInfoAlg::execute()  {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Prepare a TrackCaloClusterInfo map : 
  SG::WriteHandle<TrackCaloClusterInfo> tccInfo(m_tccInfoHandle);    
  ATH_CHECK( tccInfo.record( std::make_unique<TrackCaloClusterInfo>() ) );

  // Retrieve the input containers 
  SG::ReadHandle<xAOD::CaloClusterContainer> clusterContainer(m_inputClustersHandle);
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_inputTracksHandle);
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_inputVertexHandle);

  tccInfo->allClusters = &*clusterContainer;
  tccInfo->allTracks = &*tracks;

  // retrieve PV0 if it exists
  if(!vertices->empty()) {
    tccInfo->pv0=(*vertices)[0]; // Hard code HS vertex as PV0 : WARNING we expect it is the same as was used for m_inputTrackCaloAssocName !!
  } else {    
    ATH_MSG_ERROR ("Vertex container " << m_inputVertexHandle << " is empty! Can't perform TVA!");
    return StatusCode::FAILURE;
  }

  // Fill the maps 
  ATH_CHECK( fillInfo(tccInfo) );  
  
  return StatusCode::SUCCESS;
}


StatusCode TrackCaloClusterInfoAlg::fillInfo(SG::WriteHandle<TrackCaloClusterInfo> & tccInfo) const {

  std::multimap <const xAOD::IParticle*, const xAOD::TrackParticle*> clusterToTracksMap;

  SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::CaloClusterContainer>> > clusterLinks(m_assoClustersKey);
  static const xAOD::IParticle::FourMom_t nullV(0,0,0,0);
  
  for(const xAOD::TrackParticle* trk : *tccInfo->allTracks){
    const auto & clustLinks = clusterLinks( *trk );
    if(clustLinks.empty() ) continue;
    // follow the link to the calorimeter clusters
    for( const auto& clLink : clustLinks) {
      const xAOD::IParticle* cluster = *clLink;
      clusterToTracksMap.insert(std::make_pair(cluster, trk));
      // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
      xAOD::IParticle::FourMom_t & totalP4 = ( tccInfo->trackTotalClusterPt.insert( {trk, nullV} ) ).first->second;
      totalP4 += cluster->p4(); // add the cluster p4 into the map.      
    }
  }
  

  // Create cluster-to-tracks weight map
  for (std::pair<const xAOD::IParticle*, const xAOD::TrackParticle*>  entry : clusterToTracksMap)
    {
      const xAOD::IParticle* cluster = entry.first;
      double cluster_pt       = m_useEnergy ? cluster->e() : cluster->pt();
      double totalcluster_pt  = m_useEnergy ? tccInfo->trackTotalClusterPt.at(entry.second).E() : tccInfo->trackTotalClusterPt.at(entry.second).Pt();

      // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
      xAOD::IParticle::FourMom_t & totalP4 = ( tccInfo->clusterToTracksWeightMap.insert( {cluster, nullV} ) ).first->second;
      totalP4 += entry.second->p4() * (cluster_pt/totalcluster_pt); // add the track p4 into the map.
    }

  
  return StatusCode::SUCCESS;  
}







namespace TCCHelpers{

  ///////////////////////////////////////////////////////
  /// Implement a concrete CombinedUFOLoop dedicated to collection of energy sharing weights for UFO
  ///  see TCCHelpers.h in TrackCaloClusterRecTools/
  struct WeightsCollector : public CombinedUFOLoop {

    // additional members 
    std::multimap <const xAOD::IParticle*, const xAOD::TrackParticle*> pfoToTracksMap;
    TrackCaloClusterInfo * tccInfo_nonconst;

    const xAOD::IParticle::FourMom_t nullV = {0,0,0,0};
    
    virtual void processPFO(const xAOD::TrackParticle* trk, const xAOD::PFO* pfo) {
      /// collect pfo momentum and register the pair (trk,pfo) in the relevant tccInfo map 
      pfoToTracksMap.insert(std::make_pair(pfo, trk));
      // get the total p4 for this track, creating it from nullV  if not existing.
      xAOD::IParticle::FourMom_t & totalP4 = ( tccInfo_nonconst->trackTotalClusterPt.insert( {trk, nullV} ) ).first->second;
      totalP4 += pfo->p4(); // this modifies the totalP4 stored inside the map (reference access)
    }
    
    virtual void processTrk(const xAOD::TrackParticle* ) { }
        
  };
}



TrackCaloClusterInfoUFOAlg::TrackCaloClusterInfoUFOAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  TrackCaloClusterInfoAlg( name, pSvcLocator ) {}
  
  
StatusCode TrackCaloClusterInfoUFOAlg::initialize() {
  ATH_CHECK(m_trackVertexAssoTool.retrieve() );
  ATH_CHECK(m_inputPFOHandle.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterInfoUFOAlg::fillInfo(SG::WriteHandle<TrackCaloClusterInfo> & tccInfo) const {



  SG::ReadHandle<xAOD::PFOContainer> pfos(m_inputPFOHandle);

  // We use a dedicated helper to collect the weights mapping tracks to PFO.  
  TCCHelpers::WeightsCollector wcoll;
  wcoll.m_orig_pfoK = m_orig_pfo.key();
  wcoll.m_clustersLinkK = m_assoClustersKey.key();
  wcoll.m_trackVertexAssoTool = m_trackVertexAssoTool.get();
  wcoll.m_clusterEcut = m_clusterEcut;
  wcoll.m_useEnergy = m_useEnergy;
  wcoll.tccInfo_nonconst = tccInfo.ptr();

  // perform the weight collection : this populates tccInfo.trackTotalClusterPt and tccInfo.pfoToTracksMap maps
  wcoll.combinedUFOLoop(tccInfo.cptr(), pfos.cptr());
  

  static const xAOD::IParticle::FourMom_t nullV(0,0,0,0);
  
  // Create cluster-to-tracks weight map
  for (std::pair<const xAOD::IParticle*, const xAOD::TrackParticle*>  entry : wcoll.pfoToTracksMap){
    double cluster_pt       = m_useEnergy ? entry.first->e() : entry.first->pt();
    double totalcluster_pt  = m_useEnergy ? tccInfo->trackTotalClusterPt.at(entry.second).E() : tccInfo->trackTotalClusterPt.at(entry.second).Pt();
    
    // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
    xAOD::IParticle::FourMom_t & totalP4 = ( tccInfo->clusterToTracksWeightMap.insert( {entry.first, nullV} ) ).first->second;
    totalP4 += entry.second->p4() * (cluster_pt/totalcluster_pt); // add the track p4 into the map.
  }

  return StatusCode::SUCCESS;
  
}
