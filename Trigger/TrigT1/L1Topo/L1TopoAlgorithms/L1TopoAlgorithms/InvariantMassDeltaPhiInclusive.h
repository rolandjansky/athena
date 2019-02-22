/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassDeltaPhiInclusive.h
//  TopoCore
//  Based on InvariantMassInclusive2 and DeltaPhiIncl2 by Joerg Stelzer on 19/02/2019. For questions contact atlas-trig-l1topo-algcom@cern.ch. 
//  TO DO size of the input list to be possbly refined 

#ifndef __TopoCore__InvariantMassDeltaPhiInclusive__
#define __TopoCore__InvariantMassDeltaPhiInclusive__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassDeltaPhiInclusive : public DecisionAlg {
   public:
      InvariantMassDeltaPhiInclusive(const std::string & name);
      virtual ~InvariantMassDeltaPhiInclusive();

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
      parType_t      p_InvMassMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_InvMassMax[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET2[6] = { 0,0,0,0,0,0 };
      parType_t      p_ApplyEtaCut = { 0 };
      parType_t      p_MinEta1 = { 0 };
      parType_t      p_MaxEta1 = { 0 };
      parType_t      p_MinEta2 = { 0 };
      parType_t      p_MaxEta2 = { 0 };
      parType_t      p_DeltaPhiMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_DeltaPhiMax[6] = { 0,0,0,0,0,0 };

      TH1 * m_histAcceptM[6] = {};
      TH1 * m_histRejectM[6] = {};
      TH1 * m_histAcceptDPhi[6] = {};
      TH1 * m_histRejectDPhi[6] = {};
      TH2 * m_histAcceptEta1Eta2[6] = {};
      TH2 * m_histRejectEta1Eta2[6] = {};
   };
   
}

#endif
