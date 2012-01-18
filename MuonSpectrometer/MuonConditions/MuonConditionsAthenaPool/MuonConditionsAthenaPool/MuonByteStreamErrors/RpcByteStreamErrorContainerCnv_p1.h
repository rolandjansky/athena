/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINERCNV_P1_H
#define MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
// file:   RpcByteStreamErrorContainerCnv_p1.h
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#include "RpcByteStreamErrorContainer_p1.h"

class MsgStream;

class RpcByteStreamErrorContainerCnv_p1
   : public T_AthenaPoolTPCnvBase< Muon::RpcByteStreamErrorContainer, Muon::RpcByteStreamErrorContainer_p1 >
{
    public:
  RpcByteStreamErrorContainerCnv_p1()  {}

  void persToTrans( const Muon::RpcByteStreamErrorContainer_p1 *persObj,
                    Muon::RpcByteStreamErrorContainer    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::RpcByteStreamErrorContainer    *transObj,
                    Muon::RpcByteStreamErrorContainer_p1 *persObj,
                    MsgStream                &log );
};


template<>
class T_TPCnv<Muon::RpcByteStreamErrorContainer, Muon::RpcByteStreamErrorContainer_p1>
 : public RpcByteStreamErrorContainerCnv_p1
{
};


#endif
