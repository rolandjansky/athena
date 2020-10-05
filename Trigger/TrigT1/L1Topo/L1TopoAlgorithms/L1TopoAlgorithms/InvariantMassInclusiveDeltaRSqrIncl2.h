/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusiveDeltaRSqrIncl2.h
//  TopoCore
//  Based on InvariantMassInclusive2 and DeltaRSqrIncl2 created by Joerg Stelzer and V Sorin. 01/03/2019/
//  For questions contact atlas-trig-l1topo-algcom@cern.ch. 

#ifndef __TopoCore__InvariantMassInclusiveDeltaRSqrIncl2__
#define __TopoCore__InvariantMassInclusiveDeltaRSqrIncl2__

#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusiveDeltaRSqrIncl2 : public DecisionAlg {
   public:
      InvariantMassInclusiveDeltaRSqrIncl2(const std::string & name);
      virtual ~InvariantMassInclusiveDeltaRSqrIncl2();

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
      parType_t      p_ApplyEtaCut = { 0 };
      parType_t      p_MinEta1 = { 0 };
      parType_t      p_MaxEta1 = { 0 };
      parType_t      p_MinEta2 = { 0 };
      parType_t      p_MaxEta2 = { 0 };
      parType_t      p_DeltaRMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_DeltaRMax[6] = { 0,0,0,0,0,0 };

   };
   
}

#endif
