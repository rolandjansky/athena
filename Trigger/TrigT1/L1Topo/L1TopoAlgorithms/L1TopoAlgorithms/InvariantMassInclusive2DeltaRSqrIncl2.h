/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusive2DeltaRSqrIncl2.h
//  TopoCore
//  Based on InvariantMassInclusive2 and DeltaRSqrIncl2 created by Joerg Stelzer and V Sorin. 01/03/2019/
//  For questions contact atlas-trig-l1topo-algcom@cern.ch. 

#ifndef __TopoCore__InvariantMassInclusive2DeltaRSqrIncl2__
#define __TopoCore__InvariantMassInclusive2DeltaRSqrIncl2__

#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusive2DeltaRSqrIncl2 : public DecisionAlg {
   public:
      InvariantMassInclusive2DeltaRSqrIncl2(const std::string & name);
      virtual ~InvariantMassInclusive2DeltaRSqrIncl2();

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

      TH1 * m_histAcceptM[6] = {};
      TH1 * m_histRejectM[6] = {};
      TH1 * m_histAcceptDR[6] = {};
      TH1 * m_histRejectDR[6] = {};
      TH2 * m_histAcceptEta1Eta2[6] = {};
      TH2 * m_histRejectEta1Eta2[6] = {};
   };
   
}

#endif
