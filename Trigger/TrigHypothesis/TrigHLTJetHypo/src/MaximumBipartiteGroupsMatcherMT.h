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
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"
#include <optional>
#include <map>

class ITrigJetHypoInfoCollector;

class MaximumBipartiteGroupsMatcherMT:
virtual public IGroupsMatcherMT {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteGroupsMatcherMT(const ConditionsMT& cs);
  ~MaximumBipartiteGroupsMatcherMT(){}
  bool match(const HypoJetGroupCIter&,
             const HypoJetGroupCIter&,
             const std::unique_ptr<ITrigJetHypoInfoCollector>&,
	     bool debug=false) const override;
  std::string toString() const noexcept override;
  ConditionsMT getConditions() const noexcept override;
private:
  ConditionsMT m_conditions;
  bool m_compound;  // true if jet group size >1 
  bool m_pass;

  std::vector<std::shared_ptr<FlowEdge>> m_srcToCondEdges;

  std::optional<std::vector<std::vector<int>>>
    conditionGroupMatches(const HypoJetGroupCIter&,
			  const HypoJetGroupCIter&,
			  const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;

  
  std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges(const HypoJetGroupCIter&,
                   const HypoJetGroupCIter&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&,
                   int&,
                   std::map<int, pHypoJet>& nodeToJet) const;
  
};

#endif
