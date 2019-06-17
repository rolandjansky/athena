/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IFLOWNETWORKBUILDER_H
#define TRIGHLTJETHYPO_IFLOWNETWORKBUILDER_H

// ********************************************************************
//
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <map>
#include <optional>

class IFlowNetworkBuilder{

  /* Instantiate a class usd to build a spcific kind of flow network */

public:
  IFlowNetworkBuilder(){};
  ~IFlowNetworkBuilder(){};
  virtual std::optional<std::unique_ptr<FlowNetwork>>
    create(const HypoJetGroupCIter&,
           const HypoJetGroupCIter&,
           const std::unique_ptr<ITrigJetHypoInfoCollector>&,
           std::map<int, pHypoJet>& nodeToJet) const = 0;
};

#endif
