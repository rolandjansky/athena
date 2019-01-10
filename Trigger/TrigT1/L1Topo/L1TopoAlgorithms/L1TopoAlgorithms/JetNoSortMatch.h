/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetNoSortMatch.h
//  TopoCore
//  Created by Veronica Sorin  14/8/14

#ifndef __TopoCore__JetNoSortMatch__
#define __TopoCore__JetNoSortMatch__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetNoSortMatch : public SortingAlg {
   public:
      
      // constructor
      JetNoSortMatch(const std::string & name);

      // destructor
      virtual ~JetNoSortMatch();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_jsize = { 0 };   
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
    

   protected:
    
      JetTOB::JetSize m_jetsize;
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
