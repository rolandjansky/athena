/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetAbbreviated.h
//  TopoCore
//  Created by Veronica Sorin  14/8/14

#ifndef __TopoCore__JetAbbreviated__
#define __TopoCore__JetAbbreviated__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetAbbreviated : public SortingAlg {
   public:
      
      // constructor
      JetAbbreviated(const std::string & name);

      // destructor
      virtual ~JetAbbreviated();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   protected:
    
      JetTOB::JetSize m_jetsize;
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
