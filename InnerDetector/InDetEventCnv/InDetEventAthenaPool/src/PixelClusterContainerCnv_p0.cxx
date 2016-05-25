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
#include "AthenaKernel/errorcheck.h"

#include <string>
#include <iostream>
#include <sstream>

//================================================================


StatusCode PixelClusterContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   log << MSG::INFO << "PixelClusterContainerCnv::initialize()" << endreq;

   StoreGateSvc *detStore = nullptr;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_pixId, "PixelID") );
   CHECK( detStore->retrieve(m_pixMgr) );

   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::PixelClusterContainer* PixelClusterContainerCnv_p0::createTransient(PixelClusterContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<InDet::PixelClusterContainer> trans(new InDet::PixelClusterContainer(m_pixId->wafer_hash_max()) );
  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());

  for (InDet::PixelClusterCollection* dcColl : *persObj) {
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::SiDetectorElement * de = m_pixMgr->getDetectorElement(collHash);
      MSG_DEBUG(log,"Set PixelCluster detector element to "<< de);

      InDet::PixelClusterCollection::iterator itColl   = dcColl->begin();
      InDet::PixelClusterCollection::iterator lastColl = dcColl->end();
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

