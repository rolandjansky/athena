/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// gJetNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_GJETNOSORT_H
#define L1TOPOALGORITHMS_GJETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class gJetNoSort : public SortingAlg {
   public:
      
      // constructor
      gJetNoSort(const std::string & name);

      // destructor
      virtual ~gJetNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfgJets = { 0 };
       
   };

} // end of namespace TCS

#endif
