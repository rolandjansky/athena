//  DeltaRApproxBoxCutIncl1.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__DeltaRApproxBoxCutIncl1__
#define __TopoCore__DeltaRApproxBoxCutIncl1__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaRApproxBoxCutIncl1 : public DecisionAlg {
   public:
      DeltaRApproxBoxCutIncl1(const std::string & name);
      virtual ~DeltaRApproxBoxCutIncl1();

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
      parType_t      p_DeltaPhiMin[3] = {0, 0, 0};
      parType_t      p_DeltaPhiMax[3] = {0, 0, 0};
      parType_t      p_DeltaEtaMin[3] = {0, 0, 0};
      parType_t      p_DeltaEtaMax[3] = {0, 0, 0};
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };

   };
   
}

#endif
