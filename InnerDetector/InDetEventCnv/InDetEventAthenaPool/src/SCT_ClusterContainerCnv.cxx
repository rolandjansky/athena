/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "SCT_ClusterContainerCnv.h"
#include "InDetIdentifier/SCT_ID.h"
#include "MsgUtil.h"

#include <memory>

#include <iostream>

  SCT_ClusterContainerCnv::SCT_ClusterContainerCnv (ISvcLocator* svcloc) : SCT_ClusterContainerCnvBase(svcloc) {}
  SCT_ClusterContainerCnv::~SCT_ClusterContainerCnv() {}



StatusCode SCT_ClusterContainerCnv::initialize() {
   IDEvtAthPool::setMsgName(this,"SCT_ClusterContainerCnv");//So msg() won't use name "AthenaPoolConverter" 

   ATH_MSG_INFO("SCT_ClusterContainerCnv::initialize()");

   StatusCode sc = SCT_ClusterContainerCnvBase::initialize();
   if (sc.isFailure()) {
     ATH_MSG_FATAL("Cannot initialize cnv base !");
     return StatusCode::FAILURE;
   }

   // get StoreGate service. This is needed only for clients 
   // that register collections directly to the SG instead of adding 
   // them to the container.
   sc = service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
     ATH_MSG_FATAL("StoreGate service not found !");
     return StatusCode::FAILURE;
   }
   
   // get DetectorStore service
   StoreGateSvc *detStore(0);
   if (service("DetectorStore", detStore).isFailure()) {
     ATH_MSG_FATAL("DetectorStore service not found !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found DetectorStore.");
   }
   
   // Get the SCT helper from the detector store
   const SCT_ID* idhelper(0);
   if (detStore->retrieve(idhelper, "SCT_ID").isFailure()) {
     ATH_MSG_FATAL("Could not get SCT_ID helper !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found the SCT_ID helper.");
   }
   
   if (m_converter_p0.initialize(msg()).isFailure())
   {
     ATH_MSG_FATAL("Could not initialize converter!");
     return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}


InDet::SCT_ClusterContainer* SCT_ClusterContainerCnv::createTransient() {
  //  MsgStream log(messageService(), "SCT_ClusterContainerCnv" );
  static pool::Guid   p0_guid("A180F372-0D52-49C3-8AA0-0939CB0B8179"); // before t/p split
  static pool::Guid   p1_guid("657F6546-F5CD-4166-9567-16AD9C96D286"); // with SCT_Cluster_tlp1
  static pool::Guid   p2_guid("ECE7D831-0F31-4E6F-A6BE-2ADDE90083BA"); // with SCT_Cluster_p2

  //ATH_MSG_DEBUG("createTransient(): main converter");
  InDet::SCT_ClusterContainer* p_collection(0);
  if( compareClassGuid(p2_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
    std::auto_ptr< SCT_ClusterContainer_PERS >  p_coll( poolReadObject< SCT_ClusterContainer_PERS >() );
    p_collection = m_TPConverter_p2.createTransient( p_coll.get(), msg() );
   
  } else if( compareClassGuid(p1_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    usingTPCnvForReading( m_TPConverter );
    std::auto_ptr< InDet::SCT_ClusterContainer_tlp1 >  p_coll( poolReadObject< InDet::SCT_ClusterContainer_tlp1 >() );
    p_collection = m_TPConverter.createTransient( p_coll.get(), msg() );
  }
  //----------------------------------------------------------------
  else if( compareClassGuid(p0_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): Old input file");

    std::auto_ptr< SCT_ClusterContainer_p0 >   col_vect( poolReadObject< SCT_ClusterContainer_p0 >() );
    p_collection = m_converter_p0.createTransient( col_vect.get(), msg() );
  }
  else {
     throw std::runtime_error("Unsupported persistent version of SCT_ClusterContainer");

  }
  return p_collection;
}


SCT_ClusterContainer_PERS*    SCT_ClusterContainerCnv::createPersistent (InDet::SCT_ClusterContainer* transCont) {
   SCT_ClusterContainer_PERS *sctdc_p= m_TPConverter_p2.createPersistent( transCont, msg() );
   return sctdc_p;
}


