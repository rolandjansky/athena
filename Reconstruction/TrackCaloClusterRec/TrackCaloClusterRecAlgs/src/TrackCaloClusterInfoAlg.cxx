/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackCaloClusterInfoAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterInfoAlg.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/VertexContainer.h"


#include "xAODBase/ObjectType.h"


TrackCaloClusterInfoAlg::TrackCaloClusterInfoAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
      
  //declareProperty("OutputName"      ,    m_outputName                   = ""                      );
  declareProperty("TCCInfoName",    m_tccInfoName   = "TCCInfo", "Name of TCCInfo to be created"     );
  //declareProperty("InputObjects",    m_inputObjectName   = "CaloCalTopoClusters", "Name of containerfrom which to build TCC (usually cluster or pfo)"     );

  declareProperty("InputTrackCaloAssoc",    m_inputTrackCaloAssocName   = "InDetTrackParticlesClusterAssociationsTCC", "Name track-cluster association object"     );
  
  declareProperty("InputTracks",    m_inputTracksName   = "InDetTrackParticles", ""     );
  declareProperty("InputClusters",  m_inputClustersName = "CaloCalTopoClusters", ""     );

  declareProperty("UseEnergy",        m_useEnergy = false );
  declareProperty("VertexContainer",  m_vertexContname = "PrimaryVertices");

  
}
  
TrackCaloClusterInfoAlg::~TrackCaloClusterInfoAlg() {}
  
StatusCode TrackCaloClusterInfoAlg::initialize() {
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterInfoAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterInfoAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  TrackCaloClusterInfo * tccInfo = new TrackCaloClusterInfo();
    
  // 
  ATH_CHECK( evtStore()->retrieve( tccInfo->assocContainer, m_inputTrackCaloAssocName  ) );

  ATH_CHECK( evtStore()->retrieve( tccInfo->allTracks, m_inputTracksName  ) );

  ATH_CHECK( evtStore()->retrieve( tccInfo->allClusters, m_inputClustersName  ) );

  const xAOD::VertexContainer *vxCont=nullptr;
  ATH_CHECK( evtStore()->retrieve(vxCont, m_vertexContname) ); 
  if(!vxCont->empty()) tccInfo->pv0=(*vxCont)[0]; // Hard code HS vertex as PV0 : WARNING we expect it is the same as was used for m_inputTrackCaloAssocName !!
  else {    
    ATH_MSG_ERROR ("Vertex container " << m_vertexContname << " is empty! Can't perform TVA!");
    return StatusCode::FAILURE;
  }

  StatusCode sc = fillInfo(*tccInfo);  

  ATH_CHECK(evtStore()->record(tccInfo, m_tccInfoName));

  return sc;
}

StatusCode TrackCaloClusterInfoAlg::fillInfo(TrackCaloClusterInfo & tccInfo){


  std::multimap <const xAOD::IParticle*, const xAOD::TrackParticle*> clusterToTracksMap;

  static const FourMom_t nullV(0,0,0,0);
  // First loop to fill cluster-to-tracks map
  for ( const auto* assocClusters : *tccInfo.assocContainer ) {
    // flollow the link to the track particle
    const xAOD::TrackParticle* trk = 0;
    if (assocClusters->trackParticleLink().isValid()) {
      trk = *(assocClusters->trackParticleLink());
    }
    else {
      ATH_MSG_ERROR ("trackParticleLink is not valid! " );
    }
    
    // follow the link to the calorimeter clusters
    for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
      const xAOD::IParticle* cluster = *(assocClusters->caloClusterLinks().at(c));
      clusterToTracksMap.insert(std::make_pair(cluster, trk));
      // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
      FourMom_t & totalP4 = ( tccInfo.trackTotalClusterPt.insert( {trk, nullV} ) ).first->second;
      totalP4 += cluster->p4(); // add the cluster p4 into the map.
      
    } // for caloClusterLinks
  } // for assoc clusters
  
  // Create cluster-to-tracks weight map
  for (auto entry : clusterToTracksMap)
    {
      const xAOD::IParticle* cluster = entry.first;
      double cluster_pt       = m_useEnergy ? cluster->e() : cluster->pt();
      double totalcluster_pt  = m_useEnergy ? tccInfo.trackTotalClusterPt.at(entry.second).E() : tccInfo.trackTotalClusterPt.at(entry.second).Pt();

      // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
      FourMom_t & totalP4 = ( tccInfo.clusterToTracksWeightMap.insert( {cluster, nullV} ) ).first->second;
      totalP4 += entry.second->p4() * (cluster_pt/totalcluster_pt); // add the track p4 into the map.
    }

  
  return StatusCode::SUCCESS;
  
}














TrackCaloClusterInfoUFOAlg::TrackCaloClusterInfoUFOAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  TrackCaloClusterInfoAlg( name, pSvcLocator )
  ,  m_loosetrackvertexassoTool("LooseTrackVertexAssociationTool", this)

{
      
  declareProperty("PFOPrefix",        m_pfoPrefix = "CSSKJetETMiss" );
  declareProperty("LooseTrackVertexAssoTool" ,  m_loosetrackvertexassoTool  );
  declareProperty("ClusterECut", m_clusterEcut=0.); // WARNING this property must be the same as the one in UFOTool
  
}
  
