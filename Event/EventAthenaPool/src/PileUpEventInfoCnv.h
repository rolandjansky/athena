/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file PileUpEventInfoCnv.h
 * @brief AthenaPool converter forPileUpEventInfo
 * @author Marcin.Nowak@cern.ch
 */

#ifndef PileUpEventInfoCnv_H
#define PileUpEventInfoCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventTPCnv/PileUpEventInfo_p1.h"
#include "EventTPCnv/PileUpEventInfoCnv_p1.h"
#include "EventTPCnv/PileUpEventInfo_p2.h"
#include "EventTPCnv/PileUpEventInfoCnv_p2.h"

#include "EventTPCnv/PileUpEventInfo_p3.h"
#include "EventTPCnv/PileUpEventInfoCnv_p3.h"

#include "EventTPCnv/PileUpEventInfo_p4.h"
#include "EventTPCnv/PileUpEventInfoCnv_p4.h"

#include "EventTPCnv/PileUpEventInfo_p5.h"
#include "EventTPCnv/PileUpEventInfoCnv_p5.h"

typedef T_AthenaPoolTPCnvCnv<PileUpEventInfo,
                             PileUpEventInfoCnv_p5,
                             PileUpEventInfoCnv_p4,
                             PileUpEventInfoCnv_p3,
                             PileUpEventInfoCnv_p2,
                             PileUpEventInfoCnv_p1,
                             T_TPCnvNull<PileUpEventInfo> >
  PileUpEventInfoCnv;


#endif
