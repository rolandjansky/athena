/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DisambiguationInvmIncl2.h
//  TopoCore
//  Created by Davide Gerbaudo on 2016-10-10.

#ifndef __TopoCore__DisambiguationInvmIncl2__
#define __TopoCore__DisambiguationInvmIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {

  class DisambiguationInvmIncl2 : public DecisionAlg {
  public:
    DisambiguationInvmIncl2(const std::string & name);
    virtual ~DisambiguationInvmIncl2();
    virtual StatusCode initialize();
    virtual StatusCode processBitCorrect(const std::vector<TCS::TOBArray const *> & input,
                                         const std::vector<TCS::TOBArray *> & output,
                                         Decision & decison);
    virtual StatusCode process(const std::vector<TCS::TOBArray const *> & input,
                               const std::vector<TCS::TOBArray *> & output,
                               Decision & decison);
  private:
    parType_t  p_NumberLeading1 = { 0 };
    parType_t  p_NumberLeading2 = { 0 };
    parType_t  p_MinET1    [2] = { 0, 0 };
    parType_t  p_MinET2    [2] = { 0, 0 };
    parType_t  p_InvMassMin[2] = { 0, 0 };
    parType_t  p_InvMassMax[2] = { 0, 0 };
  };
}

#endif
