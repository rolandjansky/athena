/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FLOWNETWORKBUILDERBASE_H
#define TRIGHLTJETHYPO_FLOWNETWORKBUILDERBASE_H

#include "./IFlowNetworkBuilder.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"

class FlowNetworkBuilderBase: virtual public IFlowNetworkBuilder{
 public:
  FlowNetworkBuilderBase(ConditionsMT);
  virtual std::optional<std::unique_ptr<FlowNetwork>>
    create(const HypoJetGroupCIter&,
           const HypoJetGroupCIter&,
           const std::unique_ptr<ITrigJetHypoInfoCollector>&,
	   std::map<int, pHypoJet>& nodeToJet) const;

  virtual std::string toString() const override;
 protected:

  virtual std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges(const HypoJetGroupCIter& groups_b,
		   const HypoJetGroupCIter& groups_e,
		   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
		   int& V,
		   std::map<int, pHypoJet>& nodeToJet) const = 0;

  std::optional<std::vector<std::vector<int>>>
    conditionGroupMatches(const HypoJetGroupCIter& groups_b,
			  const HypoJetGroupCIter& groups_e,
			  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;

  std::vector<std::shared_ptr<FlowEdge>> getSourceToConditionsEdges() const;
  ConditionsMT m_conditions;

};

#endif
