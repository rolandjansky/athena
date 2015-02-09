/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_CHAMBERT0SCNV_P1_H
#define MUONEVENTTPCNV_CHAMBERT0SCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   ChamberT0sCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonChamberT0s/ChamberT0s.h"
#include "ChamberT0s_p1.h"

class MsgStream;

class ChamberT0sCnv_p1
   : public T_AthenaPoolTPCnvBase< Muon::ChamberT0s, Muon::ChamberT0s_p1 >
{
    public:
  ChamberT0sCnv_p1()  {}

  void persToTrans( const Muon::ChamberT0s_p1 *persObj,
                    Muon::ChamberT0s    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::ChamberT0s    *transObj,
                    Muon::ChamberT0s_p1 *persObj,
                    MsgStream                &log );
};


#endif
