/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcPREPDATACNV_P2_H
#define sTgcPREPDATACNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepDataCnv_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/sTgcPrepData.h"
#include "sTgcPrepData_p2.h"

class MsgStream;

class sTgcPrepDataCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::sTgcPrepData, Muon::sTgcPrepData_p2 >
{
public:
  sTgcPrepDataCnv_p2() {}
  
  Muon::sTgcPrepData
  createsTgcPrepData( const Muon::sTgcPrepData_p2 *persObj,
                      const Identifier clusId,
                      const MuonGM::sTgcReadoutElement* m_detEl,
                      MsgStream & log );
  
  void persToTrans( const Muon::sTgcPrepData_p2 *persObj,
                    Muon::sTgcPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::sTgcPrepData    *transObj,
                    Muon::sTgcPrepData_p2 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
