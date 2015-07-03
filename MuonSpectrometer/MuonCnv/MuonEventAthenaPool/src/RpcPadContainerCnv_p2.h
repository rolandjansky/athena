/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class IRPCcablingSvc;
class MsgStream;

class RpcPadContainerCnv_p2 : public T_AthenaPoolTPCnvBase< RpcPadContainer, RpcPadContainer_p2 >

{
public:
    typedef RpcPadContainer_p2 PERS;  
    typedef RpcPadContainer TRANS;
    RpcPadContainerCnv_p2(): m_rpcCabling(0), m_isInitialized(false), m_padhashmax(0)/*, m_errorCount(0), m_maxNumberOfErrors(10)*/ {}
    virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
    virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
    virtual RpcPadContainer* createTransient(const RpcPadContainer_p2* persObj, MsgStream& log);
private:
    StatusCode initialize(MsgStream &log);

    const IRPCcablingSvc *m_rpcCabling;
    bool m_isInitialized;
    unsigned int m_padhashmax;
    //unsigned int m_errorCount;
    //unsigned int m_maxNumberOfErrors;
};

#endif 



