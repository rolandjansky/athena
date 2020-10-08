/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  SimpleCone.h

#ifndef __TopoCore__SimpleCone__
#define __TopoCore__SimpleCone__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class SimpleCone : public DecisionAlg {
   public:
      SimpleCone(const std::string & name);
      virtual ~SimpleCone();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_R2 = { 10*10 };
      parType_t      p_MinET = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 49 };
      parType_t      p_MinSumET[6] = { 0 , 0,0,0,0,0 };

   };
   
}

#endif
