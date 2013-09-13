/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTrackContainer_p2
 *
 * @brief persistent partner for TrigMuonEFInfoTrackContainer
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigMuonEFInfoTrackContainer_p2 : public std::vector<TPObjRef> {
 public:
  TrigMuonEFInfoTrackContainer_p2(){}
  friend class TrigMuonEFInfoTrackContainerCnv_p2;

};
#endif
