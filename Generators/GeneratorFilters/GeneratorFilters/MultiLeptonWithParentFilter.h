/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERS_MULTILEPTONWITHPARENTFILTER_H
#define GENERATORFILTERS_MULTILEPTONWITHPARENTFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Select multiple charged leptons taking into account electrons, muons and taus (inc. hadronic decays) originating from the specified parent(s).
///
///  The user can steer the maximum |eta| and minimum pt.
///  Additionally leptonic and hadronic tau decays can be filtered.
///
/// @author Tadej Novak
class MultiLeptonWithParentFilter : public GenFilter {
public:

  MultiLeptonWithParentFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  bool hasValidParent(HepMC::GenParticle *particle);

  std::vector<int> m_PDGOrigin;
  std::vector<int> m_PDGIntermediate;
  double m_minPt;
  double m_maxEta;
  int    m_NLeptonsMin;
  int    m_NLeptonsMax;
  bool   m_incLepTau;
  bool   m_incHadTau;
  bool   m_vetoHadTau;

};

#endif
