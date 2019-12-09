/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FlowNetworkMatcherMatcherBase.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// Base class for flow network matchers. Derived classes provide a vector
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

class FlowNetworkMatcherBase:
virtual public IGroupsMatcherMT {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  FlowNetworkMatcherBase(std::size_t nConditions);

  /*
   FlowNetworkMatcherBase(unique_ptr<IFlowNetworkBuilder>,
		  std::size_t totalCapacity,
		  std::size_t m_nConditions);
  */
   
  ~FlowNetworkMatcherBase(){}

  // cannot match if internal problem (eg FlowNetwork error)
  std::optional<bool> match(const HypoJetGroupCIter&,
			    const HypoJetGroupCIter&,
			    xAODJetCollector&,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
			    bool debug=false) const override;
 protected:
  // Derived classes customise initialisation
  void setFlowNetworkBuilder(std::unique_ptr<IFlowNetworkBuilder> );
  void setTotalCapacity(double c){m_totalCapacity = c;}

  std::string toString() const {return m_flowNetworkBuilder -> toString();}

  virtual void
    reportPassingJets(const std::map<int, pHypoJet>&,
		      const std::unique_ptr<FlowNetwork>&,
		      const std::unique_ptr<ITrigJetHypoInfoCollector>&,
		      xAODJetCollector&
		      ) const = 0;

 private:
  std::size_t m_nConditions{0};    

  std::unique_ptr<IFlowNetworkBuilder> m_flowNetworkBuilder{nullptr};
  double m_totalCapacity{0};  // network capacity
};

#endif
