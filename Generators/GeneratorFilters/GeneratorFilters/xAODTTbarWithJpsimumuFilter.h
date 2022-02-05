/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GeneratorFilters/TTbarWithJpsimumuFilter.h
 * @author Fr??d??ric Derue
 * @date Jan. 2018
 * @brief filter to select ttbar with Jpsi->mumu events
 */


#ifndef GeneratorFilters_XAODTTbarWithJpsimumuFilter_H
#define GeneratorFilters_XAODTTbarWithJpsimumuFilter_H

#include "GeneratorModules/GenFilter.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"


class xAODTTbarWithJpsimumuFilter: public GenFilter {
 public:
  xAODTTbarWithJpsimumuFilter(const std::string& fname, ISvcLocator* pSvcLocator);
  virtual ~xAODTTbarWithJpsimumuFilter();
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
  bool isLeptonDecay(const xAOD::TruthParticle* part, int type) const;  

  // method to check if Jpsi pass some selection criteria
  bool passJpsiSelection(const xAOD::TruthParticle* part) const;

};

#endif /// GeneratorFilters_xAODTTbarWithJpsimumuFilter_H
