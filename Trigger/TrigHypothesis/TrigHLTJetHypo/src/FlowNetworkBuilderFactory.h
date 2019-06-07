/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FLOWNETWORKBUILDERFACTORY_H
#define TRIGHLTJETHYPO_FLOWNETWORKBUILDERFACTORY_H

// ********************************************************************
//
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//
#include <string>
#include "./IFlowNetworkBuilder.h"

class FLowNetworkBuilder{

  /* Instantiate a class usd to build a spcific kind of flow network */

public:
  FlowNetworkBuilderFactory();
  std::unique_ptr<IFlowNetworkBuilder> create(const std::string&);
};

#endif
