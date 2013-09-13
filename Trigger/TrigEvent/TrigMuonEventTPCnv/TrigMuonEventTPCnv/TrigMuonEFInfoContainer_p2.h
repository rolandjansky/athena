/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainer_p2
 *
 * @brief persistent partner for TrigMuonEFInfoContainer
 *
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
  
class TrigMuonEFInfoContainer_p2 : public std::vector<TPObjRef> {
 public:
  TrigMuonEFInfoContainer_p2(){}

};
 
 
#endif
