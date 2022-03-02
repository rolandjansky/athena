/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  MuonSort.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__MuonSort
#define TCS__MuonSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class MuonSort : public SortingAlg {
   public:
      
      // constructor
      MuonSort(const std::string & name);

      // destructor
      virtual ~MuonSort();

      virtual TCS::StatusCode initialize();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
      
   private:

      parType_t      m_numberOfMuons = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_InnerCoinCut   = { 0 };
      parType_t      m_FullStationCut = { 0 };
      parType_t      m_GoodMFieldCut  = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
