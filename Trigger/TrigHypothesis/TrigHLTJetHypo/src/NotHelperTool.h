/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collabnotation
*/

#ifndef TRIGJETNOTTOOLMT_H
#define TRIGJETNOTTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetNotToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "AthenaBaseComps/AthAlgTool.h"

#include "ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include "./Timer.h"

class ITrigJetHypoHelperVisitor;

class NotHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

 public:
  
  NotHelperTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  

  bool pass(HypoJetVector&) override;
  virtual void accept(ITrigJetHypoHelperVisitor&) override;
  std::string toStringAndResetHistory();
  
 private:
  
  
  ToolHandle<ITrigJetHypoToolHelperMT> m_hypoTool {
    this, "hypoTool", {}, "predicate to be inverted"};
  
  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};

  Gaudi::Property<int>
   m_nodeID {this, "node_id", {}, "hypo tool tree node id"};
  
  bool m_pass;
  std::size_t m_nCalls{0};
  
  std::string toString() const;
  void resetHistory();

  std::unique_ptr<JetTrigTimer> m_timer;
  double passTime();
};
#endif
