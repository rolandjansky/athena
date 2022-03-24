/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/src/HGTD_RDOContainerCnv_p1.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Implementation file of HGTD_RDOContainerCnv_p1.h
 */

#include "HGTD_EventTPCnv/HGTD_RDOContainerCnv_p1.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "HGTD_EventTPCnv/HGTD_RDOCnv_p1.h"
#include "HGTD_EventTPCnv/HGTD_RDOCollection_p1.h"
#include "HGTD_EventTPCnv/HGTD_RDO_p1.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"

#include <memory>

StatusCode HGTD_RDOContainerCnv_p1::initialize(MsgStream& log) {
  // Do not initialize again:
  m_is_initialized = true;

  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();

  StoreGateSvc* detStore;
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

void HGTD_RDOContainerCnv_p1::transToPers(
    const Trans_t* transient_container, Pers_t* persistent_container,
    MsgStream& log) {

  if (!m_is_initialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize HGTD_RDOContainerCnv_p1 "
          << endmsg;
    }
  }

  HGTD_RDOCnv_p1 rdo_converter;
  size_t n_collections = transient_container->numberOfCollections();
  Trans_t::const_iterator container_itr = transient_container->begin();

  persistent_container->m_collection_separator.resize(n_collections);

  size_t collection_separator_index_begin = 0;
  size_t total_n_clusters = 0;

  for (size_t coll_i = 0; coll_i < n_collections; coll_i++, ++container_itr) {
    const HGTD_RDOCollection& collection = (**container_itr);

    size_t collection_size = collection.size();

    persistent_container->m_collection_separator.at(coll_i).m_hash_id =
        collection.identifierHash().value();
    persistent_container->m_collection_separator.at(coll_i).m_size =
        collection_size;

    // continously resize the toal size of vector holding the individual
    // clusters
    total_n_clusters += collection_size;
    persistent_container->m_rdo_list.resize(total_n_clusters);

    if (log.level() <= MSG::VERBOSE) {
      log << MSG::VERBOSE << "Reading RDO collections size of "
          << collection_size << endmsg;
    }

    for (size_t rdo_i = 0; rdo_i < collection_size; rdo_i++) {
      // get pointer to next position in the vector that will be persistified
      HGTD_RDO_p1* pers_rdo =
          &((persistent_container->m_rdo_list)
                .at(rdo_i + collection_separator_index_begin));

      const HGTD_RDO* trans_rdo =
          dynamic_cast<const HGTD_RDO*>(collection.at(rdo_i));

      rdo_converter.transToPers(trans_rdo, pers_rdo, log);
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

void HGTD_RDOContainerCnv_p1::persToTrans(
    const Pers_t* persistent_container, Trans_t* transient_container,
    MsgStream& log) {

  if (!m_is_initialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize HGTD_RDOContainerCnv_p1 "
          << endmsg;
    }
  }

  std::unique_ptr<HGTD_RDOCollection> collection = nullptr;

  HGTD_RDOCnv_p1 rdo_converter;
  size_t collection_separator_index_begin = 0;

  for (size_t coll_i = 0;
       coll_i < persistent_container->m_collection_separator.size(); ++coll_i) {
    const HGTD_RDOCollection_p1& rdo_coll =
        persistent_container->m_collection_separator.at(coll_i);
    // get the identifier for the collection
    IdentifierHash coll_idhash = IdentifierHash(rdo_coll.m_hash_id);
    Identifier coll_id = m_hgtd_idhelper->wafer_id(coll_idhash);
    
    collection = std::make_unique<HGTD_RDOCollection>(coll_idhash);
    collection->setIdentifier(coll_id);

    unsigned short n_clusters = rdo_coll.m_size;
    collection->resize(n_clusters);
    for (unsigned short rdo_i = 0; rdo_i < n_clusters; ++rdo_i) {
      const HGTD_RDO_p1* pers_rdo =
          &((persistent_container->m_rdo_list)
                .at(rdo_i + collection_separator_index_begin));

      // NOTE I think I have to new it before calling the converter
      HGTD_RDO* trans_rdo = new HGTD_RDO();
      rdo_converter.persToTrans(pers_rdo, trans_rdo, log);
      (*collection).at(rdo_i) = trans_rdo;
    }
    collection_separator_index_begin += n_clusters;

    StatusCode sc =
        transient_container->addCollection(collection.release(), coll_idhash);
    if (sc.isFailure()) {
      throw std::runtime_error("Failed to add collection to ID Container");
    }
  }
}

HGTD_RDOContainerCnv_p1::Trans_t*
HGTD_RDOContainerCnv_p1::createTransient(
    const Pers_t* persistent_container, MsgStream& log) {

  if (!m_is_initialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize HGTD_RDOContainerCnv_p1 "
          << endmsg;
    }
  }

  std::unique_ptr<Trans_t> transient_container =
      std::make_unique<Trans_t>(m_hgtd_idhelper->wafer_hash_max());

  persToTrans(persistent_container, transient_container.get(), log);

  return (transient_container.release());
}
