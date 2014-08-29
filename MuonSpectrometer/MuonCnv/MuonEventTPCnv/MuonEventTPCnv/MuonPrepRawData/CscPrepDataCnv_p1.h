/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPREPDATACNV_P1_TRK_H
#define CSCPREPDATACNV_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/CscPrepData.h"
#include "CscPrepData_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class CscPrepDataCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::CscPrepData, Muon::CscPrepData_p1 >
{
    public:
  CscPrepDataCnv_p1() : m_prdCnv(0) {}

  void persToTrans( const Muon::CscPrepData_p1 *persObj,
                    Muon::CscPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::CscPrepData    *transObj,
                    Muon::CscPrepData_p1 *persObj,
                    MsgStream                &log );

protected:        
  PrepRawDataCnv_p1     *m_prdCnv;
};

#endif
