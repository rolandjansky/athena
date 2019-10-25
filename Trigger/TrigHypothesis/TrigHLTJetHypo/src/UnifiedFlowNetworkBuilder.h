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
#include "./JetGroupProduct.h"

using TreeVec = std::vector<std::size_t>;
using IDJetGroup = std::map<std::size_t, HypoJetVector>;

// define a map from a jet group index to a set of indices
// that can be used to find a jet
using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;


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



  
  
  std::optional<std::vector<std::shared_ptr<FlowEdge>>>
    make_flowEdges_(const HypoJetGroupCIter& groups_b,
		    const HypoJetGroupCIter& groups_e,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
		    int& V,
		    std::map<int, pHypoJet>& nodeToJet,
		    const std::vector<int>& leaves) const;

  /*
  void makeSourceToLeafEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
			     const std::vector<int>& leaves)  const;
  */
  
  bool findInitialJobGroups(std::vector<std::shared_ptr<FlowEdge>>& edges,
			    const std::vector<int>& leaves,
			    const HypoJetGroupCIter& groups_b,
			    const HypoJetGroupCIter& groups_e,
			    CondInd2JetGroupsInds& satisfiedBy,
			    std::map<std::size_t, HypoJetVector>& indJetGroups,
			    JetGroupInd2ElemInds& jg2elemjgs,  
			    std::size_t& cur_jg,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;

  
  bool propagateEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
		      CondInd2JetGroupsInds& satisfiedBy,
		      const std::map<std::size_t, HypoJetVector>& indJetGroups,
		      JetGroupInd2ElemInds&,  
		      std::size_t& cur_jg,
		      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;

  /*
    void makeSinkEdges(std::vector<std::shared_ptr<FlowEdge>>& edges,
    std::size_t sink) const;
  */
  
  bool propagate_(std::vector<std::shared_ptr<FlowEdge>>& edges,
		  std::size_t par,
		  const std::vector<std::size_t>& siblings,
		  CondInd2JetGroupsInds& satisfiedBy,
		  JetGroupInd2ElemInds& jg2elemjgs,  
		  const std::map<std::size_t, HypoJetVector>& indJetGroups,
		  std::size_t& cur_jg,
		  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;
  
};

#endif
