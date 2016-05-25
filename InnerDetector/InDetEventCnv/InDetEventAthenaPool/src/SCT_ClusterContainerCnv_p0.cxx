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
#include "AthenaKernel/errorcheck.h"

#include <string>
#include <iostream>
#include <sstream>

//================================================================


StatusCode SCT_ClusterContainerCnv_p0::initialize(MsgStream &log ) {

   ISvcLocator* svcLocator = Gaudi::svcLocator();

   // Get the messaging service, print where you are
   log << MSG::INFO << "SCT_ClusterContainerCnv::initialize()" << endreq;

   StoreGateSvc *detStore = nullptr;
   CHECK( svcLocator->service("DetectorStore", detStore) );
   CHECK( detStore->retrieve(m_sctId, "SCT_ID") );
   CHECK( detStore->retrieve(m_sctMgr) );
   MSG_DEBUG(log,"Converter initialized.");

   return StatusCode::SUCCESS;
}




InDet::SCT_ClusterContainer* SCT_ClusterContainerCnv_p0::createTransient(SCT_ClusterContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<InDet::SCT_ClusterContainer> trans(new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max()) );
  //  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());
  
  for (InDet::SCT_ClusterCollection* dcColl : *persObj) {
      // Add detElem to each drift circle
      IdentifierHash collHash = dcColl->identifyHash();
      const InDetDD::SiDetectorElement * de = m_sctMgr->getDetectorElement(collHash);
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
         log << MSG::ERROR << "Failed to add SCT_ClusterContainer to container" << endreq;
         return 0;
      }
   }
  return(trans.release());
}

