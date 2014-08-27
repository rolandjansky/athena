/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARBOOSTCATFILTER_H
#define GENERATORFILTERS_TTBARBOOSTCATFILTER_H

#include "GeneratorModules/GenFilter.h"

/// This is test class in order to have high pT region in ttbar events
/// Modified from TTBarWToLeptonFilter 
/// TTBarWToLeptonFilter author Gia Khoriauli, June 2008
/// TTBarWToLeptonFilter author Andy Buckley, extension to specific lepton multiplicities, April 2012
/// Modified by Yuji Enari, Dec 2013

/// Events that do not contain t AND t bar quarks are rejected.
/// Only tops decaying to W X are analyzed and counted in this algorithm.
class TTbarBoostCatFilter : public GenFilter {
public:

  TTbarBoostCatFilter(const std::string& name, ISvcLocator* pSvcLocator);

  ~TTbarBoostCatFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  double m_Ptmin;
  int m_numLeptons;

  double m_TopPtmin;
  double m_TopPtmax;
  double m_WPtmin;
  double m_WPtmax;
  double m_TTMassmin;
  double m_TTMassmax;
  double m_LepsPtmin;
  double m_LepsPtmax;
  double m_NusPtmin;
  double m_NusPtmax;
  double m_LepPtmin;
  double m_LepPtmax;


};

#endif
