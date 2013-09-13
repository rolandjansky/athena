/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"

class MuonFeatureDetailsContainer_tlp1
{

 public:

  MuonFeatureDetailsContainer_tlp1(){}

  std::vector<MuonFeatureDetailsContainer_p1>       m_muonFeatureDetailsContainerVec;
  std::vector<MuonFeatureDetails_p1>                m_muonFeatureDetailsVec;

};//end of class definitions

#endif

