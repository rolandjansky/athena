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
  UnifiedFlowNetworkBuilder(ConditionsMT,
			    const Tree&,
			    const std::vector<std::vector<int>>&);

 protected:
  virtual std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges(const HypoJetGroupCIter& groups_b,
		   const HypoJetGroupCIter& groups_e,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                   int& V,
		   std::map<int, pHypoJet>& nodeToJet) const override;
 
  std::string toString() const override;

 private:

  // tree structure for Conditions objects. Flow networks
  // will be constructed using Conditions and jet group objects.
  // The conditions tree gives relations among conditions (eg parent-child)
  Tree m_tree;

  //a vector of shared nodes. All shared nodes are leaf node that
  // see the jet collection.
  std::vector<std::vector<int>> m_sharedNodes;
};

#endif
