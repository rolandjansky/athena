/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GeneratorFilters/TTbarWithJpsimumuFilter.h
 * @author Frédéric Derue
 * @date Jan. 2018
 * @brief filter to select ttbar with Jpsi->mumu events
 */


#ifndef GeneratorFilters_TTbarWithJpsimumuFilter_H
#define GeneratorFilters_TTbarWithJpsimumuFilter_H

#include "GeneratorModules/GenFilter.h"

namespace HepMC{
  class GenParticle;
}

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

  MsgStream m_log;

  // method to check if Jpsi decays into pair of leptons
  bool isLeptonDecay(const HepMC::GenParticle* part, int type) const;  

  // method to check if Jpsi pass some selection criteria
  bool passJpsiSelection(const HepMC::GenParticle* part) const;

};

#endif /// GeneratorFilters_TTbarWithJpsimumuFilter_H
