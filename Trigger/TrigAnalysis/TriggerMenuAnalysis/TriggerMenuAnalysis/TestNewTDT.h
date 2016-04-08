/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TESTNEWTDT_H
#define TRIGGERMENUANALYSIS_TESTNEWTDT_H
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
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  ServiceHandle<IClassIDSvc> m_classIDSvc;

  const Trig::ChainGroup* m_chainGroup_L2;
  const Trig::ChainGroup* m_chainGroup_EF;

  std::vector<std::string> m_chainGroupNames;
  std::vector<const Trig::ChainGroup*> m_chainGroups;

  std::map<std::string, std::multiset<std::string> > m_chainCombMap;
};

#endif // TRIGGERMENUANALYSIS_TESTNEWTDT_H
