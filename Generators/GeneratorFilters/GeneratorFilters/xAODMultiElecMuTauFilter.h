/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODMULTIELECMUTAUFILTER_H
#define GENERATORFILTERS_XAODMULTIELECMUTAUFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

/// Select multiple charged leptons taking into account electrons, muons and taus (inc. hadronic decays).
///
///  The user can steer the maximum |eta| and minimum pt and there is a separate
/// (visible) pt cut for hadronic taus.
///
/// @author Carl Gwilliam
class xAODMultiElecMuTauFilter : public GenFilter {
public:

  xAODMultiElecMuTauFilter(const std::string& name, ISvcLocator* pSvcLocator);
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
