/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TEHypo.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigInterfaces/HLTCheck.h"

namespace HLT { namespace MET {

  TEHypo::TEHypo(const std::string& name, ISvcLocator* pSvcLocator) :
    HypoAlgo(name, pSvcLocator),
    m_monitor(this)
  {
    declareProperty("METLabel", m_label, "The feature label to select on.");
    declareProperty("Threshold", m_cut, "The threshold to select");
  }

  HLT::ErrorCode TEHypo::hltInitialize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TEHypo::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TEHypo::hltExecute(
      const HLT::TriggerElement* outputTE, bool& pass)
  {
    // reset the monitor
    m_monitor.reset();
    const xAOD::TrigMissingET* met = nullptr;
    HLT_CHECK(getFeature(outputTE, met, m_label) );
    // TODO(?) allow rejection based on status flag. This was implemented in the
    // old code but not used.
    pass = met->sumEt() > m_cut;
    // Fill monitoring variables only if passed.
    if (pass)
      HLT_CHECK( m_monitor.monitor(*met) );

    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("REGTEST Event " << (pass ? "accepted" : "rejected") );
      ATH_MSG_DEBUG("REGTEST " << METComponent(*met) );
    }
    return HLT::OK;
  }


} } //> end namespace HLT::MET
