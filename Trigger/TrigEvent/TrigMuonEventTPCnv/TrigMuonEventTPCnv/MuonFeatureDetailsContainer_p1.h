/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class MuonFeatureDetailsContainer_p1 : public std::vector<TPObjRef>
{
 public:

  MuonFeatureDetailsContainer_p1(){}
  friend class MuonFeatureDetailsContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_muonFeatureDetails;

};// end of class definitions


#endif
