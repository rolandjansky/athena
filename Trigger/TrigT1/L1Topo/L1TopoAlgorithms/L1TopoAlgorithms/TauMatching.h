/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  TauMatching.h
//  TopoCore

#ifndef TCS__TauMatching
#define TCS__TauMatching

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class TauMatching : public SortingAlg {
   public:
      
      // constructor
      TauMatching(const std::string & name);

      // destructor
      virtual ~TauMatching();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:

      parType_t      m_numberOfcTaus = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
      parType_t      m_rCore = { 0 };   
      parType_t      m_rHad = { 0 };   
      
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
