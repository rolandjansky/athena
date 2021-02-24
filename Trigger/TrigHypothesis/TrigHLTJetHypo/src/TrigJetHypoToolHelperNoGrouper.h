/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "./IJetsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./ConditionFilter.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolNoGrouperConfig.h"
#include "./ITrigJetRepeatedConditionConfig.h"

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class TrigJetHypoToolHelperNoGrouper:
public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {

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
  std::vector<std::unique_ptr<IJetsMatcherMT>> m_matchers;

  ///////////////////////////////

 // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandleArray<ITrigJetHypoToolNoGrouperConfig> m_configs {
   this, "HypoConfigurers", {},
   "Configurers to set up TrigJetHypoHelperNoGrouper"};

  ToolHandleArray<ITrigJetRepeatedConditionConfig>
  m_prefilterConditionMakers{this, "prefiltConditionMakers", {},
    "hypo tree Condition builder AlgTools for hypo pre-filtering"};

  // object that copies selected incomming jets into a new vector.
  ConditionFilter m_prefilter{}; 

  
  Gaudi::Property<bool>
  m_debug {this, "debug", false, "instantantiate helpers with this debug flag"};
  
  StatusCode makePrefilter();
  
  virtual std::string toString() const override;
};

#endif
