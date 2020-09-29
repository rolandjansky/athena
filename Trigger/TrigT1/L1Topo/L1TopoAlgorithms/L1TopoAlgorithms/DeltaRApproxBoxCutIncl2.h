/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRApproxBoxCutIncl2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaRApproxBoxCutIncl2__
#define __TopoCore__DeltaRApproxBoxCutIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class DeltaRApproxBoxCutIncl2 : public DecisionAlg {
   public:
      DeltaRApproxBoxCutIncl2(const std::string & name);
      virtual ~DeltaRApproxBoxCutIncl2();

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
