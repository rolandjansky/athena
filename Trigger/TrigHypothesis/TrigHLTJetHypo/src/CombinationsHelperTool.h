/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* IAlg tool that splits up incomming jets and sends then to its children */

#ifndef TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H
#define TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include "./Timer.h"

class ITrigJetHypoHelperVisitor;

class CombinationsHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {
 public:
  
  CombinationsHelperTool(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  bool pass(HypoJetVector&) override;

  virtual void accept(ITrigJetHypoHelperVisitor&) override;
  std::string toStringAndResetHistory();

 private:

   ToolHandleArray<ITrigJetHypoToolHelperMT> m_children {
     this, "children", {}, "list of child decidables"};
   
  Gaudi::Property<unsigned int>
    m_size{this, "groupSize", {}, "Jet group size"};
  
  bool testGroup(HypoJetVector&) const;

  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};
  
  Gaudi::Property<int>
    m_nodeID {this, "node_id", {}, "hypo tool tree node id"};

  bool m_pass;

  std::string toString() const;

  // use timers through pointers in order to update from const pass() 
  std::unique_ptr<JetTrigTimer> m_totalTimer;
  std::unique_ptr<JetTrigTimer> m_setupTimer;
  std::unique_ptr<JetTrigTimer> m_extraTimer;
  std::unique_ptr<JetTrigTimer> m_extraTimer1;


};
#endif
