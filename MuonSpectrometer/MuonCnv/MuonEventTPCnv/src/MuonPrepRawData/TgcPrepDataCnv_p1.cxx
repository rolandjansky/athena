/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataCnv_p1.h"

void TgcPrepDataCnv_p1::
persToTrans( const Muon::TgcPrepData_p1 *persObj, Muon::TgcPrepData *transObj,MsgStream &log ) 
{
   log << MSG::DEBUG << "TgcPrepDataCnv_p1::persToTrans" << endmsg;
   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
}

void TgcPrepDataCnv_p1::
transToPers( const Muon::TgcPrepData *transObj, Muon::TgcPrepData_p1 *persObj, MsgStream &log )
{
   log << MSG::DEBUG << "TgcPrepDataCnv_p1::transToPers" << endmsg;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
}


