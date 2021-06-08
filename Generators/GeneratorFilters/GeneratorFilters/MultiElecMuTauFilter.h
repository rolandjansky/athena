/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MULTIELECMUTAUFILTER_H
#define GENERATORFILTERS_MULTIELECMUTAUFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Select multiple charged leptons taking into account electrons, muons and taus (inc. hadronic decays).
///
///  The user can steer the maximum |eta| and minimum pt and there is a separate
/// (visible) pt cut for hadronic taus.
///
/// @author Carl Gwilliam
class MultiElecMuTauFilter : public GenFilter {
public:

  MultiElecMuTauFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_minPt;
  double m_maxEta;
  double m_minVisPtHadTau;
  int    m_NLeptons;
  bool   m_incHadTau;
  bool   m_TwoSameSignLightLeptonsOneHadTau;
};

#endif
