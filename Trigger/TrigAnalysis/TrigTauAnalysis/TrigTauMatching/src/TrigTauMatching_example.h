/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGTAUMACTHING_EXAMPLE_H_
#define TRIGTAUMACTHING_EXAMPLE_H_

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigTauMatching/ITrigTauMatching.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


class TrigTauMatching_example : public AthAlgorithm
{

public:
  TrigTauMatching_example(const std::string &name,
                          ISvcLocator *svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  Long64_t m_eventCount;
  std::pair<int, int> m_tauEvents;
  std::pair<int, int> m_diTauEvents;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
  ToolHandle<Trig::ITrigTauMatchingTool> m_matchTool;
};

#endif 
