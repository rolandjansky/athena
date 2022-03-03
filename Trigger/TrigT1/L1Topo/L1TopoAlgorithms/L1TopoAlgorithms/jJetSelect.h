/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jJetSelect.h
//  TopoCore

#ifndef __TopoCore__jJetSelect__
#define __TopoCore__jJetSelect__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jJetSelect : public SortingAlg {
   public:
      
      // constructor
      jJetSelect(const std::string & name);

      // destructor
      virtual ~jJetSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
