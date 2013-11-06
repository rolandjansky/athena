/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// the latest persistent representation type of PileUpEventInfo
typedef  PileUpEventInfo_p5  PileUpEventInfo_PERS;

typedef  T_AthenaPoolCustomCnv<PileUpEventInfo, PileUpEventInfo_PERS >   PileUpEventInfoCnvBase;

class PileUpEventInfoCnv : public PileUpEventInfoCnvBase {
friend class CnvFactory<PileUpEventInfoCnv >;
protected:
  PileUpEventInfoCnv (ISvcLocator* svcloc) : PileUpEventInfoCnvBase(svcloc) {}
  virtual PileUpEventInfo_PERS*   createPersistent (PileUpEventInfo* transObj);
  virtual PileUpEventInfo*        createTransient ();
};

#endif

