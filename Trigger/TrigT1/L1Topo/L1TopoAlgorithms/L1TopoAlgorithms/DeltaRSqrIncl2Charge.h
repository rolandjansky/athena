/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRSqrIncl2Charge.h
//  TopoCore
//  Based on DeltaRSqrIncl2 by Joerg Stelzer/V Sorin on 9/16/14.

#ifndef L1TOPOALGORITHMS_DELTARSQRINCL2CHARGE_H
#define L1TOPOALGORITHMS_DELTARSQRINCL2CHARGE_H

#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaRSqrIncl2Charge : public DecisionAlg {
   public:
      DeltaRSqrIncl2Charge(const std::string & name);
      virtual ~DeltaRSqrIncl2Charge();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      m_NumberLeading1 = { 0 };
      parType_t      m_NumberLeading2 = { 0 };
      parType_t      m_DeltaRMin[3] = {0, 0, 0};
      parType_t      m_DeltaRMax[3] = {0, 0, 0};
      parType_t      m_MinET1[3] = { 0,0,0 };
      parType_t      m_MinET2[3] = { 0,0,0 };

   };
   
}

#endif
