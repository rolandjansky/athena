/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "L1Decoder/CTPUnpackingToolBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


CTPUnpackingToolBase::CTPUnpackingToolBase(const std::string& type, 
                                           const std::string& name, 
                                           const IInterface* parent) 
  : base_class(type, name, parent)
{
}



StatusCode CTPUnpackingToolBase::initialize() 
{
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode CTPUnpackingToolBase::passBeforePrescaleSelection(const ROIB::RoIBResult* /*roib*/, const std::vector<std::string>& /*l1ItemNames*/, bool& /*pass*/) const {
  return StatusCode::SUCCESS;
}
