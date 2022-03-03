/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jEmNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JEMNOSORT_H
#define L1TOPOALGORITHMS_JEMNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jEmTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jEmNoSort : public SortingAlg {
   public:
      
      // constructor
      jEmNoSort(const std::string & name);

      // destructor
      virtual ~jEmNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfjEms = { 0 };
       
   };

} // end of namespace TCS

#endif
