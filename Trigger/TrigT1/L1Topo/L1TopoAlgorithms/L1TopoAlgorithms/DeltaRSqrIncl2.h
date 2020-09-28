/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRSqrIncl2.h
//  TopoCore
//  Created by Joerg Stelzer/V Sorin on 9/16/14.

#ifndef __TopoCore__DeltaRSqrIncl2__
#define __TopoCore__DeltaRSqrIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaRSqrIncl2 : public DecisionAlg {
   public:
      DeltaRSqrIncl2(const std::string & name);
      virtual ~DeltaRSqrIncl2();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_DeltaRMin[3] = {0, 0, 0};
      parType_t      p_DeltaRMax[3] = {0, 0, 0};
      parType_t      p_MinET1[3] = { 0,0,0 };
      parType_t      p_MinET2[3] = { 0,0,0 };

   };
   
}

#endif
