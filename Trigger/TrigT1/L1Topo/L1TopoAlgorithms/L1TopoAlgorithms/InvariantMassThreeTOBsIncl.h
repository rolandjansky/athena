/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  InvariantMassThreeTOBsIncl.h
//  TopoCore
//  Created by Carlos Moreno based on InvariantMassInclusive1 by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__InvariantMassThreeTOBsIncl__
#define __TopoCore__InvariantMassThreeTOBsIncl__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class InvariantMassThreeTOBsIncl : public DecisionAlg {
   public:
      InvariantMassThreeTOBsIncl(const std::string & name);
      virtual ~InvariantMassThreeTOBsIncl();

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

      TH1 * m_histAcceptINVThree[6] = {};
      TH1 * m_histRejectINVThree[6] = {};

   };
   
}

#endif
