/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARPLUSJETSFILTER_H
#define GENERATORFILTERS_TTBARPLUSJETSFILTER_H

#include "GeneratorModules/GenFilter.h"

class TTbarPlusJetsFilter : public GenFilter {
public:

  TTbarPlusJetsFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  /// Location of jet container in TES
  std::string m_jetContainerLoc;

  double m_ptMinJet;
  double m_etaMaxJet;
  int    m_nbJet;
  double m_ptMinJetB;
  double m_etaMaxJetB;
  int    m_nbJetB;
  double m_ptMinElec;
  double m_etaMaxElec;
  double m_ptMinMuon;
  double m_etaMaxMuon;
  int    m_nbLepton;
  bool   m_selectLepHad;
  int    m_nbEventMax;
  int    m_nbEventProcessed;
  int    m_nbEventKept;
  int    m_nbJetDiscarded; // nb of jets removed because potential electron
  int    m_nbEventWithJetDiscarded;

};

#endif
