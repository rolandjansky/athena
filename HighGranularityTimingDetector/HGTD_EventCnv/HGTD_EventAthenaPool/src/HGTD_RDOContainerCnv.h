/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventAthenaPool/src/HGTD_RDOContainerCnv.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief
 *
 */

#ifndef HGTD_EVENTATHENAPOOL_HGTD_RDOCONTAINERCNV_H
#define HGTD_EVENTATHENAPOOL_HGTD_RDOCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"

#include "HGTD_EventTPCnv/HGTD_RDOContainerCnv_p1.h"

// the latest persistent representation type of the container holding
// HGTD_RawData
typedef HGTD_RDOContainer_p1 HGTD_RDOContainer_PERS_t;

typedef HGTD_RDOContainerCnv_p1 HGTD_RDOContainerCNV_t;

typedef T_AthenaPoolCustomCnv<HGTD_RDOContainer, HGTD_RDOContainer_PERS_t>
    HGTD_RDOContainerCnvBase_t;

class HGTD_RDOContainerCnv : public HGTD_RDOContainerCnvBase_t {

  friend class CnvFactory<HGTD_RDOContainerCnv>;

public:

  HGTD_RDOContainerCnv(ISvcLocator* svcloc);

protected:

  virtual HGTD_RDOContainer_PERS_t*
  createPersistent(HGTD_RDOContainer* trans) override;

  virtual HGTD_RDOContainer* createTransient() override;

private:
  HGTD_RDOContainerCNV_t m_converter;
};

#endif // HGTD_CLUSTERCONTAINERCNV_H
