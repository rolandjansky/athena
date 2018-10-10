/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MuonSelect.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__MuonSelect
#define TCS__MuonSelect

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class MuonSelect : public SortingAlg {
   public:
      
      // constructor
      MuonSelect(const std::string & name);

      // destructor
      virtual ~MuonSelect();

      virtual TCS::StatusCode initialize();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
      
   private:

      parType_t      m_numberOfMuons = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
