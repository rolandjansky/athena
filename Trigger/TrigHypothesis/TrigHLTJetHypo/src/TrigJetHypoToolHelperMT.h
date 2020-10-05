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
 * The IMatcher objector owns a set of Conditions objects. 
 * The Matcher determines 
 * whether the container of jet groups satisfies the Conditions. If so,
 * the event passes, otherwise it fails.
 *
 */

#include <vector>
#include <memory>
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class TrigJetHypoToolHelperMT:
public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

 public:
  TrigJetHypoToolHelperMT(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  StatusCode initialize() override;
  virtual bool

    // pass - tests wethewr a jet collection passes cuts, and collects
    // information about the decision.
    pass(HypoJetVector&,
	 xAODJetCollector&,
	 const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  virtual std::size_t requiresNJets() const override;
  
  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;

 private:

  // Object to make jet groups. Jet groups
  // are vectors of jets selected from a jet vector
  // which is, in this case, the incoming jet vector.
  std::unique_ptr<IJetGrouper> m_grouper;

  // Object that matches jet groups with Conditions
  std::unique_ptr<IGroupsMatcherMT> m_matcher;

  // Bridge objects to ICleaner predicate function objects to allow polymorphic
  // pointers to be used with the STL (pass by value).
  ToolHandleArray<ICleanerTool> m_cleaners;

  ///////////////////////////////

 // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandle<ITrigJetHypoToolConfig> m_config {
   this, "HypoConfigurer", {}, "Configurer to set up TrigHLTJetHypoHelper2"}; 

 Gaudi::Property<int>
  m_parentNodeID {this, "parent_id", {0}, "hypo tool tree parent node id"};

Gaudi::Property<int>
  m_nodeID {this, "node_id", {0}, "hypo tool tree node id"};

Gaudi::Property<bool>
  m_debug {this, "debug", false, "instantantiate helpers with this debug flag"};


 void collectData(const std::string& exetime,
                  const std::unique_ptr<ITrigJetHypoInfoCollector>&,
                  const std::optional<bool>& pass) const;

 virtual std::string toString() const override;
};

#endif
