/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// jJetNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JJETNOSORT_H
#define L1TOPOALGORITHMS_JJETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jJetNoSort : public SortingAlg {
   public:
      
      // constructor
      jJetNoSort(const std::string & name);

      // destructor
      virtual ~jJetNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfjJets = { 0 };
       
   };

} // end of namespace TCS

#endif
