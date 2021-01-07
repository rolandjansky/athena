/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPADCONTAINERCNV_P1
#define RPCPADCONTAINERCNV_P1

//-----------------------------------------------------------------------------
//
// file:   RpcPadContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPadContainer_p1.h"
#include "RPC_CondCabling/RpcCablingCondData.h"

class IRPCcablingSvc; // to be removed
class MsgStream;

class RpcPadContainerCnv_p1 : public T_AthenaPoolTPCnvBase< RpcPadContainer, RpcPadContainer_p1 >

{
public:
    typedef RpcPadContainer_p1 PERS;  
    typedef RpcPadContainer TRANS;
    RpcPadContainerCnv_p1();
    virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log) override; 
    virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log) override;
    virtual RpcPadContainer* createTransient(const RpcPadContainer_p1* persObj, MsgStream& log) override;
    StatusCode initialize(MsgStream &log, IRPCcablingSvc* cabling = nullptr);
    void setRpcCablingCondData(const RpcCablingCondData* rpc){m_rpcCondData=rpc;}
private:

    const IRPCcablingSvc *m_rpcCabling; // to be removed
    bool m_isInitialized;
    const RpcCablingCondData* m_rpcCondData;
};

#endif 



