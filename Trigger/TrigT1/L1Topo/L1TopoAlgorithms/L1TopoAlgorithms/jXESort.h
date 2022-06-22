/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jXESort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JXESORT_H
#define L1TOPOALGORITHMS_JXESORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jXESort : public SortingAlg {
   public:
      
      // constructor
      jXESort(const std::string & name);

      // destructor
      virtual ~jXESort();

      virtual StatusCode initialize();

      virtual TCS::StatusCode sortBitCorrect(const InputTOBArray & input, TOBArray & output);    
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   };

} // end of namespace TCS

#endif
