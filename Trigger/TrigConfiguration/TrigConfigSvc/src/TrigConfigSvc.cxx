/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigConfigSvc.h"

TrigConf::TrigConfigSvc::TrigConfigSvc(const std::string &name, ISvcLocator *pSvcLocator) :
  base_class(name, pSvcLocator)
{
}


StatusCode
TrigConf::TrigConfigSvc::initialize() {

  ATH_MSG_INFO("Initializing LVL1ConfigSvc and HLTConfigSvc to write L1 bunchgroups, L1 menu and HLT menu to DetStore.");
  ATH_CHECK(m_lvl1Svc.retrieve());
  ATH_CHECK(m_hltSvc.retrieve());

  return StatusCode::SUCCESS;
}


