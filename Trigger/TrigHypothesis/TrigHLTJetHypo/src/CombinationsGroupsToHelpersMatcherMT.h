/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMBINATIONSGROUPSTOHELPERSMATCHERMT_H
#define TRIGHLTJETHYPO_COMBINATIONSGROUPSTOHELPERSMATCHERMT_H

// ********************************************************************
//
// NAME:     CombinationsGroupsToHelpersMatcherMT.h
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


class CombinationsGroupsToHelpersMatcherMT: public IGroupsMatcherMT {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  CombinationsGroupsToHelpersMatcherMT(const HelperToolArray&);
  ~CombinationsGroupsToHelpersMatcherMT(){}

  // cannot match if internal problem (eg FlowNetwork error)
  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
  std::string toString() const noexcept override;

private:
  HelperToolArray m_helpers;

bool
  groupPasses(const HypoJetVector&,
	      xAODJetCollector& jetCollector,
	      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;
};

#endif
