/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATACNV_p2_H
#define RPCPREPDATACNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/RpcPrepData.h"
#include "RpcPrepData_p2.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class RpcPrepDataCnv_p2
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::RpcPrepData, Muon::RpcPrepData_p2 >
{
public:
    RpcPrepDataCnv_p2() :m_prdCnv(0) {}

    void persToTrans(   const Muon::RpcPrepData_p2  *persObj,
                        Muon::RpcPrepData           *transObj,
                        MsgStream                   &log );
    void transToPers(   const Muon::RpcPrepData     *transObj,
                        Muon::RpcPrepData_p2        *persObj,
                        MsgStream                   &log );

protected:        
    PrepRawDataCnv_p1     *m_prdCnv;    
};

#endif 
