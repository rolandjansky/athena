/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "CTPUnpackingToolBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


CTPUnpackingToolBase::CTPUnpackingToolBase(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : base_class(type, name, parent) {}


StatusCode CTPUnpackingToolBase::initialize()
{
  if ( !m_monTool.empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }

  return StatusCode::SUCCESS;
}


StatusCode CTPUnpackingToolBase::passBeforePrescaleSelection(const ROIB::RoIBResult* /*roib*/,
                                                             const std::vector<std::string>& /*l1ItemNames*/,
                                                             bool& /*pass*/) const {
  ATH_MSG_ERROR("Base class should not call this function");
  return StatusCode::FAILURE;
}


StatusCode CTPUnpackingToolBase::decode(const ROIB::RoIBResult& /*roib*/, HLT::IDVec& /*enabledChains*/) const {
  ATH_MSG_ERROR("Base class should not call this function");
  return StatusCode::FAILURE;
}
