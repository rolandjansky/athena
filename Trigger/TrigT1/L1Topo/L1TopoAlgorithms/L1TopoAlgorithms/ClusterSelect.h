/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterSelect.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__ClusterSelect
#define TCS__ClusterSelect

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class ClusterSelect : public SortingAlg {
   public:
      
      // constructor
      ClusterSelect(const std::string & name);

      // destructor
      virtual ~ClusterSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:

      parType_t      m_numberOfClusters = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
      parType_t      m_iso = { 0 };   
      parType_t      m_doIsoCut = { 0 };   
      
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
