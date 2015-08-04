/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_FLOWEDGEBAG_H
#define TRIGJETHYPO_FLOWEDGEBAG_H

// ********************************************************************
//
// NAME:     FlowEdgeBag.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
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
