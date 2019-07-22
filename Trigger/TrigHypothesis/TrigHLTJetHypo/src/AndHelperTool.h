/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ANDHELPERTOOLMT_H
#define TRIGHLTJETHYPO_ANDHELPERTOOLMT_H
/********************************************************************
 *
 * NAME:     AndHelperTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "./ITrigJetHypoInfoCollector.h"

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class AndHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

 public:

  AndHelperTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  bool pass(HypoJetVector&,
	    xAODJetCollector&,
            const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;

  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;
  virtual std::string toString() const override;
  virtual std::size_t requiresNJets() const override;

 private:

  ToolHandle<ITrigJetHypoToolHelperMT> m_lhs {
    this, "lhs", {}, "LHS boolean binary expression"}; 
  
  ToolHandle<ITrigJetHypoToolHelperMT> m_rhs {
    this, "rhs", {}, "RHS boolean binary expression"}; 
  
  
  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};
  
  Gaudi::Property<int>
    m_nodeID {this, "node_id", {}, "hypo tool tree node id"};


};
#endif
