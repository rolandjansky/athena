/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DisambiguationDRIncl3.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DisambiguationDRIncl3__
#define __TopoCore__DisambiguationDRIncl3__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DisambiguationDRIncl3 : public DecisionAlg {
   public:
      DisambiguationDRIncl3(const std::string & name);
      virtual ~DisambiguationDRIncl3();

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
      parType_t      p_MinET1[2] = { 0,0 };
      parType_t      p_MinET2[2] = { 0,0 };
      parType_t      p_MinET3[2] = { 0,0 };
      parType_t      p_DRCutMin[2] = { 0,0 };
      parType_t      p_DRCutMax[2] = { 0,0 };


   };
   
}

#endif
