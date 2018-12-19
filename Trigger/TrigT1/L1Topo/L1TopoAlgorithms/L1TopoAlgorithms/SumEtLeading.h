/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  SumEtLeading.h
//  TopoCore

#ifndef __TopoCore__SumEtLeading__
#define __TopoCore__SumEtLeading__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class SumEtLeading : public DecisionAlg {
   public:

      SumEtLeading(const std::string & name);

      virtual ~SumEtLeading() {};
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );

      virtual StatusCode initialize();

   private:

      parType_t p_NumberLeading { 0 };
      parType_t p_EtMin { 0 };
      parType_t p_HtMin { 0 };
      parType_t p_EtaMax { 0 };

   };
}

#endif
