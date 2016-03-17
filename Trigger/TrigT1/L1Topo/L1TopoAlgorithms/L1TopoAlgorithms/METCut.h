//  METCut.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__METCut__
#define __TopoCore__METCut__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class METCut : public DecisionAlg {
   public:
      METCut(const std::string & name);
      virtual ~METCut();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_XE = { 0 };

   };
   
}

#endif
