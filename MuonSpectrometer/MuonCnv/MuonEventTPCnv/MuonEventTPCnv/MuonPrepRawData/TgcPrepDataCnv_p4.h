/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPREPDATACNV_P4_TRK_H
#define TgcPREPDATACNV_P4_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p4.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/TgcPrepData.h"
#include "TgcPrepData_p4.h"

class MsgStream;

class TgcPrepDataCnv_p4
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::TgcPrepData, Muon::TgcPrepData_p4 >
{
    public:
  TgcPrepDataCnv_p4() {}

  Muon::TgcPrepData
  createTgcPrepData( const Muon::TgcPrepData_p4 *persObj,
                     const Identifier& id,
                     const MuonGM::TgcReadoutElement* detEl,
                     MsgStream & log );

  void persToTrans( const Muon::TgcPrepData_p4 *persObj,
                    Muon::TgcPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::TgcPrepData    *transObj,
                    Muon::TgcPrepData_p4 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
