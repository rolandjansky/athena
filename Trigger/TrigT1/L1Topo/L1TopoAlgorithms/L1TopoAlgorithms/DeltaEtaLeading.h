/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaEtaLeading.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaEtaLeading__
#define __TopoCore__DeltaEtaLeading__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaEtaLeading : public DecisionAlg {
   public:
      DeltaEtaLeading(const std::string & name);
      virtual ~DeltaEtaLeading(){};
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode initialize();

   };
   
}

#endif
