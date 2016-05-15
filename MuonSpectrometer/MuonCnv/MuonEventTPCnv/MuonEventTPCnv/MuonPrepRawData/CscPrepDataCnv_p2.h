/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPREPDATACNV_P2_TRK_H
#define CSCPREPDATACNV_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/CscPrepData.h"
#include "CscPrepData_p2.h"

class MsgStream;

class CscPrepDataCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::CscPrepData, Muon::CscPrepData_p2 >
{
    public:
  CscPrepDataCnv_p2() {}

  Muon::CscPrepData
  createCscPrepData( const Muon::CscPrepData_p2 *persObj,
                     const Identifier& clusId,
                     const MuonGM::CscReadoutElement* detEl,
                     MsgStream & log );

  void persToTrans( const Muon::CscPrepData_p2 *persObj,
                    Muon::CscPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::CscPrepData    *transObj,
                    Muon::CscPrepData_p2 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
