/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterizationTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Muon {

  MuonClusterizationTool::MuonClusterizationTool(const std::string& t,const std::string& n, const IInterface* p) :
      AthAlgTool(t,n,p),
      m_tgcClustering(nullptr),
      m_rpcClustering(nullptr)
  {
    declareInterface<IMuonClusterizationTool>(this);
    declareProperty("CombineGasGaps", m_combineGasGaps = true);
  } 

  StatusCode MuonClusterizationTool::initialize()
  {
    ATH_CHECK(m_idHelperSvc.retrieve());
    m_tgcClustering = new HitClusteringObj(m_idHelperSvc->tgcIdHelper());
    m_rpcClustering = new HitClusteringObj(m_idHelperSvc->rpcIdHelper());
    m_rpcClustering->combinedGasGaps = m_combineGasGaps;
    m_tgcClustering->combinedGasGaps = m_combineGasGaps;
    return StatusCode::SUCCESS; 
  }

  StatusCode MuonClusterizationTool::finalize()
  {
    delete m_tgcClustering;
    delete m_rpcClustering;
    return StatusCode::SUCCESS;
  }

  TgcPrepDataContainer* MuonClusterizationTool::cluster( const TgcPrepDataContainer& prdContainer ) const {
  
    TgcPrepDataContainer* clusteredContainer = new TgcPrepDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max());
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

  TgcPrepDataCollection* MuonClusterizationTool::cluster( const TgcPrepDataCollection& col ) const {
    if( col.empty() ) return 0;
    TgcPrepDataCollection* collection = new TgcPrepDataCollection(col.identifyHash());
    collection->setIdentifier(col.identify());
    ATH_MSG_INFO("Performing clustering in " << m_idHelperSvc->toString(col.identify()) );
    std::vector<const MuonCluster*> prds;
    prds.insert(prds.end(),col.begin(),col.end());
    m_tgcClustering->cluster( prds );
    addClusters(m_tgcClustering->clustersEta,collection);
    addClusters(m_tgcClustering->clustersPhi,collection);
    ATH_MSG_INFO("  input size " << col.size() << " output size " << collection->size());

    return collection;
  }

  void MuonClusterizationTool::addClusters( const std::vector<ClusterObj>& clusters, TgcPrepDataCollection* collection ) const {
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


  RpcPrepDataContainer* MuonClusterizationTool::cluster( const RpcPrepDataContainer& prdContainer ) const {
  
    RpcPrepDataContainer* clusteredContainer = new RpcPrepDataContainer(m_idHelperSvc->rpcIdHelper().module_hash_max());
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

  RpcPrepDataCollection* MuonClusterizationTool::cluster( const RpcPrepDataCollection& col ) const {
    if( col.empty() ) return 0;
    RpcPrepDataCollection* collection = new RpcPrepDataCollection(col.identifyHash());
    collection->setIdentifier(col.identify());
    ATH_MSG_INFO("Performing clustering in " << m_idHelperSvc->toString(col.identify()) );
    std::vector<const MuonCluster*> prds;
    prds.insert(prds.end(),col.begin(),col.end());
    m_rpcClustering->cluster( prds );
    addClusters(m_rpcClustering->clustersEta,collection);
    addClusters(m_rpcClustering->clustersPhi,collection);
    ATH_MSG_INFO("  input size " << col.size() << " output size " << collection->size());
   
    return collection;
  }

  void MuonClusterizationTool::addClusters( const std::vector<ClusterObj>& clusters, RpcPrepDataCollection* collection ) const {
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
