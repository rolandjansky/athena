/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/src/HGTD_ClusterContainerCnv_p1.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Implementation file of HGTD_ClusterContainer_p1.h
 */

#include "HGTD_EventTPCnv/HGTD_ClusterContainerCnv_p1.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "HGTD_EventTPCnv/HGTD_ClusterCnv_p1.h"
#include "HGTD_EventTPCnv/HGTD_ClusterContainer_p1.h"
#include "HGTD_EventTPCnv/HGTD_Cluster_p1.h"
#include "HGTD_EventTPCnv/HGTD_PRD_Collection_p1.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"
#include <memory>

StatusCode HGTD_ClusterContainerCnv_p1::initialize(MsgStream& log) {
  // Do not initialize again:
  m_is_initialized = true;

  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();


  StoreGateSvc* detStore;
  // get StoreGate service
  StatusCode sc = svcLocator->service("StoreGateSvc", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endmsg;
    return StatusCode::FAILURE;
  }
  // get DetectorStore service
  sc = svcLocator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }
  // Get the ID helper from the detector store
  sc = detStore->retrieve(m_hgtd_idhelper, "HGTD_ID");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get HGTD_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void HGTD_ClusterContainerCnv_p1::transToPers(
    const Trans_t* transient_container, Pers_t* persistent_container,
    MsgStream& log) {

  // The transient model has a container holding collections and the
  // collections hold channels.
  //
  // The persistent model flattens this so that the persistent
  // container has two vectors:
  //   1) all collections, and
  //   2) all PRD
  //
  // The persistent collections, then only maintain indexes into the
  // container's vector of all channels.
  //
  // So here we loop over all collection and add their channels
  // to the container's vector, saving the indexes in the
  // collection.

  if (!m_is_initialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize HGTD_ClusterContainerCnv_p1 "
          << endmsg;
    }
  }

  HGTD_ClusterCnv_p1 cluster_converter;
  size_t n_collections = transient_container->numberOfCollections();
  Trans_t::const_iterator container_itr = transient_container->begin();

  persistent_container->m_collection_separator.resize(n_collections);

  size_t collection_separator_index_begin = 0;
  size_t total_n_clusters = 0;

  for (size_t coll_i = 0; coll_i < n_collections; coll_i++, ++container_itr) {
    const HGTD_ClusterCollection& collection = (**container_itr);

    size_t collection_size = collection.size();

    persistent_container->m_collection_separator.at(coll_i).m_hash_id =
        collection.identifyHash().value();
    persistent_container->m_collection_separator.at(coll_i).m_size =
        collection_size;

    // continously resize the toal size of vector holding the individual
    // clusters
    total_n_clusters += collection_size;
    persistent_container->m_cluster_list.resize(total_n_clusters);

    if (log.level() <= MSG::VERBOSE) {
      log << MSG::VERBOSE << "Reading collections with " << collection_size
          << " PRDs " << endmsg;
    }

    for (size_t clus_i = 0; clus_i < collection_size; clus_i++) {
      // get pointer to next position in the vector that will be persistified
      HGTD_Cluster_p1* pers_clus =
          &((persistent_container->m_cluster_list)
                .at(clus_i + collection_separator_index_begin));

      const HGTD_Cluster* trans_clus =
          dynamic_cast<const HGTD_Cluster*>(collection.at(clus_i));

      cluster_converter.transToPers(trans_clus, pers_clus, log);
    }
    // start next collection at end of previous
    collection_separator_index_begin += collection.size();
  }

  if (log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG
        << "Writing HGTD_ClusterContainer to HGTD_ClusterContainer_p1 done"
        << endmsg;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void HGTD_ClusterContainerCnv_p1::persToTrans(
    const Pers_t* persistent_container, Trans_t* transient_container,
    MsgStream& log) {

  HGTD_ClusterCollection* collection = nullptr;

  HGTD_ClusterCnv_p1 cluster_converter;

  size_t collection_separator_index_begin = 0;

  for (size_t coll_i = 0;
       coll_i < persistent_container->m_collection_separator.size(); ++coll_i) {

    const HGTD_PRD_Collection_p1& prd_coll =
        persistent_container->m_collection_separator.at(coll_i);

    // get the identifier for the collection
    IdentifierHash coll_idhash = IdentifierHash(prd_coll.m_hash_id);
    Identifier coll_id = m_hgtd_idhelper->wafer_id(coll_idhash);

    collection = new HGTD_ClusterCollection(coll_idhash);
    collection->setIdentifier(coll_id);

    unsigned short n_clusters = prd_coll.m_size;
    collection->resize(n_clusters);

    for (unsigned short clus_i = 0; clus_i < n_clusters; ++clus_i) {
      const HGTD_Cluster_p1* pers_cluster =
          &((persistent_container->m_cluster_list)
                .at(clus_i + collection_separator_index_begin));
      // NOTE the cluster is created without setting the detector element!
      // NOTE if this is needed down the road, it has to be added here!
      HGTD_Cluster* trans_cluster = new HGTD_Cluster(
          cluster_converter.createHGTDCluster(pers_cluster, nullptr, log));

      trans_cluster->setHashAndIndex(coll_idhash, clus_i);
      (*collection).at(clus_i) = trans_cluster;
    }
    collection_separator_index_begin += n_clusters;

    StatusCode sc = transient_container->addCollection(collection, coll_idhash);
    if (sc.isFailure()) {
      throw std::runtime_error("Failed to add collection to ID Container");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

HGTD_ClusterContainer* HGTD_ClusterContainerCnv_p1::createTransient(
    const Pers_t* persistent_container, MsgStream& log) {

  if (!m_is_initialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize HGTD_ClusterContainerCnv_p1 "
          << endmsg;
    }
  }

  std::unique_ptr<Trans_t> transient_container =
      std::make_unique<Trans_t>(m_hgtd_idhelper->wafer_hash_max());

  persToTrans(persistent_container, transient_container.get(), log);

  return (transient_container.release());
}
