/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterContainerCnv_p0.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetIdentifier/PixelID.h"
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


StatusCode PixelClusterContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   // Get the messaging service, print where you are
   log << MSG::INFO << "PixelClusterContainerCnv::initialize()" << endreq;

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

   // Get the pixel helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
   } else {
      MSG_DEBUG(log,"Found the PixelID helper.");
   }

   sc = detStore->retrieve(m_pixMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelDetectorDescription" << endreq;
      return sc;
   }

   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::PixelClusterContainer* PixelClusterContainerCnv_p0::createTransient(const PixelClusterContainer_p0* persObj, MsgStream& log) {

  std::auto_ptr<InDet::PixelClusterContainer> trans(new InDet::PixelClusterContainer(m_pixId->wafer_hash_max()) );
  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());
  
  PixelClusterContainer_p0::const_iterator it   = persObj->begin();
  PixelClusterContainer_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
      const InDet::PixelClusterCollection* dcColl = *it;
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::SiDetectorElement * de = m_pixMgr->getDetectorElement(collHash);
      MSG_DEBUG(log,"Set PixelCluster detector element to "<< de);

      InDet::PixelClusterCollection::const_iterator itColl   = dcColl->begin();
      InDet::PixelClusterCollection::const_iterator lastColl = dcColl->end();
      for (int num = 0; itColl != lastColl; ++itColl, ++num) {
	 MSG_DEBUG(log,"PRD " << num);
         (*itColl)->m_detEl = de;
      }

      StatusCode sc= trans->addCollection(dcColl, collHash);
      if (sc.isSuccess()){
	 MSG_VERBOSE(log,"PixelClusterContainer successfully added to Container !");
      } else {
         log << MSG::ERROR << "Failed to add PixelClusterContainer to container" << endreq;
         return 0;
      }
   }
  return(trans.release());
}

