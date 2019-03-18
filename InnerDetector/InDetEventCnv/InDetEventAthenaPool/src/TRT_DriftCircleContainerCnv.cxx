/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DriftCircleContainerCnv.h"
#include "InDetIdentifier/TRT_ID.h"

#include <memory>

#include <iostream>

  TRT_DriftCircleContainerCnv::TRT_DriftCircleContainerCnv (ISvcLocator* svcloc)
    : TRT_DriftCircleContainerCnvBase(svcloc, "TRT_DriftCircleContainerCnv"),
      m_converter_p0(),
      m_storeGate(nullptr)
  {}


StatusCode TRT_DriftCircleContainerCnv::initialize() {
   ATH_MSG_INFO("TRT_DriftCircleContainerCnv::initialize()");

   StatusCode sc = TRT_DriftCircleContainerCnvBase::initialize();
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
   
   // Get the trt helper from the detector store
   const TRT_ID* idhelper(0);
   if (detStore->retrieve(idhelper, "TRT_ID").isFailure()) {
     ATH_MSG_FATAL("Could not get TRT_ID helper !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found the TRT_ID helper.");
   }
   
   if (m_converter_p0.initialize(msg()).isFailure())
   {
     ATH_MSG_FATAL("Could not initialize converter!");
     return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}


InDet::TRT_DriftCircleContainer* TRT_DriftCircleContainerCnv::createTransient() {

  const pool::Guid   p0_guid("A99630C5-3D7C-4DB4-9E6C-FC3CEF981895"); // before t/p split
  const pool::Guid   p1_guid("42B48D79-AF4E-4D45-AAA9-A2BA5A033534"); // with TRT_DriftCircle_tlp1
  const pool::Guid   p2_guid("36195EDE-941C-424B-81A1-E04C867C35D8"); // with TRT_DriftCircle_p2
  ATH_MSG_DEBUG("createTransient(): main converter");
  InDet::TRT_DriftCircleContainer* p_collection(0);
  if( compareClassGuid(p2_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");  
    std::unique_ptr< InDet::TRT_DriftCircleContainer_p2 >   col_vect( poolReadObject< InDet::TRT_DriftCircleContainer_p2 >() );
    ATH_MSG_DEBUG("Delegate TP converter ");
    p_collection = m_TPConverter2.createTransient( col_vect.get(), msg() );
  }
  else if( compareClassGuid(p1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::unique_ptr< InDet::TRT_DriftCircleContainer_tlp1 >  p_coll( poolReadObject< InDet::TRT_DriftCircleContainer_tlp1 >() );
    p_collection = m_TPConverter.createTransient( p_coll.get(), msg() );
  }
  //----------------------------------------------------------------
  else if( compareClassGuid(p0_guid) ) {
    ATH_MSG_DEBUG("createTransient(): Old input file");
    std::unique_ptr< TRT_DriftCircleContainer_p0 >   col_vect( poolReadObject< TRT_DriftCircleContainer_p0 >() );
    p_collection = m_converter_p0.createTransient( col_vect.get(), msg() );
  }
  else {
     throw std::runtime_error("Unsupported persistent version of TRT_DriftCircleContainer");

  }
  return p_collection;
}


TRT_DriftCircleContainer_PERS*    TRT_DriftCircleContainerCnv::createPersistent (InDet::TRT_DriftCircleContainer* transCont) {
   TRT_DriftCircleContainer_PERS *trtdc_p= m_TPConverter2.createPersistent( transCont, msg() );
   return trtdc_p;
}


