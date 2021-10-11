/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  jLargeRJetSelect.h
//  TopoCore

#ifndef __TopoCore__jLargeRJetSelect__
#define __TopoCore__jLargeRJetSelect__

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jLargeRJetSelect : public SortingAlg {
   public:
      
      // constructor
      jLargeRJetSelect(const std::string & name);

      // destructor
      virtual ~jLargeRJetSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
      parType_t      m_doEtaCut = { 0 };
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
