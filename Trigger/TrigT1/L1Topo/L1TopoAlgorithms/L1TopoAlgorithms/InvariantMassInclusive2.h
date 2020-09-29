/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassInclusive2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__InvariantMassInclusive2__
#define __TopoCore__InvariantMassInclusive2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

class TH2;

namespace TCS {
   
   class InvariantMassInclusive2 : public DecisionAlg {
   public:
      InvariantMassInclusive2(const std::string & name);
      virtual ~InvariantMassInclusive2();

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
      parType_t      p_MinET1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinET2[6] = { 0,0,0,0,0,0 };
      parType_t      p_ApplyEtaCut = { 0 };
      parType_t      p_MinEta1 = { 0 };
      parType_t      p_MaxEta1 = { 0 };
      parType_t      p_MinEta2 = { 0 };
      parType_t      p_MaxEta2 = { 0 };

   };
   
}

#endif
