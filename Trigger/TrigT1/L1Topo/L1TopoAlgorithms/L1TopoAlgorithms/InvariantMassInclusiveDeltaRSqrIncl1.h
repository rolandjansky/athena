/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusiveDeltaRSqrIncl1.h
//  TopoCore
//  Based on InvariantMassInclusive1 and DeltaRSqrIncl1 created by Joerg Stelzer and V Sorin. 01/03/2019.
//  For questions contact atlas-trig-l1topo-algcom@cern.ch. 

#ifndef __TopoCore__InvariantMassInclusiveDeltaRSqrIncl1__
#define __TopoCore__InvariantMassInclusiveDeltaRSqrIncl1__

#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusiveDeltaRSqrIncl1 : public DecisionAlg {
   public:
      InvariantMassInclusiveDeltaRSqrIncl1(const std::string & name);
      virtual ~InvariantMassInclusiveDeltaRSqrIncl1();

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

      TH2 * m_histAcceptM[6] = {};
      TH2 * m_histRejectM[6] = {};

   };
   
}

#endif
