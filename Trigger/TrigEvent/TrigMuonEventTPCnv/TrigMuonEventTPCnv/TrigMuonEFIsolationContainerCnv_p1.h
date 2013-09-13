/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P1_H__
#define TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P1_H__

/**
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFIsolationContainerCnv_p1
 *
 * @brief T/P Converter for TrigMuonEFIsolationContainer_p1
 *
 * @author Mark Owen <markowen@cern.ch>
 */

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p1.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p1.h"


typedef T_AthenaPoolTPCnvVector< TrigMuonEFIsolationContainer, TrigMuonEFIsolationContainer_p1, TrigMuonEFIsolationCnv_p1 > TrigMuonEFIsolationContainerCnv_p1;

#endif //TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINERCNV_P1_H__
