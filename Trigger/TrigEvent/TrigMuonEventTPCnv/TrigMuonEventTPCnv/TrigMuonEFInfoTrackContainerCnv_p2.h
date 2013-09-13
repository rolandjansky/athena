/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTraclContainerCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonEFInfoTrackContainer_p2
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_CNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p2.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

// class MsgStream;

class TrigMuonEFInfoTrackContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigMuonEFInfoTrackContainer, TrigMuonEFInfoTrackContainer_p2, ITPConverterFor< TrigMuonEFInfoTrack> > {

 public:
  TrigMuonEFInfoTrackContainerCnv_p2() {}

}; 

#endif
