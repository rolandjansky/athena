/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MinDPhiIncl2.h
//  TopoCore
//  Created by Veronica Sorin 

#ifndef __TopoCore__MinDPhiIncl2__
#define __TopoCore__MinDPhiIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class MinDPhiIncl2 : public DecisionAlg {
   public:
      MinDPhiIncl2(const std::string & name);
      virtual ~MinDPhiIncl2();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_DeltaPhiMin[3] = {0, 0, 0};
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };


   };
   
}

#endif
