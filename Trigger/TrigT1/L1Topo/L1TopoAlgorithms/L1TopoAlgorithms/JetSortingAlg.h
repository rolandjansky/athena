/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlg.h
//  TopoCore
//  Created by Christoper Willis 12/6/12

#ifndef __TopoCore__JetSortingAlg__
#define __TopoCore__JetSortingAlg__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetSortingAlg : public SortingAlg {
   public:
      
      // constructor
      JetSortingAlg(const std::string & name);

      // destructor
      virtual ~JetSortingAlg();
    
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
    
   protected:
    
      JetTOB::JetSize m_jetsize;
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
