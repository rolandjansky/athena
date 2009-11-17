/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNTUPLEBASE_NTUPLEALGORITHM_H
#define TRIGNTUPLEBASE_NTUPLEALGORITHM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigNtupleBase/NtupleComponent.h"
#include "TTree.h"
#include <vector>
#include <string>
#include <map>

class NtupleAlgorithm: public AthAlgorithm {

 public:
  NtupleAlgorithm (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);
  StatusCode beginRun();

  TTree *tree(void) { return m_tree; }
  TTree *treeRun(void) { return m_treeRun; }

  StatusCode containerKey(std::string propertyName, std::string *containerKey);

  ServiceHandle<ActiveStoreSvc>& activeStore() const;

  ToolHandle<Trig::TrigDecisionTool>& trigDecisionTool() const;

 protected:
  /** A function to parse the NtComponent class requests */
  StatusCode parseNtupleComponentStrings(void);

  /** Ntuple properties */
  std::string m_ntupleFile;
  std::string m_ntupleDir;
  std::string m_ntupleTreeName;
  std::string m_ntupleTreeTitle;

  /** A vector of the names of NtComponent classes available from this algorithm */
  std::vector<std::string> m_availableComponents;

  /** A vector of strings defining ntuple components and their properties */
  std::vector<std::string> m_ntupleComponentCommands;

  /** A vector of NtComponent class parameters */
  std::vector<NtComponent::NtComponentParameters> m_ntupleComponentParameters;
    
  /** A vector of instantiated ntuple components */
  std::vector<NtComponent::NtupleComponent *> m_ntupleComponents;

  /** A vector of instantiated run-wise ntuple components */
  std::vector<NtComponent::NtupleComponent *> m_ntupleRunComponents;

 private:
  /** A pointer to the TTree */
  TTree *m_tree;
  TTree *m_treeRun;

  /** Iterators for access to ntuple component vector */
  std::vector<NtComponent::NtupleComponent *>::iterator m_itrNtComp;
  std::vector<NtComponent::NtupleComponent *>::iterator m_itrNtCompEnd;

  mutable ServiceHandle<ActiveStoreSvc> m_activeStore;
  mutable ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
};

inline
ServiceHandle<ActiveStoreSvc>& NtupleAlgorithm::activeStore() const 
{ return m_activeStore; } 

inline
ToolHandle<Trig::TrigDecisionTool>& NtupleAlgorithm::trigDecisionTool() const 
{ return m_trigDecisionTool; }

#endif
