/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_RDOContainerCnv_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief
 *
 */

#ifndef HGTD_EVENTTPCNV_HGTD_RDOCONTAINERCNV_P1_H
#define HGTD_EVENTTPCNV_HGTD_RDOCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HGTD_EventTPCnv/HGTD_RDOContainer_p1.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"

class HGTD_ID;
class StoreGateSvc;

class HGTD_RDOContainerCnv_p1
    : public T_AthenaPoolTPCnvBase<HGTD_RDOContainer,
                                   HGTD_RDOContainer_p1> {

public:
  typedef HGTD_RDOContainer_p1 Pers_t;
  typedef HGTD_RDOContainer Trans_t;

  HGTD_RDOContainerCnv_p1() : m_is_initialized(false) {}

  virtual void persToTrans(const Pers_t* persistent_container,
                           Trans_t* transient_container, MsgStream& log);
  virtual void transToPers(const Trans_t* transient_container,
                           Pers_t* persistent_container, MsgStream& log);

  virtual Trans_t* createTransient(const Pers_t* persistent_container,
                                   MsgStream& log);

private:
  StatusCode initialize(MsgStream& log);

  const HGTD_ID* m_hgtd_idhelper;

  bool m_is_initialized;
};

#endif // HGTD_EVENTTPCNV_HGTD_RDOCONTAINERCNV_P1_H
