/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DriftCircleContainerCnv_p0.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "MsgUtil.h"
#include "AthenaKernel/errorcheck.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include <string>
#include <iostream>
#include <sstream>

//================================================================


StatusCode TRT_DriftCircleContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   log << MSG::INFO << "TRT_DriftCircleContainerCnv::initialize()" << endmsg;

   StoreGateSvc *detStore = nullptr;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_trtId, "TRT_ID") );
   // Initialize readCondHandle key                                                  
   CHECK(m_trtDetEleContKey.initialize());
   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::TRT_DriftCircleContainer* TRT_DriftCircleContainerCnv_p0::createTransient(TRT_DriftCircleContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<InDet::TRT_DriftCircleContainer> trans(std::make_unique<InDet::TRT_DriftCircleContainer>(m_trtId->straw_layer_hash_max()) );
  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());

  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey);
  const InDetDD::TRT_DetElementCollection* elements(trtDetEleHandle->getElements());
  if (not trtDetEleHandle.isValid() or elements==nullptr) {
    log << MSG::FATAL << m_trtDetEleContKey.fullKey() << " is not available." << endmsg;
    return trans.release();
  }
  
  for (InDet::TRT_DriftCircleCollection* dcColl : *persObj) {
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::TRT_BaseElement * de = elements->getDetectorElement(collHash);
      MSG_DEBUG(log,"Set TRT_DriftCircle detector element to "<< de);

      InDet::TRT_DriftCircleCollection::iterator itColl   = dcColl->begin();
      InDet::TRT_DriftCircleCollection::iterator lastColl = dcColl->end();
      for (int num = 0; itColl != lastColl; ++itColl, ++num) {
	      MSG_DEBUG(log,"PRD " << num);
         (*itColl)->m_detEl = de;
      }

      StatusCode sc= trans ? trans->addCollection(dcColl, collHash): StatusCode::FAILURE;
      if (sc.isSuccess()){
	 MSG_VERBOSE(log,"TRT_DriftCircleContainer successfully added to Container !");
      } else {
         log << MSG::ERROR << "Failed to add TRT_DriftCircleContainer to container" << endmsg;
         return nullptr;
      }
   }
  return(trans.release());
}

