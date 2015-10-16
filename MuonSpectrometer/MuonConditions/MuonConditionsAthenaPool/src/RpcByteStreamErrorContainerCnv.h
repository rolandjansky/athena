/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINERCNV_H
#define MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainer_p1.h"

// the latest persistent representation type of RpcByteStreamErrorContainer
typedef  Muon::RpcByteStreamErrorContainer_p1  RpcByteStreamErrorContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::RpcByteStreamErrorContainer, RpcByteStreamErrorContainer_PERS >   RpcByteStreamErrorContainerCnvBase;

class RpcByteStreamErrorContainerCnv : public RpcByteStreamErrorContainerCnvBase {
friend class CnvFactory<RpcByteStreamErrorContainerCnv >;
protected:
  RpcByteStreamErrorContainerCnv (ISvcLocator* svcloc) : RpcByteStreamErrorContainerCnvBase(svcloc) {}
  virtual RpcByteStreamErrorContainer_PERS*   createPersistent (Muon::RpcByteStreamErrorContainer* transObj);
  virtual Muon::RpcByteStreamErrorContainer*  createTransient ();
};

#endif
