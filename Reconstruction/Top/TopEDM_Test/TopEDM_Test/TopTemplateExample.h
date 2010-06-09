/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2008

#ifndef TOPTEMPLATEEXAMPLE_H
#define TOPTEMPLATEEXAMPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TopEvent/TTbarLJContainer.h"

class TopTemplateExample : public AthAlgorithm {
public:
  TopTemplateExample(const std::string &name,ISvcLocator *pSvcLocator);

  // The usual stuff
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  // An analysis for a specific channel does not have to be templated:
  void electronChannelAnalysis(const TopRec::TTbarEJContainer& ttbars);

  // Analysis code that is common to different lepton channels can go
  // into a templated method
  template<class TopContainer> void genericLeptonAnalysis(const TopContainer& ttbars);

private:
  // StoreGate keys
  std::string m_ttbarEJContainerName;
  std::string m_ttbarMuJContainerName;

};

#endif/*TOPTEMPLATEEXAMPLE_H*/
