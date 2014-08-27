/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTER_FOUREPTONMASSFILTER
#define GENERATORFILTER_FOUREPTONMASSFILTER

#include "GeneratorModules/GenFilter.h"

/// Filter on two dilepton-pairs with two mass windows (by default >60GeV and >12GeV)
///
/// - Apply Pt and Eta cuts on leptons.  Default is Pt > 5 GeV and |eta| < 3
/// - Optionally allow same sign pairs.  Default is true
/// - Optionally allow emu pairs.  Default is true
///
/// @author Theodota Lagouri Theodota Lagouri <theodota.lagouri@cern.ch>
/// @author Konstantinos Nikolopoulos <konstantinos.nikolopoulos@cern.ch>
class FourLeptonMassFilter : public GenFilter {
public:

  FourLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_minPt;
  double m_maxEta;
  double m_minMass1;
  double m_maxMass1;
  double m_minMass2;
  double m_maxMass2;
  bool   m_allowElecMu;
  bool   m_allowSameCharge;

};

#endif
