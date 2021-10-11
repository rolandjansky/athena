/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "EndOfEventROIConfirmerAlg.h"


EndOfEventROIConfirmerAlg::EndOfEventROIConfirmerAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {}


StatusCode EndOfEventROIConfirmerAlg::initialize() {
  ATH_MSG_DEBUG( "EndOfEventROIConfirmerAlg::initialize()" );
  ATH_CHECK( m_writeHandleKeyArray_ROIs.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode EndOfEventROIConfirmerAlg::execute(const EventContext& context) const {
  ATH_MSG_DEBUG( "EndOfEventROIConfirmerAlg::execute()" );

  for (const auto& whk : m_writeHandleKeyArray_ROIs) {
    SG::ReadHandle<TrigRoiDescriptorCollection> readHandle( whk.key() );
    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << whk.key() << " already present - this chain must have run as part of the trigger in this event" );
    } else {
      ATH_MSG_DEBUG( "The " << whk.key() << " is not here - we should create it such that we can run algorithms at the end of the HLT-accepted event" );
    
      auto handle = SG::makeHandle(whk, context);
      auto objp = std::make_unique<TrigRoiDescriptorCollection> (TrigRoiDescriptorCollection());
      ATH_CHECK( handle.record (std::move (objp)) );
      handle->push_back(new TrigRoiDescriptor(true));
    }
  }
  return StatusCode::SUCCESS;
}
