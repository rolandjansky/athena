/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusive1.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__InvariantMassInclusive1__
#define __TopoCore__InvariantMassInclusive1__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class InvariantMassInclusive1 : public DecisionAlg {
   public:
      InvariantMassInclusive1(const std::string & name);
      virtual ~InvariantMassInclusive1();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_InvMassMin[6] = {0, 0, 0,0,0,0};
      parType_t      p_InvMassMax[6] = {0, 0, 0,0,0,0};
      parType_t      p_MinET1[6] = { 0 ,0,0,0,0,0};
      parType_t      p_MinET2[6] = { 0 ,0,0,0,0,0};
      parType_t      p_OneBarrel = { 0 };

   };
   
}

#endif
