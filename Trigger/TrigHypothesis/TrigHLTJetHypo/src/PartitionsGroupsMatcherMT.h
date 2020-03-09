/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PARTITIONSGROUPSMATCHERMT_H
#define TRIGHLTJETHYPO_PARTITIONSGROUPSMATCHERMT_H

// ********************************************************************
//
// NAME:     PartitionsGroupsMatcherMT.h
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
#include <optional>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class PartitionsGroupsMatcherMT:
virtual public IGroupsMatcherMT {

  /* Given a two equi-length containers of HypoJetVectors
     and Conditions , associate the ith elements, and determine if,
     for each i, the Condition is datisfoied by the HypoJetVector.
  */

public:
  PartitionsGroupsMatcherMT(ConditionsMT&& cs);
  ~PartitionsGroupsMatcherMT(){}

  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
  std::string toString() const noexcept override;

private:
  ConditionsMT m_conditions;
  std::size_t m_nConditions{0};
  std::size_t m_minNjets{0};

};

#endif
