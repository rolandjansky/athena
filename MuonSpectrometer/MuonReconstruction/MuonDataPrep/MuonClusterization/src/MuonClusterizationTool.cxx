/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterizationTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Muon {

  MuonClusterizationTool::MuonClusterizationTool(const std::string& t,const std::string& n, const IInterface*  p )
    :  AthAlgTool(t,n,p)
    , m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
    , m_tgcClustering(0)
    , m_rpcClustering(0)
  {
    declareInterface<IMuonClusterizationTool>(this);
    declareProperty("CombineGasGaps", m_combineGasGaps = true);
  } 

  MuonClusterizationTool::~MuonClusterizationTool()
  {
  }

  StatusCode MuonClusterizationTool::initialize()
  {
    if( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;

    if (m_idHelper.retrieve().isFailure()){
      msg(MSG::ERROR) <<"Could not get " << m_idHelper <<endmsg; 
      return StatusCode::FAILURE;
    }
  
    m_tgcClustering = new HitClusteringObj(m_idHelper->tgcIdHelper());
    m_rpcClustering = new HitClusteringObj(m_idHelper->rpcIdHelper());
    //m_rpcClustering->debug = true;
    m_rpcClustering->combinedGasGaps = m_combineGasGaps;
    m_tgcClustering->combinedGasGaps = m_combineGasGaps;
    return StatusCode::SUCCESS; 
  }

  StatusCode MuonClusterizationTool::finalize()
  {
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    delete m_tgcClustering;
    delete m_rpcClustering;

    return StatusCode::SUCCESS;
  }

  TgcPrepDataContainer* MuonClusterizationTool::cluster( const TgcPrepDataContainer& prdContainer ){
  
    TgcPrepDataContainer* clusteredContainer = new TgcPrepDataContainer(m_idHelper->tgcIdHelper().module_hash_max());
    // loop over Tgc collections in container
    TgcPrepDataContainer::const_iterator cit = prdContainer.begin();
    TgcPrepDataContainer::const_iterator cit_end = prdContainer.end();
    for( ;cit!=cit_end;++cit ){
      if( !*cit || (*cit)->empty() ) continue;
      const TgcPrepDataCollection& col = **cit;
      TgcPrepDataCollection* clusteredCol = cluster(col);
      if( clusteredCol ) clusteredContainer->addCollection(clusteredCol, col.identifyHash() ).ignore();
    }
    return clusteredContainer;
  }

  TgcPrepDataCollection* MuonClusterizationTool::cluster( const TgcPrepDataCollection& col ) {
    if( col.empty() ) return 0;
    TgcPrepDataCollection* collection = new TgcPrepDataCollection(col.identifyHash());
    collection->setIdentifier(col.identify());
    ATH_MSG_INFO("Performing clustering in " << m_idHelper->toString(col.identify()) );
    std::vector<const MuonCluster*> prds;
    prds.insert(prds.end(),col.begin(),col.end());
    m_tgcClustering->cluster( prds );
    addClusters(m_tgcClustering->clustersEta,collection);
    addClusters(m_tgcClustering->clustersPhi,collection);
    ATH_MSG_INFO("  input size " << col.size() << " output size " << collection->size());

    return collection;
  }

  void MuonClusterizationTool::addClusters( const std::vector<ClusterObj>& clusters, TgcPrepDataCollection* collection ){
    std::vector<ClusterObj>::const_iterator cit = clusters.begin();
    std::vector<ClusterObj>::const_iterator cit_end = clusters.end();
    for( ;cit!=cit_end;++cit ){

      // ignore non-active clusters
      const ClusterObj& cl = *cit;
      if( !cl.active() ) continue;
      const TgcPrepData* first = static_cast<const TgcPrepData*>(cl.hitList.front());
      if( !first ) continue;

      // copy identifiers of contained prds
      std::vector<Identifier> rdoList;
      rdoList.reserve(cl.hitList.size());
      std::vector< const MuonCluster*>::const_iterator hit = cl.hitList.begin();
      std::vector< const MuonCluster*>::const_iterator hit_end = cl.hitList.end();
      for( ;hit!=hit_end;++hit ) rdoList.push_back((*hit)->identify());

      // create new PRD object
      TgcPrepData* prd = new TgcPrepData( first->identify(), first->collectionHash(), 
					  first->localPosition(),
					  rdoList, 
					  new Amg::MatrixX( first->localCovariance() ),
					  first->detectorElement());
      //TgcPrepData::BC_CURRENT); add bc id
      collection->push_back(prd);
    }
  }


  RpcPrepDataContainer* MuonClusterizationTool::cluster( const RpcPrepDataContainer& prdContainer ){
  
    RpcPrepDataContainer* clusteredContainer = new RpcPrepDataContainer(m_idHelper->rpcIdHelper().module_hash_max());
    // loop over Rpc collections in container
    RpcPrepDataContainer::const_iterator cit = prdContainer.begin();
    RpcPrepDataContainer::const_iterator cit_end = prdContainer.end();
    for( ;cit!=cit_end;++cit ){
      if( !*cit || (*cit)->empty() ) continue;
      const RpcPrepDataCollection& col = **cit;
      RpcPrepDataCollection* clusteredCol = cluster(col);
      if( clusteredCol ) clusteredContainer->addCollection(clusteredCol, col.identifyHash() ).ignore();
    }
    return clusteredContainer;
  }

  RpcPrepDataCollection* MuonClusterizationTool::cluster( const RpcPrepDataCollection& col ) {
    if( col.empty() ) return 0;
    RpcPrepDataCollection* collection = new RpcPrepDataCollection(col.identifyHash());
    collection->setIdentifier(col.identify());
    ATH_MSG_INFO("Performing clustering in " << m_idHelper->toString(col.identify()) );
    std::vector<const MuonCluster*> prds;
    prds.insert(prds.end(),col.begin(),col.end());
    m_rpcClustering->cluster( prds );
    addClusters(m_rpcClustering->clustersEta,collection);
    addClusters(m_rpcClustering->clustersPhi,collection);
    ATH_MSG_INFO("  input size " << col.size() << " output size " << collection->size());
   
    return collection;
  }

  void MuonClusterizationTool::addClusters( const std::vector<ClusterObj>& clusters, RpcPrepDataCollection* collection ){
    std::vector<ClusterObj>::const_iterator cit = clusters.begin();
    std::vector<ClusterObj>::const_iterator cit_end = clusters.end();
    for( ;cit!=cit_end;++cit ){

      // ignore non-active clusters
      const ClusterObj& cl = *cit;
      if( !cl.active() ) continue;
      const RpcPrepData* first = static_cast<const RpcPrepData*>(cl.hitList.front());
      if( !first ) continue;

      // copy identifiers of contained prds
      std::vector<Identifier> rdoList;
      rdoList.reserve(cl.hitList.size());
      std::vector< const MuonCluster*>::const_iterator hit = cl.hitList.begin();
      std::vector< const MuonCluster*>::const_iterator hit_end = cl.hitList.end();
      for( ;hit!=hit_end;++hit ) rdoList.push_back((*hit)->identify());

      // create new PRD object
      RpcPrepData* prd = new RpcPrepData( first->identify(), first->collectionHash(), 
					  first->localPosition(),
					  rdoList, 
					  new Amg::MatrixX(first->localCovariance() ),
					  first->detectorElement(),
					  first->time(),
					  first->triggerInfo(),
					  first->ambiguityFlag());
      //RpcPrepData::BC_CURRENT); add bc id
      collection->push_back(prd);
    }
  }

}
