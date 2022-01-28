/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jLJetSelect.h
//  TopoCore

#ifndef __TopoCore__jLJetSelect__
#define __TopoCore__jLJetSelect__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jLJetSelect : public SortingAlg {
   public:
      
      // constructor
      jLJetSelect(const std::string & name);

      // destructor
      virtual ~jLJetSelect();
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
