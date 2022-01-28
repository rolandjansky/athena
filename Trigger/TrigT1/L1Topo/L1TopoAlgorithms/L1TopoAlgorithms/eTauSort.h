/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  eTauSort.h
//  TopoCore

#ifndef TCS__eTauSort
#define TCS__eTauSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class eTauSort : public SortingAlg {
   public:
      
      // constructor
      eTauSort(const std::string & name);

      // destructor
      virtual ~eTauSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
   
   private:

      parType_t      m_numberOfeTaus = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_rCore = { 0 };   
      parType_t      m_rHad = { 0 };   
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
