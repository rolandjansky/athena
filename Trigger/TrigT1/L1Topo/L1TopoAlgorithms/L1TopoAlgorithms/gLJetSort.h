/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gLJetSort.h
//  TopoCore

#ifndef L1TOPOALGORITHMS_GLJETSORT_H
#define L1TOPOALGORITHMS_GLJETSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gLargeRJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class gLJetSort : public SortingAlg {
   public:
      
      // constructor
      gLJetSort(const std::string & name);

      // destructor
      virtual ~gLJetSort();
      virtual TCS::StatusCode initialize(); 
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:
      parType_t      m_numberOfgLJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
