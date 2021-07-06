/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassThreeTOBsInclCharge.h
//  TopoCore
//  Created by Paula Martinez based on InvariantMassInclusive1 by Joerg Stelzer on 11/16/12.

#ifndef L1TOPOALGORITHMS_INVARIANTMASSTHREETOBSINCLCHARGE_H
#define L1TOPOALGORITHMS_INVARIANTMASSTHREETOBSINCLCHARGE_H

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class InvariantMassThreeTOBsInclCharge : public DecisionAlg {
   public:
      InvariantMassThreeTOBsInclCharge(const std::string & name);
      virtual ~InvariantMassThreeTOBsInclCharge();

      virtual StatusCode initialize() override final;

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_InvMassMin[6] = {0, 0, 0, 0, 0, 0};
      parType_t      p_InvMassMax[6] = {0, 0, 0, 0, 0, 0};
      parType_t      p_MinET1[6] = { 0, 0, 0, 0, 0, 0};
      parType_t      p_OneBarrel = { 0 };

   };
   
}

#endif
