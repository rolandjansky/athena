/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ClusterContainerCnv_p0.h"

#include "MsgUtil.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

#include <iostream>
#include <sstream>
#include <string>

//================================================================


SCT_ClusterContainerCnv_p0::SCT_ClusterContainerCnv_p0():
  m_sctId{nullptr},
  m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}
{
}

StatusCode SCT_ClusterContainerCnv_p0::initialize(MsgStream& log ) {

  ISvcLocator* svcLocator = Gaudi::svcLocator();

  // Get the messaging service, print where you are
  log << MSG::INFO << "SCT_ClusterContainerCnv::initialize()" << endmsg;

  StoreGateSvc* detStore = nullptr;
  CHECK( svcLocator->service("DetectorStore", detStore) );
  CHECK( detStore->retrieve(m_sctId, "SCT_ID") );
  CHECK( m_SCTDetEleCollKey.initialize() );
  MSG_DEBUG(log,"Converter initialized.");

  return StatusCode::SUCCESS;
}

InDet::SCT_ClusterContainer* SCT_ClusterContainerCnv_p0::createTransient(SCT_ClusterContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<InDet::SCT_ClusterContainer> trans(std::make_unique<InDet::SCT_ClusterContainer>(m_sctId->wafer_hash_max()) );
  //  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(*sctDetEleHandle);
  if (not sctDetEleHandle.isValid() or elements==nullptr) {
    log << MSG::FATAL << m_SCTDetEleCollKey.fullKey() << " is not available." << endmsg;
    return nullptr;
  }
  
  for (InDet::SCT_ClusterCollection* dcColl : *persObj) {
    // Add detElem to each drift circle
    IdentifierHash collHash = dcColl->identifyHash();
    const InDetDD::SiDetectorElement * de = elements->getDetectorElement(collHash);
    //      MSG_DEBUG(log,"Set SCT_Cluster detector element to "<< de);

    InDet::SCT_ClusterCollection::iterator itColl   = dcColl->begin();
    InDet::SCT_ClusterCollection::iterator lastColl = dcColl->end();
    for (; itColl != lastColl; ++itColl) {
      (*itColl)->m_detEl = de;
    }

    StatusCode sc= trans->addCollection(dcColl, collHash);
    if (sc.isSuccess()){
      //         MSG_VERBOSE("SCT_ClusterContainer successfully added to Container !");
    } else {
      log << MSG::ERROR << "Failed to add SCT_ClusterContainer to container" << endmsg;
      return nullptr;
    }
  }
  return(trans.release());
}
