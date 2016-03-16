/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWBETACONTAINER_TLP1_H
#define INDETLOWBETACONTAINER_TLP1_H

#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_p1.h"
//#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainerCnv_tlp1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"

class InDetLowBetaContainer_tlp1
{
 public:
  InDetLowBetaContainer_tlp1() {}
  //  friend class InDetLowBetaContainerCnv_tlp1;
  
  //private:
  std::vector<InDetLowBetaContainer_p1> m_lowBetaContainers;
  std::vector<InDetLowBetaCandidate_p1> m_lowBetaCandidate;
};



#endif



