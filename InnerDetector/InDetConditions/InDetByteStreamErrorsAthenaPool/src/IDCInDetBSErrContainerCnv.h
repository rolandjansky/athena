/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_IDCInDetBSErrContainerCnv_H
#define INDETBYTESTREAMERRORSATHENAPOOL_IDCInDetBSErrContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer64_p1.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "IDCInDetBSErrContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef  InDetBSErrContainer64_p1  IDCBSErrContainer_PERS;
typedef  T_AthenaPoolCustomCnv<IDCInDetBSErrContainer, IDCBSErrContainer_PERS >   IDCInDetBSErrContainerCnvBase;
// IDCInDetBSErrContainerCnv
class IDCInDetBSErrContainerCnv : public IDCInDetBSErrContainerCnvBase {
public:
  IDCInDetBSErrContainerCnv (ISvcLocator* svcloc) : IDCInDetBSErrContainerCnvBase(svcloc) {}

protected:
  virtual IDCBSErrContainer_PERS*   createPersistent (IDCInDetBSErrContainer* transCont);
  virtual IDCInDetBSErrContainer*   createTransient ();

  IDCInDetBSErrContainerCnv_p1  m_TPConverter;
};


#endif
