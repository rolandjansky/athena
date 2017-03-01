//  SortingAlg.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef TCS_SortingAlg
#define TCS_SortingAlg

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoCommon/StatusCode.h"

namespace TCS {

   class SortingAlg : public ConfigurableAlg {
   public:
      
      SortingAlg(const  std::string & name) :
         ConfigurableAlg(name, SORT)
      {};

      virtual ~SortingAlg();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output) = 0;
      virtual TCS::StatusCode sortBitCorrect(const InputTOBArray & input, TOBArray & output);

   protected:

      virtual StatusCode doReset();
      
   };

}

#endif
