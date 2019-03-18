/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "PixelClusterContainerCnv.h"
#include "InDetIdentifier/PixelID.h"

#include <memory>

#include <iostream>

  PixelClusterContainerCnv::PixelClusterContainerCnv (ISvcLocator* svcloc)
    : PixelClusterContainerCnvBase(svcloc, "PixelClusterContainerCnv"),
      m_converter_p0(),
      m_storeGate(nullptr)
  {}


StatusCode PixelClusterContainerCnv::initialize() {
   ATH_MSG_INFO("PixelClusterContainerCnv::initialize()");

   StatusCode sc = PixelClusterContainerCnvBase::initialize();
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
   
   // Get the pixel helper from the detector store
   const PixelID* idhelper(0);
   if (detStore->retrieve(idhelper, "PixelID").isFailure()) {
     ATH_MSG_FATAL("Could not get PixelID helper !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found the PixelID helper.");
   }
   
   if (m_converter_p0.initialize(msg()).isFailure())
   {
     ATH_MSG_FATAL("Could not initialize converter!");
     return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}


InDet::PixelClusterContainer* PixelClusterContainerCnv::createTransient() {
  const pool::Guid   p0_guid("37B00A31-EA80-45DF-9A3F-2721EC0F0DA6"); // before t/p split
  const pool::Guid   p1_guid("9DB54746-8C4E-4A56-8B4C-0E5D42905218"); // with PixelCluster_tlp1
  const pool::Guid   p2_guid("DE48E26B-9E03-4EAD-86B9-351AD88D060E"); // with pixelCluster_p2
  const pool::Guid   p3_guid("7BF0F163-B227-434C-86A6-16130E005E6C"); // with pixelCluster_p3
  ATH_MSG_DEBUG("createTransient(): main converter");
  InDet::PixelClusterContainer* p_collection(0);
  if( compareClassGuid(p3_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 3 detected");
    std::unique_ptr< InDet::PixelClusterContainer_p3 >  p_coll( poolReadObject< InDet::PixelClusterContainer_p3 >() );
    p_collection = m_converter_p3.createTransient( p_coll.get(), msg() );
  } else if( compareClassGuid(p2_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
    std::unique_ptr< InDet::PixelClusterContainer_p2 >  p_coll( poolReadObject< InDet::PixelClusterContainer_p2 >() );
    p_collection = m_converter_p2.createTransient( p_coll.get(), msg() );
  } else if( compareClassGuid(p1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::unique_ptr< InDet::PixelClusterContainer_tlp1 >  p_coll( poolReadObject< InDet::PixelClusterContainer_tlp1 >() );
    p_collection = m_TPConverter.createTransient( p_coll.get(), msg() );
  }
  //----------------------------------------------------------------
  else if( compareClassGuid(p0_guid) ) {
    ATH_MSG_DEBUG("createTransient(): Old input file");

    std::unique_ptr< PixelClusterContainer_p0 >   col_vect( poolReadObject< PixelClusterContainer_p0 >() );
    p_collection = m_converter_p0.createTransient( col_vect.get(), msg() );
  }
  else {
     throw std::runtime_error("Unsupported persistent version of PixelClusterContainer");

  }
  return p_collection;
}


PixelClusterContainer_PERS*    PixelClusterContainerCnv::createPersistent (InDet::PixelClusterContainer* transCont) {
   PixelClusterContainer_PERS *pixdc_p= m_converter_p3.createPersistent( transCont, msg() );
   return pixdc_p;
}


