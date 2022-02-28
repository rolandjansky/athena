/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gJetSort.h
//  TopoCore

#ifndef __TopoCore__gJetSort__
#define __TopoCore__gJetSort__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class gJetSort : public SortingAlg {
   public:
      
      // constructor
      gJetSort(const std::string & name);

      // destructor
      virtual ~gJetSort();
      virtual TCS::StatusCode initialize(); 
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
