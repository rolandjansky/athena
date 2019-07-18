/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PARTITIONSGROUPSTOHELPERSMATCHERMT_H
#define TRIGHLTJETHYPO_PARTITIONSGROUPSTOHELPERSMATCHERMT_H

// ********************************************************************
//
// NAME:     PartitionsGroupsToHelpersMatcherMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "AsgTools/ToolHandleArray.h"
#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"

#include <optional>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

using HelperToolArray = ToolHandleArray<ITrigJetHypoToolHelperMT>;


class PartitionsGroupsToHelpersMatcherMT: public IGroupsMatcherMT {

  /* Looks for a HypooJet Vector chosen from a list of input HypoJetVectors
     that satisfies all of the child ITrigJetHypoToolHelperMT objects*/

  
public:
  PartitionsGroupsToHelpersMatcherMT(const HelperToolArray&);
  ~PartitionsGroupsToHelpersMatcherMT(){}

  // cannot match if internal problem (eg FlowNetwork error)
  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
  std::string toString() const noexcept override;

private:
  HelperToolArray m_helpers;

};

#endif
