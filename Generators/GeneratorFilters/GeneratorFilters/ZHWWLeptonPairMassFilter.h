/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filter according to the following steps:
// Require four leptons, total charge zero, at least one same-flavour opposite-sign (SFOS) pair
// Determine SFOS pair with mass closest to Z mass peak
//   This pair is the Z-candidate pair, the other pair is the Higgs-candidate pair
// Require Higgs-candidate pair mass < x (default 75 GeV)
// If the Higgs-candidate pair is SFOS, require MET > y (default 1 GeV)
//
// AuthorList:
//         A Liblong: Oct 2016


#ifndef GENERATORFILTERS_ZHWWLEPTONPAIRMASSFILTER_H
#define GENERATORFILTERS_ZHWWLEPTONPAIRMASSFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/MsgStream.h"
#include <functional>

class ZHWWLeptonPairMassFilter: public GenFilter {
 public:
  ZHWWLeptonPairMassFilter(const std::string& alg_name, ISvcLocator* pSvcLocator);
  virtual ~ZHWWLeptonPairMassFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();
  bool isPrompt(const HepMC::GenParticle* part);
  bool hasParentThat(const HepMC::GenParticle* part, std::function<bool(const HepMC::GenParticle*)> fn);
  std::vector<HepMC::GenParticle*> getPromptLeptons();
  void checkParentPDGID(const GenParticle* part);

 private:
  // user-defined parameters
  double m_H_cand_mass_cut_upper = 75000.0; 
  double m_H_cand_mass_cut_lower = 5000.0; 
  double m_Z_cand_mass_max_distance_from_peak = 15000.0; 

  MsgStream m_log;
};

#endif
