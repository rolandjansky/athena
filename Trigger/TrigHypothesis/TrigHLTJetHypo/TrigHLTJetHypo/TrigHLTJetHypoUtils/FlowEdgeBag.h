/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FLOWEDGEBAG_H
#define TRIGHLTJETHYPO_FLOWEDGEBAG_H

// ********************************************************************
//
// NAME:     FlowEdgeBag.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <vector>
#include "./FlowEdge.h"


using spFE = std::shared_ptr<FlowEdge>;
using FlowEdgeBag = std::vector<spFE>;
#endif
