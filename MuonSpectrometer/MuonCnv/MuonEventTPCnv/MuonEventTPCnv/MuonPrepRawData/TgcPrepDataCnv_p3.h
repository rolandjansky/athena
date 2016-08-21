/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPREPDATACNV_P3_TRK_H
#define TgcPREPDATACNV_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p3.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/TgcPrepData.h"
#include "TgcPrepData_p3.h"

class MsgStream;

class TgcPrepDataCnv_p3
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::TgcPrepData, Muon::TgcPrepData_p3 >
{
    public:
  TgcPrepDataCnv_p3() {}

  Muon::TgcPrepData
  createTgcPrepData( const Muon::TgcPrepData_p3 *persObj,
                     const Identifier& id,
                     const MuonGM::TgcReadoutElement* detEl,
                     MsgStream & log );

  void persToTrans( const Muon::TgcPrepData_p3 *persObj,
                    Muon::TgcPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::TgcPrepData    *transObj,
                    Muon::TgcPrepData_p3 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
