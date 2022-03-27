/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterContainerCnv_p0.h"

#include "MsgUtil.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "InDetIdentifier/PixelID.h"
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

PixelClusterContainerCnv_p0::PixelClusterContainerCnv_p0():
  m_pixelDetEleCollKey{"PixelDetectorElementCollection"}
{
}

StatusCode PixelClusterContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   log << MSG::INFO << "PixelClusterContainerCnv::initialize()" << endmsg;

   StoreGateSvc* detStore = nullptr;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_pixId, "PixelID") );
   CHECK( m_pixelDetEleCollKey.initialize() );
   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::PixelClusterContainer* PixelClusterContainerCnv_p0::createTransient(PixelClusterContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<InDet::PixelClusterContainer> trans(std::make_unique<InDet::PixelClusterContainer>(m_pixId->wafer_hash_max()) );
  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    log << MSG::FATAL << m_pixelDetEleCollKey.fullKey() << " is not available." << endmsg;
    return trans.release();
  }

  for (InDet::PixelClusterCollection* dcColl : *persObj) {
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::SiDetectorElement * de = elements->getDetectorElement(collHash);
      MSG_DEBUG(log,"Set PixelCluster detector element to "<< de);

      InDet::PixelClusterCollection::iterator itColl   = dcColl->begin();
      InDet::PixelClusterCollection::iterator lastColl = dcColl->end();
      for (int num = 0; itColl != lastColl; ++itColl, ++num) {
	 MSG_DEBUG(log,"PRD " << num);
         (*itColl)->m_detEl = de;
      }

      StatusCode sc= trans? trans->addCollection(dcColl, collHash): StatusCode::FAILURE;
      if (sc.isSuccess()){
	 MSG_VERBOSE(log,"PixelClusterContainer successfully added to Container !");
      } else {
         log << MSG::ERROR << "Failed to add PixelClusterContainer to container" << endmsg;
         return nullptr;
      }
   }
  return trans.release();
}

