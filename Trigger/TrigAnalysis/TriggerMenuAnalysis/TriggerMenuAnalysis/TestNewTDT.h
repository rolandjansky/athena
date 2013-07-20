/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TestNewTDT_h__
#define __TestNewTDT_h__
/*
  TestNewTDT.h
*/
#include <string>
#include <vector>
#include <set>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

class TestNewTDT : public AthAlgorithm {
public:
  TestNewTDT(const std::string& name, ISvcLocator* svcloc);
  ~TestNewTDT();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

protected:
  void printTDT(const Trig::ChainGroup* cg);

  void inspectChain(const std::string& chain_name);
  void inspectCombination(const Trig::Combination& comb, 
			  const std::string& chain_name);

private:
  ToolHandle<Trig::TrigDecisionTool> mTrigDecisionTool;
  ServiceHandle<IClassIDSvc> mClassIDSvc;

  const Trig::ChainGroup* mChainGroup_L2;
  const Trig::ChainGroup* mChainGroup_EF;

  std::vector<std::string> mChainGroupNames;
  std::vector<const Trig::ChainGroup*> mChainGroups;

  std::map<std::string, std::multiset<std::string> > mChainCombMap;
};

#endif // __TestNewTDT_h__
