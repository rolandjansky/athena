/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  SortingAlg.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/16/12.
//

#ifndef __TopoCore__BaseSortingAlg__
#define __TopoCore__BaseSortingAlg__

#include "L1TopoCoreSimulation/ConfigurableAlg.h"
#include "L1TopoCoreSimulation/InputTOBArray.h"

#include "L1TopoCoreSimulation/TOBArray.h"

#include "L1TopoCoreSimulation/StatusCode.h"

namespace TCS {

   class SortingAlg : public ConfigurableAlg {
   public:
      SortingAlg(const  std::string & name) :
         ConfigurableAlg(name)
      {};

      virtual bool isSortingAlg() const { return true; }

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output) = 0;
   protected:

      virtual StatusCode doReset();
      
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__BaseSortingAlg__) */
