/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassThreeTOBsIncl1.h
//  TopoCore
//  Created by Carlos Moreno based on InvariantMassInclusive1 by Joerg Stelzer on 11/16/12.

#ifndef L1TOPOALGORITHMS_INVARIANTMASSTHREETOBSINCL1_H
#define L1TOPOALGORITHMS_INVARIANTMASSTHREETOBSINCL1_H

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class InvariantMassThreeTOBsIncl1 : public DecisionAlg {
   public:
      InvariantMassThreeTOBsIncl1(const std::string & name);
      virtual ~InvariantMassThreeTOBsIncl1();

      virtual StatusCode initialize() override final;

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_InvMassMin[6] = {0, 0, 0,0,0,0};
      parType_t      p_InvMassMax[6] = {0, 0, 0,0,0,0};
      parType_t      p_MinET1[6] = { 0 ,0,0,0,0,0};
      parType_t      p_OneBarrel = { 0 };

   };
   
}

#endif
