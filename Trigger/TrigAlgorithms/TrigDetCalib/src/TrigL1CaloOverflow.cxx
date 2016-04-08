/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1CaloOverflow.h"
#include "TrigT1Result/RoIBResult.h"

#include <bitset>

TrigL1CaloOverflow::TrigL1CaloOverflow(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator),
  m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this)
{
}

HLT::ErrorCode TrigL1CaloOverflow::hltInitialize()
{
  if (m_lvl1Tool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_lvl1Tool);
    return HLT::BAD_JOB_SETUP;
  }
  return HLT::OK;
}

HLT::ErrorCode TrigL1CaloOverflow::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/, unsigned int output)
{
  beforeExecMonitors().ignore();

  const ROIB::RoIBResult* result;
  if (evtStore()->retrieve(result).isFailure()) return HLT::OK;

  std::bitset<3> overflow = m_lvl1Tool->lvl1EMTauJetOverflow(*result);
  if (overflow.any()) {
    HLT::TriggerElement* te = addRoI(output);
    te->setActiveState(true);
    ATH_MSG_DEBUG("Event was force accepted by L1Calo due to TOB overflows in CMX transmission");
  }
  
  afterExecMonitors().ignore();
  return HLT::OK;
}
