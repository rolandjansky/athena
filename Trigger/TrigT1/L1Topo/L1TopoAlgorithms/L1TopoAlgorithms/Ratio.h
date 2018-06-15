/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  Ratio.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__Ratio__
#define __TopoCore__Ratio__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class Ratio : public DecisionAlg {
   public:
      Ratio(const std::string & name);
      virtual ~Ratio();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 49 };
      parType_t      p_HT = { 0 };
      parType_t      p_Ratio[2] = { 0 , 0 };
      parType_t      p_isXE2 = { 0 };

   };
   
}

#endif
