/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TRUTHJETFILTER_H
#define GENERATORFILTERS_TRUTHJETFILTER_H

#include "GeneratorModules/GenFilter.h"


/// Truth jet filters
/// @author Osamu Jinnouchi, Jan 2006
class TruthJetFilter : public GenFilter {
public:

  TruthJetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_Njet;
  double m_NjetMinPt;
  double m_NjetMaxEta;
  double m_jet_pt1;
  bool   m_applyDeltaPhiCut;
  double m_MinDeltaPhi;

  std::string m_TruthJetContainerName;

};

#endif
