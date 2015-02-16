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

#include "ByteStreamData/ByteStreamMetadataContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include <set>

using namespace AthPoolEx;

//___________________________________________________________________________
ReadBs::ReadBs(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), p_SGinMeta("StoreGateSvc/InputMetaDataStore", name), p_SGmeta("StoreGateSvc/MetaDataStore", name) {
}
//___________________________________________________________________________
ReadBs::~ReadBs() {
}
//___________________________________________________________________________
StatusCode ReadBs::initialize() {
   ATH_MSG_INFO("in initialize()");

   // Locate the StoreGateSvc and initialize our local ptr
   if (!p_SGinMeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find Input MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }
   if (!p_SGmeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadBs::execute() {
   ATH_MSG_DEBUG("in execute()");
   if (p_SGinMeta->contains<ByteStreamMetadata>("ByteStreamMetadata")) {
      const DataHandle<ByteStreamMetadata> bsmd;
      if (p_SGinMeta->retrieve(bsmd, "ByteStreamMetadata").isFailure()) {
         ATH_MSG_FATAL("Could not find ByteStreamMetadata");
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("ByteStreamMetadata, run# = " << bsmd->getRunNumber() << ", #events = " << bsmd->getNumEvents());
   }
   if (p_SGmeta->contains<ByteStreamMetadataContainer>("ByteStreamMetadata")) {
      const DataHandle<ByteStreamMetadataContainer> bsmdc;
      if (p_SGmeta->retrieve(bsmdc, "ByteStreamMetadata").isFailure()) {
         ATH_MSG_FATAL("Could not find ByteStreamMetadataContainer");
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("ByteStreamMetadataContainer, size =  " << bsmdc->size());
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadBs::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
