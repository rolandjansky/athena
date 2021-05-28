/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCOINDATACNV_P4_H
#define TGCCOINDATACNV_P4_H

//-----------------------------------------------------------------------------
//
// file:   TgcCoinDataCnv_p4.h
//
//-----------------------------------------------------------------------------

#include "MuonTrigCoinData/TgcCoinData.h"
#include "TgcCoinData_p4.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;

class TgcCoinDataCnv_p4
    : public T_AthenaPoolTPPolyCnvBase< Muon::TgcCoinData, Muon::TgcCoinData, Muon::TgcCoinData_p4 >
{
public:
  //    TgcCoinDataCnv_p4() : m_coindataCnv(0) {}
    TgcCoinDataCnv_p4() : m_localPosCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Muon::TgcCoinData_p4 *persObj,
        Muon::TgcCoinData    *transObj,
        MsgStream                &log );
    void transToPers( const Muon::TgcCoinData    *transObj,
        Muon::TgcCoinData_p4 *persObj,
        MsgStream                &log );

 protected:        
    LocalPositionCnv_p1   *m_localPosCnv;
    ErrorMatrixCnv_p1     *m_errorMxCnv;

};

#endif 
