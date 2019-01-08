/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  METSortingAlg.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__METSortingAlg
#define TCS__METSortingAlg

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class METSortingAlg : public SortingAlg {
   public:
      
      // constructor
      METSortingAlg(const std::string & name);

      // destructor
      virtual ~METSortingAlg();

      virtual StatusCode initialize();
      
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:

      parType_t  p_MinEta = { 0 };
      parType_t  p_MaxEta = { 31 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
