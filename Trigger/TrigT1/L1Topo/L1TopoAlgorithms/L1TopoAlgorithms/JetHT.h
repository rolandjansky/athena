/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetHT.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

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

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_MinET = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 49 };
      parType_t      p_HT[6] = { 0 , 0,0,0,0,0 };

   };
   
}

#endif
