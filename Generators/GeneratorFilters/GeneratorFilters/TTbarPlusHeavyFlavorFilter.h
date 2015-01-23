/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GeneratorFilters/TTbarPlusHeavyFlavorFilter.h
 * @author Georges Aad
 * @date Sep. 2014
 * @brief filter to select ttbar+jets/HF events
*/


#ifndef GeneratorFilters_TTbarPlusHeavyFlavorFilter_H
#define GeneratorFilters_TTbarPlusHeavyFlavorFilter_H

#include "GeneratorModules/GenFilter.h"

namespace HepMC{
  class GenParticle;
}

class TTbarPlusHeavyFlavorFilter: public GenFilter {
public:
  TTbarPlusHeavyFlavorFilter(const std::string& fname, ISvcLocator* pSvcLocator);
  virtual ~TTbarPlusHeavyFlavorFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:


  /// properties
  bool m_useFinalStateHadrons;
  bool m_selectB;
  bool m_selectC;
  bool m_selectL;


  double m_bPtMinCut;
  double m_bEtaMaxCut;
  double m_cPtMinCut;
  double m_cEtaMaxCut;

  int m_bMultiCut;
  int m_cMultiCut;

  bool m_excludeBFromTop;
  bool m_excludeCFromTop;


  MsgStream m_log;

  bool passBSelection(const HepMC::GenParticle* part) const;
  bool passCSelection(const HepMC::GenParticle* part) const;

  int hadronType(int pdgid) const;
  bool isBHadron(const HepMC::GenParticle* part) const;
  bool isCHadron(const HepMC::GenParticle* part) const;

  bool isInitialHadron(const HepMC::GenParticle* part) const;
  bool isFinalHadron(const HepMC::GenParticle* part) const;

  bool isQuarkFromHadron(const HepMC::GenParticle* part) const;
  bool isCHadronFromB(const HepMC::GenParticle* part) const;

  /// init_part needed to detect looping graphs (sherpa) and to switch on using barcode to resolve it without affecting pythia8
  /// up to know only seen at parton level
  bool isLooping(const HepMC::GenParticle* part, std::set<const HepMC::GenParticle*> init_part = std::set<const HepMC::GenParticle*>()) const;

  const HepMC::GenParticle*  findInitial(const HepMC::GenParticle* part, bool looping) const;

  bool isFromTop(const HepMC::GenParticle* part, bool looping) const;
  bool isDirectlyFromTop(const HepMC::GenParticle* part, bool looping) const;
  bool isDirectlyFromWTop(const HepMC::GenParticle* part, bool looping) const;



  

  

};

#endif /// GeneratorFilters_TTbarPlusHeavyFlavorFilter_H
