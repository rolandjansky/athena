/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  TransverseMassInclusive1.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__TransverseMassInclusive1__
#define __TopoCore__TransverseMassInclusive1__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class TransverseMassInclusive1 : public DecisionAlg {
   public:
      TransverseMassInclusive1(const std::string & name);
      virtual ~TransverseMassInclusive1();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_TMassMin[6] = {0, 0, 0,0,0,0};
      parType_t      p_MinET1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET2[6] = { 0,0,0,0,0,0 };

      TH1 * m_histAcceptM[6] = {};
      TH1 * m_histRejectM[6] = {};
   };
   
}

#endif
