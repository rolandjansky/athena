/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  LayoutConstraints.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#include "L1TopoCoreSimulation/LayoutConstraints.h"


// Maximum number of components that require parameters
unsigned int TCS::LayoutConstraints::g_maxComponents = 70;

// Max number of TOBs to be received by L1Topo
unsigned int TCS::LayoutConstraints::g_maxJets = 48;
unsigned int TCS::LayoutConstraints::g_maxClusters = 80;

// Max number of TOBs to be analysed by inclusive algorithms:
unsigned int TCS::LayoutConstraints::g_maxInclJets = 8;
unsigned int TCS::LayoutConstraints::g_maxInclClusters = 8;

// Maximum number of parameters per algorithm
unsigned int TCS::LayoutConstraints::g_maxParams = 10;
