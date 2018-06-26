/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaRLeading.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaRLeading__
#define __TopoCore__DeltaRLeading__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaRLeading : public DecisionAlg {
   public:
      DeltaRLeading(const std::string & name);
      virtual ~DeltaRLeading(){};
      
      virtual TCS::StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                       const std::vector<TCS::TOBArray *> & output,
                                       Decision & decison );
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__DeltaRLeading__) */
