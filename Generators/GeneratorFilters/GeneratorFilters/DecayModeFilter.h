/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTER_DECAYMODEFILTER_H
#define GENERATORFILTER_DECAYMODEFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @brief Select events containing specific decay modes.
///
///  Currently known modes:
///  ID  Name      Process
///  A   direct    squark/gluino -> chi^0_1
///  B   bosonic   squark/gluino -> chi_2 -> chi^0_1
///  C   leptonic  squark/gluino -> chi_2 -> slepton -> chi^0_1
///  D   direct 3-body squark/gluino -> chi^0_1
///  E   asymmetric direct squark/gluino -> chi^0_1
///
///  Submodes for mode B are: 'W', 'Z' and 'H'.
///
///  Submodes for mode C are: 'se', 'smu' and 'stau'.
///
/// @author C. Horn, March 2010
///
class DecayModeFilter : public GenFilter {
public:

  DecayModeFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  std::string m_modeID;
  std::string m_submode;
  int m_nChargedLeptons;
  bool m_selected;

  std::vector<unsigned int> m_producedParticles_PDG;
  std::vector<unsigned int> m_chi2s_PDG;
  std::vector<unsigned int> m_LSPs_PDG;

  int m_nDirectModeEvents;
  int m_nBosonicModeEvents;
  int m_nLeptonicModeEvents;
  int m_nDirect3bodyModeEvents;
  //
  int m_nDoubleBosonicMode;
  int m_nDoubleLeptonicMode;
  int m_nBosonicLeptonicMode;
  //
  int m_nDirectDecays;
  int m_nBosonicDecays;
  int m_nLeptonicDecays;
  int m_nDirect3bodyDecays;

private:

  std::string printChain(HepMC::ConstGenParticlePtr parent) const;
  void analyzeChain(HepMC::ConstGenParticlePtr parent, bool&, bool&, bool&, bool& );
  void countChain(HepMC::ConstGenParticlePtr parent, int& length, int& Nchi2, int& NW,int& NZ,int& NH, int& Nse, int& Nsmu, int& Nstau, int& nChargedLeptons, int& nSMParticles) const;
  void countPIDs(int sparticle_PDG, int SM_PDG, int& Nchi2, int& NW, int& NZ, int& NH, int& Nse, int& Nsmu, int& Nstau, int& nChargedLeptons) const;

};

#endif
