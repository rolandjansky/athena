/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  LayoutConstraints.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//
//  Class provides static access to limits of the hardware

#ifndef __TopoCore__LayoutConstraints__
#define __TopoCore__LayoutConstraints__

#include <iostream>

namespace TCS {
   
   class LayoutConstraints {
   public:
      static unsigned int maxComponents() { return g_maxComponents; }
      
      static unsigned int maxJets() { return g_maxJets; }
      static unsigned int maxClusters() { return g_maxClusters; }
      
      static unsigned int maxInclJets() { return g_maxInclJets; }
      static unsigned int maxInclClusters() { return g_maxInclClusters; }
      
      static unsigned int maxParams() { return g_maxParams; }
      
   private:
      // Maximum number of components that require parameters
      static unsigned int g_maxComponents;
      
      // Max number of TOBs to be received by L1Topo
      static unsigned int g_maxJets;
      static unsigned int g_maxClusters;
      
      // Max number of TOBs to be analysed by inclusive algorithms:
      static unsigned int g_maxInclJets;
      static unsigned int g_maxInclClusters;

      // Maximum number of parameters per algorithm
      static unsigned int g_maxParams;

   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__LayoutConstraints__) */
