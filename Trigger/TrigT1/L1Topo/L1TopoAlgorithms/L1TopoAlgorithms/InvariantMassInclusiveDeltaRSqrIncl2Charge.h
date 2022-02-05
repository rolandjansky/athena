/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusiveDeltaRSqrIncl2Charge.h
//  TopoCore
//  Based on InvariantMassInclusive2 and DeltaRSqrIncl2Charge created by Joerg Stelzer and V Sorin. 01/03/2019/
//  For questions contact atlas-trig-l1topo-algcom@cern.ch. 

#ifndef L1TOPOALGORITHMS_INVARIANTMASSINCLUSIVEDELTARSQRINCL2CHARGE_H
#define L1TOPOALGORITHMS_INVARIANTMASSINCLUSIVEDELTARSQRINCL2CHARGE_H

#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusiveDeltaRSqrIncl2Charge : public DecisionAlg {
   public:
      InvariantMassInclusiveDeltaRSqrIncl2Charge(const std::string & name);
      virtual ~InvariantMassInclusiveDeltaRSqrIncl2Charge();

      virtual StatusCode initialize() override final;
 
      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      

   private:

      parType_t      m_NumberLeading1 = { 0 };
      parType_t      m_NumberLeading2 = { 0 };
      parType_t      m_InvMassMin[6] = { 0,0,0,0,0,0 };
      parType_t      m_InvMassMax[6] = { 0,0,0,0,0,0 };
      parType_t      m_MinET1[6] = { 0,0,0,0,0,0 };
      parType_t      m_MinET2[6] = { 0,0,0,0,0,0 };
      parType_t      m_ApplyEtaCut = { 0 };
      parType_t      m_MinEta1 = { 0 };
      parType_t      m_MaxEta1 = { 0 };
      parType_t      m_MinEta2 = { 0 };
      parType_t      m_MaxEta2 = { 0 };
      parType_t      m_DeltaRMin[6] = { 0,0,0,0,0,0 };
      parType_t      m_DeltaRMax[6] = { 0,0,0,0,0,0 };

   };
   
}

#endif
