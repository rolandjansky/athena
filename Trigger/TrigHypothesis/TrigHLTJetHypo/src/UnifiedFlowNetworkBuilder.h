/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_UNIFIEDFLOWNETWORKBUILDER_H
#define TRIGHLTJETHYPO_UNIFIEDFLOWNETWORKBUILDER_H


#include "./IFlowNetworkBuilder.h"
#include "./FlowNetworkBuilderBase.h"
#include "./ConditionsDefsMT.h"
#include "./Tree.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"

using TreeVec = std::vector<std::size_t>;

class UnifiedFlowNetworkBuilder: 
virtual public IFlowNetworkBuilder, private FlowNetworkBuilderBase{
 public:
  UnifiedFlowNetworkBuilder(ConditionsMT, const Tree&);
 protected:
  virtual std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges(const HypoJetGroupCIter& groups_b,
		   const HypoJetGroupCIter& groups_e,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                   int& V,
		   std::map<int, pHypoJet>& nodeToJet) const override;

 private:
  Tree m_tree;  // tree structure for Conditions objects.
};

#endif
