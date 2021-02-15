/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTResultMTMakerAlg.h"

HLTResultMTMakerAlg::HLTResultMTMakerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode HLTResultMTMakerAlg::initialize() {
  ATH_CHECK( m_resultMaker.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::finalize() {
  ATH_CHECK( m_resultMaker.release() );
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::execute(const EventContext& context) const {
  return m_resultMaker->makeResult(context);
}
