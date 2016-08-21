/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATACNV_P3_TRK_H
#define RPCPREPDATACNV_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p3.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/RpcPrepData.h"
#include "RpcPrepData_p3.h"

class MsgStream;

class RpcPrepDataCnv_p3
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::RpcPrepData, Muon::RpcPrepData_p3 >
{
    public:
  RpcPrepDataCnv_p3() {}

  Muon::RpcPrepData
  createRpcPrepData( const Muon::RpcPrepData_p3 *persObj,
                     const Identifier& id,
                     const MuonGM::RpcReadoutElement* detEl,
                     MsgStream & log );

  void persToTrans( const Muon::RpcPrepData_p3 *persObj,
                    Muon::RpcPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::RpcPrepData    *transObj,
                    Muon::RpcPrepData_p3 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
