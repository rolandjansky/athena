/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODZWINDOWFILTER_H
#define GENERATORFILTERS_XAODZWINDOWFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"


/// Filter on dileptons within a Z mass window
///
/// - Apply Pt and Eta cuts on leptons.  Default is Pt > 5 GeV and |eta| < 5
/// - Optionally allow same sign paris.  Default is true
/// - Optionally allow Z -> e mu.  Default is false
///
/// @author Carl Gwilliam <gwilliam@mail.cern.ch>
class xAODDiLeptonMassFilter : public GenFilter {
public:

  xAODDiLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator);
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

#endif //GENERATORFILTERS_XAODZWINDOWFILTER_H
