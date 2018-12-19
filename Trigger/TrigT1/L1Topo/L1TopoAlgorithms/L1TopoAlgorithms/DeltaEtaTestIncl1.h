/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaEtaTestIncl1.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaEtaTestIncl1__
#define __TopoCore__DeltaEtaTestIncl1__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaEtaTestIncl1 : public DecisionAlg {
   public:
      DeltaEtaTestIncl1(const std::string & name);
      virtual ~DeltaEtaTestIncl1();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_DeltaEtaMin = { 0 };
      parType_t      p_DeltaEtaMax = { 0 };
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_EtaMin1 = { 0 };
      parType_t      p_EtaMax1 = { 63 };
      parType_t      p_EtaMin2 = { 0 };
      parType_t      p_EtaMax2 = { 63 };


   };
   
}

#endif
