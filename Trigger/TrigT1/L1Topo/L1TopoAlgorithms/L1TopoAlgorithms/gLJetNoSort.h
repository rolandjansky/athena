/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// gLJetNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_GLJETNOSORT_H
#define L1TOPOALGORITHMS_GLJETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gLargeRJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class gLJetNoSort : public SortingAlg {
   public:
      
      // constructor
      gLJetNoSort(const std::string & name);

      // destructor
      virtual ~gLJetNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfgLJets = { 0 };
       
   };

} // end of namespace TCS

#endif
