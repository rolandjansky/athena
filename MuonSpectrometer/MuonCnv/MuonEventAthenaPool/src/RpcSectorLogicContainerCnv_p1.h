/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCSECTORLOGICCONTAINERCNV_P1
#define MUONEVENTATHENAPOOL_RPCSECTORLOGICCONTAINERCNV_P1

//-----------------------------------------------------------------------------
//
// file:   RpcSectorLogicContainerCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonEventAthenaPool/RpcSectorLogicContainer_p1.h"

class IRPCcablingSvc;
class MsgStream;

class RpcSectorLogicContainerCnv_p1 : public T_AthenaPoolTPCnvBase< RpcSectorLogicContainer, RpcSectorLogicContainer_p1 >

{
public:
    typedef RpcSectorLogicContainer_p1 PERS;  
    typedef RpcSectorLogicContainer TRANS;
    RpcSectorLogicContainerCnv_p1() {}
    virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
    virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
private:

};

#endif 



