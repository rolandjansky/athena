/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigationThinningSvc.h"



TrigNavigationThinningSvc::TrigNavigationThinningSvc( const std::string& name, ISvcLocator* pSvcLocator ) 
  : base_class(name, pSvcLocator),
    m_slimmingTool("TrigNavigationSlimmingTool/TrigNavigationSlimmingTool")
{
  declareProperty("SlimmingTool", m_slimmingTool, "Tool responsible for the actual thinning");
}


StatusCode TrigNavigationThinningSvc::initialize() {
  CHECK( m_slimmingTool.retrieve() );
  return StatusCode::SUCCESS;
}


