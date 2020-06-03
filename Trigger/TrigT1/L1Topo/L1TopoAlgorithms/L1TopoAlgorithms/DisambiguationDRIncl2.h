/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DisambiguationDRIncl2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DisambiguationDRIncl2__
#define __TopoCore__DisambiguationDRIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DisambiguationDRIncl2 : public DecisionAlg {
   public:
      DisambiguationDRIncl2(const std::string & name);
      virtual ~DisambiguationDRIncl2();

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
      parType_t      p_MinET1[2] = { 0,0 };
      parType_t      p_MinET2[2] = { 0,0 };
      parType_t      p_DRCutMin[2] = { 0,0 };
      parType_t      p_DRCutMax[2] = { 0,0 };


   };
   
}

#endif
