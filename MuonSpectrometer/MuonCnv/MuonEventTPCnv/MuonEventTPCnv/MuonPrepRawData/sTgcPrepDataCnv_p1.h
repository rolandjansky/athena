/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcPREPDATACNV_P1_H
#define sTgcPREPDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/sTgcPrepData.h"
#include "sTgcPrepData_p1.h"

class MsgStream;

class sTgcPrepDataCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::sTgcPrepData, Muon::sTgcPrepData_p1 >
{
    public:
  sTgcPrepDataCnv_p1() {}

  Muon::sTgcPrepData
  createsTgcPrepData( const Muon::sTgcPrepData_p1 *persObj,
                      const Identifier& /*id*/,
                      const MuonGM::sTgcReadoutElement* m_detEl,
                      MsgStream & log );

  void persToTrans( const Muon::sTgcPrepData_p1 *persObj,
                    Muon::sTgcPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::sTgcPrepData    *transObj,
                    Muon::sTgcPrepData_p1 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
