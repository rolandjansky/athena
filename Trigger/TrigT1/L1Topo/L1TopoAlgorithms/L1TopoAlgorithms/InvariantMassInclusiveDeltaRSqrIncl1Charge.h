/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusiveDeltaRSqrIncl1Charge.h
//  TopoCore
//  Based on InvariantMassInclusiveDeltaRSqrIncl1, InvariantMassInclusive1 and DeltaRSqrIncl1 created by Joerg Stelzer and V Sorin.
//  For questions contact atlas-trig-l1topo-algcom@cern.ch. 

#ifndef L1TOPOALGORITHMS_INVARIANTMASSINCLUSIVEDELTARSQRINCL1CHARGE_H
#define L1TOPOALGORITHMS_INVARIANTMASSINCLUSIVEDELTARSQRINCL1CHARGE_H

#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusiveDeltaRSqrIncl1Charge : public DecisionAlg {
   public:
      InvariantMassInclusiveDeltaRSqrIncl1Charge(const std::string & name);
      virtual ~InvariantMassInclusiveDeltaRSqrIncl1Charge();

      virtual StatusCode initialize() override final;

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_InvMassMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_InvMassMax[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET2[6] = { 0,0,0,0,0,0 };
      parType_t      p_OneBarrel = { 0 };
      parType_t      p_DeltaRMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_DeltaRMax[6] = { 0,0,0,0,0,0 };

   };
   
}

#endif
