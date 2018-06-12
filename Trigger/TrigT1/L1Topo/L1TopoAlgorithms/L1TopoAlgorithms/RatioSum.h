/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  RatioSum.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__RatioSum__
#define __TopoCore__RatioSum__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class RatioSum : public DecisionAlg {
   public:
      RatioSum(const std::string & name);
      virtual ~RatioSum();

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

      parType_t      p_MinMET = { 0};
      parType_t      p_MinET2 = { 0 };
      parType_t      p_EtaMin2 = { 0 };
      parType_t      p_EtaMax2 = { 49 };
      parType_t      p_MinET3 = { 0 };
      parType_t      p_EtaMin3 = { 0 };
      parType_t      p_EtaMax3 = { 49 };

      parType_t      p_HT = { 0 };
      parType_t      p_SUM = { 0 };
      parType_t      p_Ratio[2] = { 0 , 0 };

   };
   
}

#endif
