/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetSelect.h
//  TopoCore
//  Created by Veronica Sorin  14/8/14

#ifndef __TopoCore__JetSelect__
#define __TopoCore__JetSelect__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetSelect : public SortingAlg {
   public:
      
      // constructor
      JetSelect(const std::string & name);

      // destructor
      virtual ~JetSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
      parType_t      m_jsize = { 0 };   
      parType_t      m_doEtaCut = { 0 };

   protected:
    
      JetTOB::JetSize m_jetsize;
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
