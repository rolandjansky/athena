//  LAR.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__LAR__
#define __TopoCore__LAR__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class LAR : public DecisionAlg {
   public:
      LAR(const std::string & name);
      virtual ~LAR();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_MinET = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 0 };
      parType_t      p_PhiMax = { 0 };
      parType_t      p_PhiMin = { 0 };

   };
   
}

#endif
