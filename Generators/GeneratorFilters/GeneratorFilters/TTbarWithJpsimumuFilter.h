/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GeneratorFilters/TTbarWithJpsimumuFilter.h
 * @author Fr??d??ric Derue
 * @date Jan. 2018
 * @brief filter to select ttbar with Jpsi->mumu events
 */


#ifndef GeneratorFilters_TTbarWithJpsimumuFilter_H
#define GeneratorFilters_TTbarWithJpsimumuFilter_H

#include "GeneratorModules/GenFilter.h"


class TTbarWithJpsimumuFilter: public GenFilter {
 public:
  TTbarWithJpsimumuFilter(const std::string& fname, ISvcLocator* pSvcLocator);
  virtual ~TTbarWithJpsimumuFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

 private:

  /// properties
  bool m_selectJpsi;
  //bool m_useMuonDecay;
  //bool m_useElectronDecay;
  double m_JpsiPtMinCut;
  double m_JpsiEtaMaxCut;

  // method to check if Jpsi decays into pair of leptons
  bool isLeptonDecay(HepMC::ConstGenParticlePtr part, int type) const;  

  // method to check if Jpsi pass some selection criteria
  bool passJpsiSelection(HepMC::ConstGenParticlePtr part) const;

};

#endif /// GeneratorFilters_TTbarWithJpsimumuFilter_H
