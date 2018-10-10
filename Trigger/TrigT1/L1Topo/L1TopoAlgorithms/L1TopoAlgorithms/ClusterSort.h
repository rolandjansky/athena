/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterSort.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__ClusterSort
#define TCS__ClusterSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class ClusterSort : public SortingAlg {
   public:
      
      // constructor
      ClusterSort(const std::string & name);

      // destructor
      virtual ~ClusterSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
   
   private:

      parType_t      m_numberOfClusters = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_iso = { 0 };   
      parType_t      m_doIsoCut = { 0 };
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
