/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_RPCCOINDATACNV_P1_H
#define MUONEVENTTPCNV_RPCCOINDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RpcCoinDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonTrigCoinData/RpcCoinData.h"
#include "RpcCoinData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class RpcCoinDataCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Muon::RpcCoinData, Muon::RpcCoinData, Muon::RpcCoinData_p1 >
{
public:
    RpcCoinDataCnv_p1() {}

    Muon::RpcCoinData
    createRpcCoinData( const Muon::RpcCoinData_p1 *persObj,
                       const Identifier& id,
                       const MuonGM::RpcReadoutElement* detEl,
                       MsgStream &log );

    void persToTrans( const Muon::RpcCoinData_p1 *persObj,
        Muon::RpcCoinData    *transObj,
        MsgStream                &log );
    void transToPers( const Muon::RpcCoinData    *transObj,
        Muon::RpcCoinData_p1 *persObj,
        MsgStream                &log );
};

#endif 
