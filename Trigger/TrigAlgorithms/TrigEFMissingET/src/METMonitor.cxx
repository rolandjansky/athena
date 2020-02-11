/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
#include "TrigEFMissingET/METMonitor.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigInterfaces/HLTCheck.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

namespace {
  float signedLog(float x) {
    float EPSILON = 1.189 / Gaudi::Units::GeV;
    if (fabs(x) < EPSILON)
      return 0;
    return copysign(log10(fabs(x / Gaudi::Units::GeV)), x);
  }
} //> end anonymous namespace

namespace HLT { namespace MET {
  METMonitor::METMonitor(Algo* parent) :
    AthMessaging(parent->msg() ),
    m_status(32, 0)
  {
    parent->declareMonitoredVariable("EF_MEx_log", m_MExLog);
    parent->declareMonitoredVariable("EF_MEy_log", m_MEyLog);
    parent->declareMonitoredVariable("EF_MEz_log", m_MEzLog);
    parent->declareMonitoredVariable("EF_MET_log", m_METLog);
    parent->declareMonitoredVariable("EF_ME_log", m_MELog);
    parent->declareMonitoredVariable("EF_SumEt_log", m_SumEtLog);
    parent->declareMonitoredVariable("EF_SumE_log", m_SumELog);

    parent->declareMonitoredVariable("EF_MEx_lin", m_MExLin);
    parent->declareMonitoredVariable("EF_MEy_lin", m_MEyLin);
    parent->declareMonitoredVariable("EF_MEz_lin", m_MEzLin);
    parent->declareMonitoredVariable("EF_MET_lin", m_METLin);
    parent->declareMonitoredVariable("EF_ME_lin", m_MELin);
    parent->declareMonitoredVariable("EF_SumEt_lin", m_SumEtLin);
    parent->declareMonitoredVariable("EF_SumE_lin", m_SumELin);

    parent->declareMonitoredVariable("EF_MET_phi", m_phi);
    parent->declareMonitoredStdContainer("EF_MET_status", m_status);
  }

  void METMonitor::reset()
  {
      m_MExLog = -9e9;
      m_MEyLog = -9e9;
      m_MEzLog = -9e9;
      m_METLog = -9e9;
      m_MELog = -9e9;
      m_SumEtLog = -9e9;
      m_SumELog = -9e9;
      m_MExLin = -9e9;
      m_MEyLin = -9e9;
      m_MEzLin = -9e9;
      m_METLin = -9e9;
      m_MELin = -9e9;
      m_SumEtLin = -9e9;
      m_SumELin = -9e9;
      m_phi = -9e9;

      m_status.assign(32, 0);
  }

  HLT::ErrorCode METMonitor::monitor(const xAOD::TrigMissingET& met)
  {
    METComponent total(met);
    // Fill the total MET variables
    m_MExLin = total.mpx / Gaudi::Units::GeV;
    m_MEyLin = total.mpy / Gaudi::Units::GeV;
    m_MEzLin = total.mpz / Gaudi::Units::GeV;
    m_METLin = total.met() / Gaudi::Units::GeV;
    m_MELin = total.magnitude() / Gaudi::Units::GeV;
    m_SumEtLin = total.sumEt / Gaudi::Units::GeV;
    m_SumELin = total.sumE / Gaudi::Units::GeV;
    m_phi = total.phi();

    m_MExLog = signedLog(total.mpx);
    m_MEyLog = signedLog(total.mpy);
    m_MEzLog = signedLog(total.mpz);
    m_SumEtLog = signedLog(total.sumEt);
    m_SumELog = signedLog(total.sumE);
    if (total.magnitude() > 1 / Gaudi::Units::keV)
      m_MELog = log10(total.magnitude() / Gaudi::Units::GeV);
    if (total.met() > 1 / Gaudi::Units::keV)
      m_METLog = log10(total.met() / Gaudi::Units::GeV);

    for (std::size_t ii = 0; ii < m_status.size(); ++ii)
      if (total.status & (1 << ii) )
        m_status.at(ii) = 1;
      else
        m_status.at(ii) = 0;
    return HLT::OK;
  }

  METComponentMonitor::METComponentMonitor(Algo* parent) :
    METMonitor(parent)
  {
    parent->declareMonitoredStdContainer("compN", m_compIndex);
    parent->declareMonitoredStdContainer("compEx", m_compExLog);
    parent->declareMonitoredStdContainer("compEy", m_compEyLog);
    parent->declareMonitoredStdContainer("compEz", m_compEzLog);
    parent->declareMonitoredStdContainer("compEt", m_compEtLog);
    parent->declareMonitoredStdContainer("compSumEt", m_compSumEtLog);
    parent->declareMonitoredStdContainer("compSumE", m_compSumELog);
    parent->declareMonitoredStdContainer("compEt_lin", m_compEtLin);
    parent->declareMonitoredStdContainer("compSumEt_lin", m_compSumEtLin);
    parent->declareMonitoredStdContainer("compStatus", m_compStatus);

  }

  void METComponentMonitor::reset()
  {
    METMonitor::reset();
    m_compIndex.clear();
    m_compExLog.clear();
    m_compEyLog.clear();
    m_compEzLog.clear();
    m_compEtLog.clear();
    m_compSumEtLog.clear();
    m_compSumELog.clear();
    m_compEtLin.clear();
    m_compSumEtLin.clear();
    m_compStatus.clear();

  }

  HLT::ErrorCode METComponentMonitor::monitor(const xAOD::TrigMissingET& met)
  {
    HLT_CHECK( METMonitor::monitor(met) );
    for (std::size_t ii = 0; ii < met.getNumberOfComponents(); ++ii) {
      METComponent comp(ii, met);
      m_compIndex.push_back(ii);
      m_compExLog.push_back(signedLog(comp.mpx) );
      m_compEyLog.push_back(signedLog(comp.mpy) );
      m_compEzLog.push_back(signedLog(comp.mpz) );
      m_compEtLog.push_back(comp.met() > 1 / Gaudi::Units::keV
          ? log10(comp.met() / Gaudi::Units::GeV)
          : -9e9);
      m_compSumEtLog.push_back(signedLog(comp.sumEt) );
      m_compSumELog.push_back(signedLog(comp.sumE) );
      m_compEtLin.push_back(comp.met() / Gaudi::Units::GeV);
      m_compSumEtLin.push_back(comp.sumEt / Gaudi::Units::GeV);
      m_compStatus.push_back(comp.status);
    }
    return HLT::OK;
  }
} } //> end namespace HLT::MET
