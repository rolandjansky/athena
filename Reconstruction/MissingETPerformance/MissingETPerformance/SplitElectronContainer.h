/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SPLITELECTRONCONTAINER_H
#define SPLITELECTRONCONTAINER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "egammaEvent/Electron.h"

#include <string>

class SplitElectronContainer : public AthAlgorithm
{
 public:

  SplitElectronContainer(const std::string& name, ISvcLocator* pSvcLocator);
  ~SplitElectronContainer();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  virtual bool MatchTruthElectron(const Analysis::Electron*, const TruthParticleContainer*);

 private:
  std::string m_evtInfoKey;

  std::string m_eleContName;
  std::string m_ZeeCollName;
  std::string m_truthContainerName;

  std::string m_modEleContName; 
  std::string m_remEleContName; 
  std::string m_selEleContName; 
  std::string m_remEleTruthContName; 
  std::string m_selEleTruthContName; 

  bool m_doTruth;
  bool m_invSel;
  int m_event;

  // variable for truth matching
  Analysis::Electron* m_matchedElectron;
  bool m_isMatched;
  bool m_stableEle;
  double m_Dr;

protected:

};
#endif
