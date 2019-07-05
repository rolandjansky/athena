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

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

class ITrigJetInfoCollector;
class xAODJetCollector;

class NotHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

 public:
  
  NotHelperTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
  
  bool pass(HypoJetVector&,
	    xAODJetCollector&,
            const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;


  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;

 private:
  
  
  ToolHandle<ITrigJetHypoToolHelperMT> m_hypoTool {
    this, "hypoTool", {}, "predicate to be inverted"};
  
  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};

  Gaudi::Property<int>
   m_nodeID {this, "node_id", {}, "hypo tool tree node id"};
  
  std::string toString() const;
};
#endif

