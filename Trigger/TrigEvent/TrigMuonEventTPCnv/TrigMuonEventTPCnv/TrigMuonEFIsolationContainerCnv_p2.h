/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P2_H__
#define TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P2_H__

/**
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFIsolationContainerCnv_p2
 *
 * @brief T/P Converter for TrigMuonEFIsolationContainer_p2
 *
 * @author Mark Owen <markowen@cern.ch>
 */

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p2.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"


typedef T_AthenaPoolTPCnvVector< TrigMuonEFIsolationContainer, TrigMuonEFIsolationContainer_p2, TrigMuonEFIsolationCnv_p2 > TrigMuonEFIsolationContainerCnv_p2;

#endif //TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P2_H__
