/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSZWINDOWFILTER_H
#define GENERATORFILTERSZWINDOWFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filter on dileptons within a Z mass window
///
/// - Apply Pt and Eta cuts on leptons.  Default is Pt > 5 GeV and |eta| < 5
/// - Optionally allow same sign paris.  Default is true
/// - Optionally allow Z -> e mu.  Default is false
///
/// @author Carl Gwilliam <gwilliam@mail.cern.ch>
class DiLeptonMassFilter : public GenFilter {
public:

  DiLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  double m_minPt;
  double m_maxEta;
  double m_minMass;
  double m_maxMass;
  double m_minDilepPt;
  bool   m_allowElecMu;
  bool   m_allowSameCharge;

  int m_AthenaCalls;
};

#endif
