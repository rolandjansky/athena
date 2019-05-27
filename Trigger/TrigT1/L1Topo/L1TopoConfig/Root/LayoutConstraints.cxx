/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  LayoutConstraints.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/18/12.

#include "L1TopoConfig/LayoutConstraints.h"


// Maximum number of components that require parameters
const unsigned int TCS::LayoutConstraints::g_maxComponents = 100;

// Max number of TOBs to be received by L1Topo
const unsigned int TCS::LayoutConstraints::g_maxJets = 48;
const unsigned int TCS::LayoutConstraints::g_maxClusters = 80;

// Max number of TOBs to be analysed by inclusive algorithms:
const unsigned int TCS::LayoutConstraints::g_maxInclJets = 8;
const unsigned int TCS::LayoutConstraints::g_maxInclClusters = 8;

// Maximum number of parameters per algorithm
const unsigned int TCS::LayoutConstraints::g_maxParams = 20;
