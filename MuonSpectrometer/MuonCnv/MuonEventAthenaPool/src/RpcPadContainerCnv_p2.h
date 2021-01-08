/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCPADCONTAINERCNV_P2
#define MUONEVENTATHENAPOOL_RPCPADCONTAINERCNV_P2

//-----------------------------------------------------------------------------
//
// file:   RpcPadContainerCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPadContainer_p2.h"
#include "RPC_CondCabling/RpcCablingCondData.h"

class IRPCcablingSvc; // to be removed
class MsgStream;

class RpcPadContainerCnv_p2 : public T_AthenaPoolTPCnvBase< RpcPadContainer, RpcPadContainer_p2 >

{
public:
    typedef RpcPadContainer_p2 PERS;  
    typedef RpcPadContainer TRANS;
    RpcPadContainerCnv_p2();
    virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
    virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
    virtual RpcPadContainer* createTransient(const RpcPadContainer_p2* persObj, MsgStream& log);
    StatusCode initialize(MsgStream &log, IRPCcablingSvc* cabling = nullptr);
    void setRpcCablingCondData(const RpcCablingCondData* rpc){m_rpcCondData=rpc;}
private:

    const IRPCcablingSvc *m_rpcCabling;  // to be removed
    bool m_isInitialized;
    const RpcCablingCondData* m_rpcCondData;
};

#endif 



