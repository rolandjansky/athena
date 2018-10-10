/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DisambiguationDetaDPhiIncl3.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DisambiguationDetaDPhiIncl3__
#define __TopoCore__DisambiguationDetaDPhiIncl3__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DisambiguationDetaDPhiIncl3 : public DecisionAlg {
   public:
      DisambiguationDetaDPhiIncl3(const std::string & name);
      virtual ~DisambiguationDetaDPhiIncl3();

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
      parType_t      p_NumberLeading3 = { 0 };
      parType_t      p_DisambDR[2] = {0, 0};
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_MinET3 = { 0 };
      parType_t      p_EtaMin1 = { 0 };
      parType_t      p_EtaMax1 = { 0 };
      parType_t      p_EtaMin2 = { 0 };
      parType_t      p_EtaMax2 = { 0 };
      parType_t      p_EtaMin3 = { 0 };
      parType_t      p_EtaMax3 = { 0 };
      parType_t      p_DeltaPhiMin = { 0 };
      parType_t      p_DeltaPhiMax = { 0 };
      parType_t      p_DeltaEtaMin = { 0 };
      parType_t      p_DeltaEtaMax = { 0 };


   };
   
}

#endif
