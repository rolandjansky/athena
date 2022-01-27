/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_RDOCnv_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Transient/Persistent converter for the HGTD_RDO class.
 *
 */

#ifndef HGTD_EVENTTPCNV_HGTD_RDOCNV_P1_H
#define HGTD_EVENTTPCNV_HGTD_RDOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HGTD_EventTPCnv/HGTD_RDO_p1.h"
#include "HGTD_RawData/HGTD_RDO.h"

class MsgStream;

class HGTD_RDOCnv_p1
    : public T_AthenaPoolTPCnvBase<HGTD_RDO, HGTD_RDO_p1> {
public:
  HGTD_RDOCnv_p1() = default;

  void persToTrans(const HGTD_RDO_p1* pers_obj, HGTD_RDO* trans_obj,
                   MsgStream& log);

  void transToPers(const HGTD_RDO* trans_obj, HGTD_RDO_p1* pers_obj,
                   MsgStream& log);
};

#endif // HGTD_EVENTTPCNV_HGTD_RDOCNV_P1_H
