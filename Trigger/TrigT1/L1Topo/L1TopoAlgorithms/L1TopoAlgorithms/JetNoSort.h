/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetNoSort.h
//  TopoCore
//  Created by Veronica Sorin  14/8/14

#ifndef __TopoCore__JetNoSort__
#define __TopoCore__JetNoSort__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetNoSort : public SortingAlg {
   public:
      
      // constructor
      JetNoSort(const std::string & name);

      // destructor
      virtual ~JetNoSort();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_jsize = { 0 };   
    

   protected:
    
      JetTOB::JetSize m_jetsize;
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
