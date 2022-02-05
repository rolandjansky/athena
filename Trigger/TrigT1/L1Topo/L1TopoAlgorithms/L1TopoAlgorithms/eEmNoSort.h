/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// eEmNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_EEMNOSORT_H
#define L1TOPOALGORITHMS_EEMNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class eEmNoSort : public SortingAlg {
   public:
      
      // constructor
      eEmNoSort(const std::string & name);

      // destructor
      virtual ~eEmNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:

      parType_t      m_numberOfeEms = { 0 };
      parType_t      m_minREta = { 0 };
      parType_t      m_minRHad = { 0 };
      parType_t      m_minWsTot = { 0 }; 

   };

} // end of namespace TCS

#endif
