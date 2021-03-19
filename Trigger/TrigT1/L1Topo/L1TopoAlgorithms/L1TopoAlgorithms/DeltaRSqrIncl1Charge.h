/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRSqrIncl1Charge.h
//  TopoCore
//  Created by Paula Martinez based on DeltaRSqrIncl1 by Joerg Stelzer/V Sorin.

#ifndef L1TOPOALGORITHMS_DELTARSQRINCL1CHARGE_H
#define L1TOPOALGORITHMS_DELTARSQRINCL1CHARGE_H

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class DeltaRSqrIncl1Charge : public DecisionAlg {
   public:
      DeltaRSqrIncl1Charge(const std::string & name);
      virtual ~DeltaRSqrIncl1Charge();

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
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_OneBarrel = { 0 };
     
   };
   
}

#endif
