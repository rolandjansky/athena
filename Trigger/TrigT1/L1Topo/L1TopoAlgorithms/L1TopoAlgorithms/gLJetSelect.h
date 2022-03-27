/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gLJetSelect.h
//  TopoCore

#ifndef L1TOPOALGORITHMS_GLJETSELECT_H
#define L1TOPOALGORITHMS_GLJETSELECT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class gLJetSelect : public SortingAlg {
   public:
      
      // constructor
      gLJetSelect(const std::string & name);

      // destructor
      virtual ~gLJetSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);    
    
   private:
   
      parType_t      m_numberOfgLJets = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
