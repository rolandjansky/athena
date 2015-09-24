/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ClusterContainerCnv_p0.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/SCT_ID.h"
#include "MsgUtil.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <iostream>
#include <sstream>

//================================================================


StatusCode SCT_ClusterContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   // Get the messaging service, print where you are
   log << MSG::INFO << "SCT_ClusterContainerCnv::initialize()" << endreq;

   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } else {
      MSG_DEBUG(log,"Found DetectorStore.");
   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_sctId, "SCT_ID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_ID helper !" << endreq;
      return StatusCode::FAILURE;
   } else {
     MSG_DEBUG(log,"Found the SCT_ID helper.");
   }

   sc = detStore->retrieve(m_sctMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_DetectorDescription" << endreq;
      return sc;
   }

   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::SCT_ClusterContainer* SCT_ClusterContainerCnv_p0::createTransient(const SCT_ClusterContainer_p0* persObj, MsgStream& log) {

  std::auto_ptr<InDet::SCT_ClusterContainer> trans(new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max()) );
  //  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());
  
  SCT_ClusterContainer_p0::const_iterator it   = persObj->begin();
  SCT_ClusterContainer_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
      const InDet::SCT_ClusterCollection* dcColl = *it;
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::SiDetectorElement * de = m_sctMgr->getDetectorElement(collHash);
      //      MSG_DEBUG(log,"Set SCT_Cluster detector element to "<< de);

      InDet::SCT_ClusterCollection::const_iterator itColl   = dcColl->begin();
      InDet::SCT_ClusterCollection::const_iterator lastColl = dcColl->end();
      for (int num = 0; itColl != lastColl; ++itColl, ++num) {
	//         MSG_DEBUG(log,"PRD " << num);
         (*itColl)->m_detEl = de;
      }

      StatusCode sc= trans->addCollection(dcColl, collHash);
      if (sc.isSuccess()){
	//         MSG_VERBOSE("SCT_ClusterContainer successfully added to Container !");
      } else {
         log << MSG::ERROR << "Failed to add SCT_ClusterContainer to container" << endreq;
         return 0;
      }
   }
  return(trans.release());
}