TrackCaloClusterInfoUFOAlg::~TrackCaloClusterInfoUFOAlg() {}
  
StatusCode TrackCaloClusterInfoUFOAlg::initialize() {
  ATH_CHECK(m_loosetrackvertexassoTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterInfoUFOAlg::fillInfo(TrackCaloClusterInfo & tccInfo){


  std::multimap <const xAOD::IParticle*, const xAOD::TrackParticle*> pfoToTracksMap;
  
  // These are the PFO containers that we actually want to use to create the Unholy Alliance
  const xAOD::PFOContainer* chPFO = nullptr;
  ATH_CHECK( evtStore()->retrieve(chPFO, m_pfoPrefix+"ChargedParticleFlowObjects") );

  const xAOD::PFOContainer* nPFO = nullptr;
  ATH_CHECK( evtStore()->retrieve(nPFO, m_pfoPrefix+"NeutralParticleFlowObjects") );

  
  // These are the unmodified PFO containers. Currently, we need these to do a cluster matching, since the modified
  // containers change the eta and phi of each PFO
  const xAOD::PFOContainer* chPFO_orig = nullptr;
  ATH_CHECK( evtStore()->retrieve(chPFO_orig, "JetETMissChargedParticleFlowObjects") );

  const xAOD::PFOContainer* nPFO_orig = nullptr;
  ATH_CHECK( evtStore()->retrieve(nPFO_orig, "JetETMissNeutralParticleFlowObjects") );

  
  static const FourMom_t nullV(0,0,0,0);
  
  // First loop to fill cluster-to-tracks map
  for ( const auto* assocClusters : *tccInfo.assocContainer ) {

    if (!assocClusters->trackParticleLink().isValid()){ATH_MSG_ERROR ("trackParticleLink is not valid! " ); continue;}
    const xAOD::TrackParticle* trk = *(assocClusters->trackParticleLink());

    // if not matched to PV0, ignore track
    if (! m_loosetrackvertexassoTool->isCompatible(*trk, *tccInfo.pv0) ) continue ;
    

    // Only include tracks which aren't associated to a charged PFO -- don't want to double count!
    bool isTrackMatchedToPFO = false;
    for ( const auto* cpfo : *chPFO ){

      int isInDenseEnvironment = false;
      cpfo->attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
      if(isInDenseEnvironment) continue;

      if(cpfo->track(0) == trk) {
        isTrackMatchedToPFO = true;
        break;
      }
    }
    if(isTrackMatchedToPFO) continue;
 
    // follow the link to the calorimeter clusters
    for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
        const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));

        // See if any of the PFO are matched to this cluster
        int i=-1;
        for ( const auto* cpfo : *nPFO ){
          i++;
          if(cpfo->pt()<=0.) continue;
          // Should do a beter dPhi matching, but this should basically be fine
          if( !(fabs((nPFO_orig->at(i))->eta()  - cluster->rawEta() )<0.01 && fabs((nPFO_orig->at(i))->phi()  - cluster->rawPhi() )<0.01)) continue;

          // Only want to apply this to PFO which aren't able to be subtracted with E/p
          if(cluster->rawE() < m_clusterEcut) continue;

          pfoToTracksMap.insert(std::make_pair(cpfo, trk));

          FourMom_t & totalP4 = ( tccInfo.trackTotalClusterPt.insert( {trk, nullV} ) ).first->second;
          totalP4 += cpfo->p4(); // add the cluster p4 into the map.
          
        } // for npfoContainer

        i = -1;
        // See if any of the charged PFO are matched to this cluster
        for ( const auto* cpfo : *chPFO ){
          i++;
          if(cpfo->pt()<=0.) continue;

          // This compares the location of the uncorrected, charged PFO to the cluster to see if they are a match
          if( !(fabs((chPFO_orig->at(i))->eta()  - cluster->rawEta() )<0.01 && fabs((chPFO_orig->at(i))->phi()  - cluster->rawPhi() )<0.01)) continue;

          // Only want to apply this to PFO which aren't able to be subtracted with E/p
          int isInDenseEnvironment = false;
          cpfo->attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
          if(!isInDenseEnvironment) continue;

          pfoToTracksMap.insert(std::make_pair(cpfo, trk));

          FourMom_t & totalP4 = ( tccInfo.trackTotalClusterPt.insert( {trk, nullV} ) ).first->second;
          totalP4 += cpfo->p4(); // add the cluster p4 into the map.

        } // for chPFO

    } // for clusterLinks
  } // for assoc clusters
    
    // Create cluster-to-tracks weight map
  for (auto entry : pfoToTracksMap){
    double cluster_pt       = m_useEnergy ? entry.first->e() : entry.first->pt();
    double totalcluster_pt  = m_useEnergy ? tccInfo.trackTotalClusterPt.at(entry.second).E() : tccInfo.trackTotalClusterPt.at(entry.second).Pt();

    // find FourMom_t for trk or insert (0,0,0,0) if not yet in map :
    FourMom_t & totalP4 = ( tccInfo.clusterToTracksWeightMap.insert( {entry.first, nullV} ) ).first->second;
    totalP4 += entry.second->p4() * (cluster_pt/totalcluster_pt); // add the track p4 into the map.
    
  }

  return StatusCode::SUCCESS;
  
}
