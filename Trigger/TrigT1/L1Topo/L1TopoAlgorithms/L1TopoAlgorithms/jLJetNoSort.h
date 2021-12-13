/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// jLJetNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JLARGERJETNOSORT_H
#define L1TOPOALGORITHMS_JLARGERJETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLargeRJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jLJetNoSort : public SortingAlg {
   public:
      
      // constructor
      jLJetNoSort(const std::string & name);

      // destructor
      virtual ~jLJetNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfjLargeRJets = { 0 };
   
   };

} // end of namespace TCS

#endif
