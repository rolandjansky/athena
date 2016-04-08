/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Reldis_i/Reldis_i.h"

Reldis_i::Reldis_i(const std::string& name, ISvcLocator* pSvcLocator) : 
  GenModule(name, pSvcLocator),
  m_reldisRootFileName("_cwn.root"),
  m_reldisTreeName("h2032")
{
  declareProperty("ReldisRootFileName", m_reldisRootFileName);
  declareProperty("ReldisTreeName",     m_reldisTreeName);
  
  m_eventCounter = 0;
}

Reldis_i::~Reldis_i() {}

StatusCode Reldis_i::genInitialize() {
  
  ATH_MSG_INFO("Reldis_i::genInitialize");
  
  m_reldisTree.Init(m_reldisRootFileName, m_reldisTreeName); 
  
  ATH_MSG_INFO("RELDIS events: " << m_reldisTree.fChain->GetEntriesFast());
  
  return StatusCode::SUCCESS;
}

StatusCode Reldis_i::callGenerator() {
  
  ATH_MSG_DEBUG("Reldis_i::callGenerator");  
  
  int retCode = m_reldisTree.LoadTree(m_eventCounter);
  
  if (retCode<0) { 
    
    ATH_MSG_INFO("Problem with TChain.LoadTree, return code: " << retCode << ", m_eventCounter: " << m_eventCounter);
    
    return StatusCode::FAILURE;
  }
  
  m_reldisTree.GetEntry(m_eventCounter);
  
  return StatusCode::SUCCESS;
}

StatusCode Reldis_i::fillEvt(GenEvent* evt) { 
  
  ATH_MSG_DEBUG("Reldis_i::fillEvt. EventCounter: " << m_eventCounter);
  
  m_reldisToHepMC.fill_next_event(m_reldisTree, evt, m_eventCounter);
  
  m_eventCounter++;
  
  return StatusCode::SUCCESS;
}

StatusCode Reldis_i::genFinalize() {

  ATH_MSG_INFO("Reldis_i::genFinalize");
  
  return StatusCode::SUCCESS;
}
