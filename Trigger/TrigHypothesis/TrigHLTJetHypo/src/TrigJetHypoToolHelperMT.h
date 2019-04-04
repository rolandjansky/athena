/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGJETHYPOHELPERMT_H
#define TRIGHLTJETHYPO_TRIGJETHYPOHELPERMT_H

/**
 * A configurable helper class to implement Jet Trigger algorithms.
 * Initial jet removal from incomming container is done using the ICleaner predicates.
 * The surviving jets are grouped into subsets by the IJetGroup object.
 *
 * The IMatcher objector owns a set of Conditions objects. The MAtcher determines 
 * wether the container of jet groups satisfies the Conditions. If so,
 * the event passes, otherwise it fails.
 *
 */

#include <vector>
#include <memory>
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

#include "ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"

#include "./Timer.h"

class ITrigJetHypoHelperVisitor;

class TrigJetHypoToolHelperMT:
public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

 public:
  TrigJetHypoToolHelperMT(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual bool  pass(HypoJetVector&) override;
  
  virtual void accept(ITrigJetHypoHelperVisitor&) override;

  std::string toStringAndResetHistory();

 private:

  // Object to make jet groups. Jet groups
  // are vectors of jets selected from a jet vector
  // which is, in this case, the incoming jet vector.
  std::unique_ptr<IJetGrouper> m_grouper;

  // Object that matchs jet groups with Conditions
  std::unique_ptr<IGroupsMatcher> m_matcher;

  // Bridge objects to ICleaner predicate function objects to allow polymorphic
  // pointers to be used with the STL (pass by value).
  ToolHandleArray<ICleanerTool> m_cleaners;


  // Paraphanalia needed for the Jet Hypo Helper class:
  Conditions m_conditions;

  ///////////////////////////////

 // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandle<ITrigJetHypoToolConfig> m_config {
   this, "HypoConfigurer", {}, "Configurer to set up TrigHLTJetHypoHelper2"}; 

 Gaudi::Property<int>
  m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};

Gaudi::Property<int>
  m_nodeID {this, "node_id", {}, "hypo tool tree node id"};

Gaudi::Property<bool>
  m_debug {this, "debug", false, "instantantiate helpers with this debug flag"};

 bool m_pass;

 std::string toString() const;
 void resetHistory();

 std::unique_ptr<JetTrigTimer> m_timer;
};

#endif
