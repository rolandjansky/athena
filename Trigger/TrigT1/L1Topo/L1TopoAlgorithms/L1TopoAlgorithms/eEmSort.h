/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  eEmSort.h
//  TopoCore

#ifndef TCS__eEmSort
#define TCS__eEmSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class eEmSort : public SortingAlg {
   public:
      
      // constructor
      eEmSort(const std::string & name);

      // destructor
      virtual ~eEmSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
   
   private:

      parType_t      m_numberOfeEms = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_iso = { 0 };   
      parType_t      m_doIsoCut = { 0 };
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
