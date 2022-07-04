/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

class MsgStream;

class RpcSectorLogicContainerCnv_p1 : public T_AthenaPoolTPCnvConstBase< RpcSectorLogicContainer, RpcSectorLogicContainer_p1 >

{
public:
    using base_class::persToTrans;
    using base_class::transToPers;
    typedef RpcSectorLogicContainer_p1 PERS;  
    typedef RpcSectorLogicContainer TRANS;
    RpcSectorLogicContainerCnv_p1() {}
    virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log) const override;
    virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log) const override;
private:

};

#endif 



