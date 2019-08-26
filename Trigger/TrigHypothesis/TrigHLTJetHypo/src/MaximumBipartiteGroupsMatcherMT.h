/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H
#define TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H

// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcherMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./IFlowNetworkBuilder.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"
#include <optional>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class MaximumBipartiteGroupsMatcherMT:
virtual public IGroupsMatcherMT {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteGroupsMatcherMT(ConditionsMT&& cs);
  ~MaximumBipartiteGroupsMatcherMT(){}

  // cannot match if internal problem (eg FlowNetwork error)
  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
  std::string toString() const override;

private:
  ConditionsMT m_conditions;
  bool m_compound;  // true if jet group size >1
  std::size_t m_nConditions{0};
    
  std::unique_ptr<IFlowNetworkBuilder> m_flowNetworkBuilder;
  double m_totalCapacity{0};  // min number of jets to satisfy  all Conditions
};

#endif
