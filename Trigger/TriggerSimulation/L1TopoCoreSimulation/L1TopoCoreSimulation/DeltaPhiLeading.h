//
//  DeltaPhiLeading.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#ifndef __TopoCore__DeltaPhiLeading__
#define __TopoCore__DeltaPhiLeading__

#include <iostream>
#include "L1TopoCoreSimulation/DecisionAlg.h"

namespace TCS {
   
   class DeltaPhiLeading : public DecisionAlg {
   public:
      DeltaPhiLeading(const std::string & name);
      virtual ~DeltaPhiLeading(){};
      
      virtual TCS::StatusCode process(const std::vector<TCS::TOBArray> & input,
                                      TCS::TOBArray & output, TCS::Decision & decison);
      
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__DeltaPhiLeading__) */
