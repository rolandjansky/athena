/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ReadBs.cxx
 *  @brief This file contains the implementation for the ReadBs class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadBs.cxx,v 1.26 2009-03-14 22:05:57 gemmeren Exp $
 **/

#include "ReadBs.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"

#include "GaudiKernel/MsgStream.h"

#include "ByteStreamData/ByteStreamMetadataContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include <set>

using namespace AthPoolEx;

//___________________________________________________________________________
ReadBs::ReadBs(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), p_SGinMeta("StoreGateSvc/InputMetaDataStore", name), p_SGmeta("StoreGateSvc/MetaDataStore", name) {
}
//___________________________________________________________________________
ReadBs::~ReadBs() {
}
//___________________________________________________________________________
StatusCode ReadBs::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = p_SGinMeta.retrieve();
   if (!sc.isSuccess() || 0 == p_SGinMeta) {
      log << MSG::ERROR << "Could not find Input MetaData StoreGateSvc" << endreq;
   }
   sc = p_SGmeta.retrieve();
   if (!sc.isSuccess() || 0 == p_SGmeta) {
      log << MSG::ERROR << "Could not find MetaData StoreGateSvc" << endreq;
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode ReadBs::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;
   if (p_SGinMeta->contains<ByteStreamMetadata>("ByteStreamMetadata")) {
      const DataHandle<ByteStreamMetadata> bsmd;
      StatusCode sc = p_SGinMeta->retrieve(bsmd, "ByteStreamMetadata");
      if (sc.isFailure() || !bsmd) {
         log << MSG::FATAL << "Could not find ByteStreamMetadata" << endreq;
         return(StatusCode::FAILURE);
      }
      log << MSG::INFO << "ByteStreamMetadata, run# = " << bsmd->getRunNumber() << ", #events = " << bsmd->getNumEvents() << endreq;
   }
   if (p_SGmeta->contains<ByteStreamMetadataContainer>("ByteStreamMetadata")) {
      const DataHandle<ByteStreamMetadataContainer> bsmdc;
      StatusCode sc = p_SGmeta->retrieve(bsmdc, "ByteStreamMetadata");
      if (sc.isFailure() || !bsmdc) {
         log << MSG::FATAL << "Could not find ByteStreamMetadataContainer" << endreq;
         return(StatusCode::FAILURE);
      }
      log << MSG::INFO << "ByteStreamMetadataContainer, size =  " << bsmdc->size() << endreq;
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReadBs::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
