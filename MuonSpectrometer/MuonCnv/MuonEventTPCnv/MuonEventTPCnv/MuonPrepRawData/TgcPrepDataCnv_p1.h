/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPREPDATACNV_P1_H
#define TGCPREPDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/TgcPrepData.h"
#include "TgcPrepData_p1.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class TgcPrepDataCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::TgcPrepData, Muon::TgcPrepData_p1 >
{
public:
    TgcPrepDataCnv_p1() : m_prdCnv(0) {}

    void persToTrans( const Muon::TgcPrepData_p1 *persObj,
        Muon::TgcPrepData    *transObj,
        MsgStream                &log );
    void transToPers( const Muon::TgcPrepData    *transObj,
        Muon::TgcPrepData_p1 *persObj,
        MsgStream                &log );

protected:        
  PrepRawDataCnv_p1     *m_prdCnv;
};

#endif 
