/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1CaloOverflow.h"

#include "TrigT1Result/RoIBResult.h"
#include "xAODEventInfo/EventInfo.h"

#include <bitset>

// BCIDs of the abort gap
const int ABORT_GAP_START = 3446;
const int ABORT_GAP_END   = 3563;

TrigL1CaloOverflow::TrigL1CaloOverflow(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator),
  m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this)
{
  declareProperty("ignoreAbortGap", m_ignoreAbortGap = true, "Ignore overflows in abort gap");
  declareProperty("acceptCMXOverflows", m_acceptCMXOverflows = true, "Accept CMX overflows");
  declareProperty("acceptCaloRoIBOverflows", m_acceptCaloRoIBOverflows = true, "Accept Calo RoIB overflows");
  declareProperty("acceptLvl1ConsistencyProblems", m_acceptLvl1Consist = true, "Accept events with problems in the Lvl1 consistency checker");
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

  // Do nothing for events in the abort gap (tile laser causes overflows)
  if (m_ignoreAbortGap) {
    const xAOD::EventInfo* evt(0);
    if (evtStore()->retrieve(evt).isSuccess() &&
        evt->bcid() >= ABORT_GAP_START && evt->bcid() <= ABORT_GAP_END) {
      return HLT::OK;
    }
  }

  const ROIB::RoIBResult* result;
  if (evtStore()->retrieve(result).isFailure()) return HLT::OK;

  // Overflows in the TOB transmission to CMX
  if (m_acceptCMXOverflows) {
    std::bitset<3> overflow = m_lvl1Tool->lvl1EMTauJetOverflow(*result);
    if (overflow.any()) {
      addRoI(output)->setActiveState(true);
      ATH_MSG_DEBUG("Event was force accepted by L1Calo due to TOB overflows in CMX transmission");
    }
  }

  // Overflows on the RoI link to the RoIB
  if (m_acceptCaloRoIBOverflows) {
    if (result->CheckEMOverflow() || result->CheckJOverflow()) {
      addRoI(output)->setActiveState(true);
      ATH_MSG_DEBUG("Event has overflows in the EM or JET RoI transmission to the RoIB");
    }
  }
  
  // Events with issues in the consistency checker (not strictly overflows, see ATR-14607)
  if (m_acceptLvl1Consist) {
    HLT::ErrorCode ec = config()->getLvlConverterStatus();
    if ( ec.reason()==HLT::Reason::MISSING_FEATURE &&
         ec.steeringInternalReason()==HLT::SteeringInternalReason::MISSING_CALO_ROI ) {   // see Lvl1ConsistencyChecker.cxx
      addRoI(output)->setActiveState(true);
      ATH_MSG_DEBUG("Event has Lvl1 consistency checker problems");
    }
  }

  afterExecMonitors().ignore();
  return HLT::OK;
}
