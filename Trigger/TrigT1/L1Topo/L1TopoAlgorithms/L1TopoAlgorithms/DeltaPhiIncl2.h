/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaPhiIncl2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaPhiIncl2__
#define __TopoCore__DeltaPhiIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaPhiIncl2 : public DecisionAlg {
   public:
      DeltaPhiIncl2(const std::string & name);
      virtual ~DeltaPhiIncl2();

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
      parType_t      p_DeltaPhiMin[2] = {0, 0};
      parType_t      p_DeltaPhiMax[2] = {0, 0};
      parType_t      p_MinET1[2] = { 0,0 };
      parType_t      p_MinET2[2] = { 0,0 };

   };
   
}

#endif
