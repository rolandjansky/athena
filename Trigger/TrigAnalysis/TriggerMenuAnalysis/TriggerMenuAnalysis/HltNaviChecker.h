/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALSYIS_HLTNAVICHECKER_H
#define TRIGGERMENUANALSYIS_HLTNAVICHECKER_H
/*
  HltNaviChecker.h
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TFile.h"
#include "TTree.h"

class HltNaviChecker : public AthAlgorithm {
public:
  HltNaviChecker(const std::string& name, ISvcLocator* svcloc);
  ~HltNaviChecker();

  StatusCode beginRun();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  void checkFeatures(const std::string& chain_name);
  void checkPassBits();
  void checkFeatures(const HLT::TriggerElement* te, 
		     HLT::NavigationCore* navi, 
		     const std::string& prefix="");
private:
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  ToolHandle<RoILinksCnvTool> m_RoILinksCnvTool;
  std::vector<std::string> m_chainNames;

  TFile* m_file;
  TTree* m_tree;
  int m_NPassBits;
  std::vector<unsigned int>* m_EFElectronPassBitsIndex;
  
};

#endif // TRIGGERMENUANALSYIS_HLTNAVICHECKER_H
