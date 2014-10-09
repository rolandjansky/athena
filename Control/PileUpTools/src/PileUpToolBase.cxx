/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpToolBase.h"

PileUpToolBase::PileUpToolBase(const std::string& type,
                               const std::string& name,
                               const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_firstXing(-999),
  m_lastXing(999),
  m_filterPassed(true)
{
  declareProperty("FirstXing", m_firstXing);
  declareProperty("LastXing", m_lastXing);
}

StatusCode
PileUpToolBase::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IPileUpTool::interfaceID() ) {
    *ppvIf = (IPileUpTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}
