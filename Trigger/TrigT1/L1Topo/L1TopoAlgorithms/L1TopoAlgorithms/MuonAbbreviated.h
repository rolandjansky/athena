/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MuonAbbreviated.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__MuonAbbreviated
#define TCS__MuonAbbreviated

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class MuonAbbreviated : public SortingAlg {
   public:
      
      // constructor
      MuonAbbreviated(const std::string & name);

      // destructor
      virtual ~MuonAbbreviated();

      virtual TCS::StatusCode initialize();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
      
   private:

      parType_t      m_numberOfMuons = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
