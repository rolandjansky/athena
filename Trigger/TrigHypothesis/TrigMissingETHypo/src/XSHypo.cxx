/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "XSHypo.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigInterfaces/HLTCheck.h"

namespace HLT { namespace MET {

  XSHypo::XSHypo(const std::string& name, ISvcLocator* pSvcLocator) :
    HypoAlgo(name, pSvcLocator),
    m_monitor(this)
  {
    declareProperty("METLabel", m_label, "The feature label to select on.");
    declareProperty("Threshold", m_cut, "The threshold to select");
    declareProperty("OffsetTerm", m_xsOffset = 0,
        "The offset term");
    declareProperty("LinearTerm", m_xsLinear = 1,
        "The linear term");
    declareProperty("QuadraticTerm", m_xsQuadratic = 0,
        "The quadratic term");
    declareProperty("MinimumMET", m_minMET = 10*Gaudi::Units::GeV,
        "MET value below which we always fail the event");
    declareProperty("MinimumSumEt", m_minSumEt = 16*Gaudi::Units::GeV,
        "SumEt value below which we always fail the event");
    // This very much needs to change... it's way too low
    declareProperty("METCeiling", m_maxMET = 64*Gaudi::Units::GeV,
        "MET value above which we always pass the event");
    declareProperty("SumEtCeiling", m_maxSumEt = 6.5*Gaudi::Units::TeV,
        "SumEt value above which we always pass the event");
  }

  HLT::ErrorCode XSHypo::hltInitialize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode XSHypo::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode XSHypo::hltExecute(
      const HLT::TriggerElement* outputTE, bool& pass)
  {
    m_monitor.reset();
    const xAOD::TrigMissingET* met = nullptr;
    HLT_CHECK(getFeature(outputTE, met, m_label) );
    // TODO(?) allow rejection based on status flag. This was implemented in the
    // old code but not used.
    double metValue = sqrt(met->ex()*met->ex()+met->ey()*met->ey() );
    double sumEtValue = met->sumEt();
    double significance = 0;
    if (sumEtValue > 0 ) {
      double numerator = metValue/Gaudi::Units::GeV;
      double denominator = m_xsOffset +
        m_xsLinear*sqrt(sumEtValue/Gaudi::Units::GeV) +
        m_xsQuadratic*sumEtValue/Gaudi::Units::GeV;
      significance = numerator/denominator;
    }
    if (metValue < m_minMET || sumEtValue < m_minSumEt)
      pass = false;
    else if (metValue > m_maxMET || sumEtValue > m_maxSumEt)
      pass = true;
    else
      pass = significance > m_cut;

    if (pass) {
      HLT_CHECK( m_monitor.monitor(*met) );
      m_monitor.setXS(significance);
    }

    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("REGTEST Event " << (pass ? "accepted" : "rejected") );
      ATH_MSG_DEBUG("REGTEST " << METComponent(*met) );
      ATH_MSG_DEBUG("REGTEST xs = " << significance);
    }
    return HLT::OK;
  }

  XSHypo::Monitor::Monitor(Algo* parent) :
    METMonitor(parent)
  {
    parent->declareMonitoredVariable("Hypo_XS", m_xs);
    parent->declareMonitoredVariable("Hypo_XS2", m_xs2);
  }

  void XSHypo::Monitor::reset()
  {
    METMonitor::reset();
    m_xs = -9e9;
    m_xs2 = -9e9;
  }

  void XSHypo::Monitor::setXS(float xs)
  {
    m_xs = xs;
    m_xs = xs*xs;
  }

} } //> end namespace HLT::MET
