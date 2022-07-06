/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jXENoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JXENOSORT_H
#define L1TOPOALGORITHMS_JXENOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jXENoSort : public SortingAlg {
   public:
      
      // constructor
      jXENoSort(const std::string & name);

      // destructor
      virtual ~jXENoSort();

      virtual StatusCode initialize();
      
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   };

} // end of namespace TCS

#endif
