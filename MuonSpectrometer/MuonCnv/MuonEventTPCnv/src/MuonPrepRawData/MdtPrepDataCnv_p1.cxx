/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p1.h"

void MdtPrepDataCnv_p1::
persToTrans( const Muon::MdtPrepData_p1 *persObj, Muon::MdtPrepData *transObj,MsgStream &log ) 
{
   Amg::Vector2D driftRadius;
   driftRadius.setZero();

   *transObj = Muon::MdtPrepData (transObj->identify(),
                                  transObj->collectionHash(),
                                  driftRadius,
                                  nullptr, // errDriftRadius
                                  transObj->detectorElement(),
                                  persObj->m_tdc,
                                  persObj->m_adc,
                                  static_cast<const Muon::MdtDriftCircleStatus>(persObj->m_status));

   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
}

void MdtPrepDataCnv_p1::
transToPers( const Muon::MdtPrepData *transObj, Muon::MdtPrepData_p1 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "MdtPrepDataCnv_p1::transToPers" << endmsg;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_tdc              = transObj->tdc();
   persObj->m_adc              = transObj->adc();
   persObj->m_status           = transObj->status();
}


