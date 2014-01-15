/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWBETACANDIDATE_TLP1_H
#define INDETLOWBETACANDIDATE_TLP1_H

#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"

class InDetLowBetaCandidate_tlp1
{
 public:
  InDetLowBetaCandidate_tlp1() {}
  friend class InDetLowBetaCandidateCnv_tlp1;
  
 private:
    std::vector<InDetLowBetaCandidate_p1> m_lowBetaCandidate;
};

#endif



