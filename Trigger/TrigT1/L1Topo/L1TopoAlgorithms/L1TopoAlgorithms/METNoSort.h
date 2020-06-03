/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  METNoSort.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__METNoSort
#define TCS__METNoSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class METNoSort : public SortingAlg {
   public:
      
      // constructor
      METNoSort(const std::string & name);

      // destructor
      virtual ~METNoSort();

      virtual StatusCode initialize();
      
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);



   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
