/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// jMETNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JMETNOSORT_H
#define L1TOPOALGORITHMS_JMETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jMETNoSort : public SortingAlg {
   public:
      
      // constructor
      jMETNoSort(const std::string & name);

      // destructor
      virtual ~jMETNoSort();

      virtual StatusCode initialize();
      
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);



   };

} // end of namespace TCS

#endif
