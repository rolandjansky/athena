/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jEmSort.h
//  TopoCore


#ifndef __TopoCore__jEmSort__
#define __TopoCore__jEmSort__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jEmSort : public SortingAlg {
   public:
      
      // constructor
      jEmSort(const std::string & name);

      // destructor
      virtual ~jEmSort();
      virtual TCS::StatusCode initialize(); 
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
