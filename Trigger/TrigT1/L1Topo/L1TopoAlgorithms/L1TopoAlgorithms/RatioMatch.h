/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  RatioMatch.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__RatioMatch__
#define __TopoCore__RatioMatch__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class RatioMatch : public DecisionAlg {
   public:
      RatioMatch(const std::string & name);
      virtual ~RatioMatch();

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
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
//      parType_t      p_EtaMin = { 0 };
//      parType_t      p_EtaMax = { 49 };
//      parType_t      p_DeltaR = { 0 };
      parType_t      p_Ratio[2] = { 0 , 0 };

   };
   
}

#endif
