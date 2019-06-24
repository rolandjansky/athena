/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MULTIJETFLOWNETWORKBUILDER_H
#define TRIGHLTJETHYPO_MULTIJETFLOWNETWORKBUILDER_H


#include "./IFlowNetworkBuilder.h"
#include "./FlowNetworkBuilderBase.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"

class MultijetFlowNetworkBuilder: 
virtual public IFlowNetworkBuilder, private FlowNetworkBuilderBase{
 public:
  MultijetFlowNetworkBuilder(const ConditionsMT&);
 protected:
  virtual std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges(const HypoJetGroupCIter& groups_b,
		   const HypoJetGroupCIter& groups_e,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                   int& V,
		   std::map<int, pHypoJet>& nodeToJet) const override;
};

#endif
