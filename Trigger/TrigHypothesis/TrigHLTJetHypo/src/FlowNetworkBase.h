/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FlowNetworkBase.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// Based class for flow network matchers. Derived classes provide a vector
// of FlowEdges
// AUTHOR:   P Sherwood
//
// ********************************************************************
//
#ifndef TRIGHLTJETHYPO_FLOWNETWORKBASE_H
#define TRIGHLTJETHYPO_FLOWNETWORKBASE_H

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./IFlowNetworkBuilder.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"
#include <optional>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class FlowNetworkBase:
virtual public IGroupsMatcherMT {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  FlowNetworkBase(std::size_t nConditions);

  /*
   FlowNetworkBase(unique_ptr<IFlowNetworkBuilder>,
		  std::size_t totalCapacity,
		  std::size_t m_nConditions);
  */
   
  ~FlowNetworkBase(){}

  // cannot match if internal problem (eg FlowNetwork error)
  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
  std::string toString() const override;

protected:
  std::size_t m_nConditions{0};    

  // Derived classes customise initialisation
  std::unique_ptr<IFlowNetworkBuilder> m_flowNetworkBuilder{nullptr};
  double m_totalCapacity{0};  // network capacity
};

#endif
