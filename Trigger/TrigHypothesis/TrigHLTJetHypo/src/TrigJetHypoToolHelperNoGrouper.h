/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGJETHYPOHELPERNOGROUPER_H
#define TRIGHLTJETHYPO_TRIGJETHYPOHELPERNOGROUPER_H

/**
 * A configurable helper class to implement Jet Trigger algorithms.
 * Initial jet removal from incomming container is done using the 
 * prefilter predicates.
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
#include "./IJetsMatcher.h"
#include "./ConditionsDefs.h"
#include "./ITrigHypoJetVectorFilterConfig.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"
#include "ITrigJetHypoToolNoGrouperConfig.h"
#include "./ITrigJetConditionConfig.h"

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class TrigJetHypoToolHelperNoGrouper:
public extends<AthAlgTool, ITrigJetHypoToolHelper> {

 public:
  TrigJetHypoToolHelperNoGrouper(const std::string& type,
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

  // Object that matches jet groups with Conditions
  std::vector<std::unique_ptr<IJetsMatcher>> m_matchers;

  ///////////////////////////////

 // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandleArray<ITrigJetHypoToolNoGrouperConfig> m_configs {
   this, "HypoConfigurers", {},
   "Configurers to set up TrigJetHypoHelperNoGrouper"};

  ToolHandleArray<ITrigHypoJetVectorFilterConfig>
  m_prefilterMakers{this, "prefilterMakers", {},
    "configuration tool to create prefilter"};

  Gaudi::Property<bool>
  m_debug {this, "debug", false, "instantantiate helpers with this debug flag"};
  
  virtual std::string toString() const override;
};

#endif
