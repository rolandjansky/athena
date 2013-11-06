/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file MergedEventInfoCnv.h
 * @brief  AthenaPool converter for MergedEventInfo
 * @author Marcin.Nowak@cern.ch
 */

#ifndef MergedEventInfoCnv_H
#define MergedEventInfoCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventInfo/MergedEventInfo.h"
#include "EventTPCnv/MergedEventInfo_p1.h"
#include "EventTPCnv/MergedEventInfoCnv_p1.h"

#include "EventTPCnv/MergedEventInfo_p2.h"
#include "EventTPCnv/MergedEventInfoCnv_p2.h"

// the latest persistent representation type of MergedEventInfo
typedef  MergedEventInfo_p2  MergedEventInfo_PERS;

typedef  T_AthenaPoolCustomCnv<MergedEventInfo, MergedEventInfo_PERS >   MergedEventInfoCnvBase;

class MergedEventInfoCnv : public MergedEventInfoCnvBase {
friend class CnvFactory<MergedEventInfoCnv >;
protected:
  MergedEventInfoCnv (ISvcLocator* svcloc) : MergedEventInfoCnvBase(svcloc) {}
  virtual MergedEventInfo_PERS*   createPersistent (MergedEventInfo* transObj);
  virtual MergedEventInfo*        createTransient ();
};

#endif

