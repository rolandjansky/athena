/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    
      virtual void processPFO(const xAOD::TrackParticle* trk, const xAOD::FlowElement* pfo) {
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
  ATH_MSG_DEBUG("TCC_INFO_UFO_ALG_INIT");
  //ATH_MSG_INFO(m_trackVertexAssoTool.get()->name());
  ATH_CHECK(m_orig_pfo.initialize());
  // ATH_CHECK(m_clusterECut.initialize());
  ATH_MSG_DEBUG("END INIT BLOCK");
  ATH_MSG_DEBUG("m_orig_pfo "+m_orig_pfo.key());
  ATH_MSG_DEBUG("m_inputPFOHandle"+m_inputPFOHandle.key());
  ATH_MSG_DEBUG("m_trackVertexAssoTool "+m_trackVertexAssoTool.name());
  ATH_MSG_DEBUG(m_clusterEcut);
  ATH_MSG_DEBUG("DEBUG OF VarHandleInits");
  ATH_MSG_DEBUG("TCC DIAGNOSTIC");
  
  ATH_MSG_DEBUG("inputTracksHandle "+m_inputTracksHandle.key());
  ATH_MSG_DEBUG("assoClustersKey "+m_assoClustersKey.key());
  ATH_MSG_DEBUG("inputClustersHandle "+m_inputClustersHandle.key());
  ATH_MSG_DEBUG("inputVertexHandle "+m_inputVertexHandle.key());
  ATH_MSG_DEBUG("useEnergy? "+m_useEnergy);
  ATH_MSG_DEBUG("END TCC READLHANDLE DIAGNOSTIC");
  ATH_MSG_DEBUG(m_tccInfoHandle.key());
  ATH_MSG_DEBUG("WRITEHANDLE DIAGNOSTIC");

  ATH_MSG_DEBUG("TCC INITS FROM PARENT CLASS");
  ATH_CHECK(m_inputClustersHandle.initialize() );
  ATH_CHECK(m_inputTracksHandle.initialize() );
  ATH_CHECK(m_inputVertexHandle.initialize());
  ATH_CHECK(m_tccInfoHandle.initialize());  
  ATH_CHECK(m_assoClustersKey.initialize());
  ATH_MSG_DEBUG("TCC INITS DONE");


  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterInfoUFOAlg::fillInfo(SG::WriteHandle<TrackCaloClusterInfo> & tccInfo) const {
    ATH_MSG_VERBOSE("TCC_UFO_ALG fillInfo start");


  SG::ReadHandle<xAOD::FlowElementContainer> pfos(m_inputPFOHandle);


  ATH_MSG_VERBOSE("GOT FE");
  // We use a dedicated helper to collect the weights mapping tracks to FlowElement.  
  TCCHelpers::WeightsCollector wcoll;
  wcoll.m_orig_pfoK = m_orig_pfo.key();
  ATH_MSG_VERBOSE("GOT ORIG PFO");
 
  wcoll.m_clustersLinkK="AssoClustersUFO"; //removing the container name from the assoClustersKey is required for the accessor to work. hardcode until a better solution is available

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
