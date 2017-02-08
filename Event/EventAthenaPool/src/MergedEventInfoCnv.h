/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

typedef T_AthenaPoolTPCnvCnv<MergedEventInfo,
                             MergedEventInfoCnv_p2,
                             MergedEventInfoCnv_p1,
                             T_TPCnvNull<MergedEventInfo> >
  MergedEventInfoCnv;


#endif

