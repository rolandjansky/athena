//  JetHT.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__JetHT__
#define __TopoCore__JetHT__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class JetHT : public DecisionAlg {
   public:
      JetHT(const std::string & name);
      virtual ~JetHT();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_MinET = { 0 };

   };
   
}

#endif
