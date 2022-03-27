/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// eTauNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_ETAUNOSORT_H
#define L1TOPOALGORITHMS_ETAUNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class eTauNoSort : public SortingAlg {
   public:
      
      // constructor
      eTauNoSort(const std::string & name);

      // destructor
      virtual ~eTauNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:

      parType_t      m_numberOfeTaus = { 0 };
      parType_t      m_minRCore = { 0 };
      parType_t      m_minRHad = { 0 };
   };

} // end of namespace TCS

#endif
