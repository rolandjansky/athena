/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigiTestToolBase.h"

DigiTestToolBase::DigiTestToolBase(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent) : 
  AthAlgTool(type, name, parent)
{
  declareProperty("HistPath",  m_path="/truth/");
  declareProperty("McEventKey", m_key="TruthEvent");
}

StatusCode
DigiTestToolBase::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IDigiTestTool::interfaceID() ) {
    *ppvIf = (IDigiTestTool*)this; 
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}
