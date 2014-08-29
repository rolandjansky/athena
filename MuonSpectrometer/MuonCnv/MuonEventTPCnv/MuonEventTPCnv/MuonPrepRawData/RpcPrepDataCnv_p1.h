/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATACNV_P1_H
#define RPCPREPDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/RpcPrepData.h"
#include "RpcPrepData_p1.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class RpcPrepDataCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::RpcPrepData, Muon::RpcPrepData_p1 >
{
public:
    RpcPrepDataCnv_p1() :m_prdCnv(0) {}

    void persToTrans(   const Muon::RpcPrepData_p1  *persObj,
                        Muon::RpcPrepData           *transObj,
                        MsgStream                   &log );
    void transToPers(   const Muon::RpcPrepData     *transObj,
                        Muon::RpcPrepData_p1        *persObj,
                        MsgStream                   &log );

protected:        
    PrepRawDataCnv_p1     *m_prdCnv;    
};

#endif 
