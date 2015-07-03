/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/MdtPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p1.h"

void MdtPrepDataCnv_p1::
persToTrans( const Muon::MdtPrepData_p1 *persObj, Muon::MdtPrepData *transObj,MsgStream &log ) 
{
   //log << MSG::DEBUG << "MdtPrepDataCnv_p1::persToTrans" << endreq;
   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
   transObj->m_tdc              = persObj->m_tdc;
   transObj->m_adc              = persObj->m_adc;
   transObj->m_status           = static_cast<const Muon::MdtDriftCircleStatus>(persObj->m_status);
}

void MdtPrepDataCnv_p1::
transToPers( const Muon::MdtPrepData *transObj, Muon::MdtPrepData_p1 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "MdtPrepDataCnv_p1::transToPers" << endreq;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_tdc              = transObj->m_tdc;
   persObj->m_adc              = transObj->m_adc;
   persObj->m_status           = transObj->m_status;
}


