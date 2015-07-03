/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/CscPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p2.h"

void CscPrepDataCnv_p2::
persToTrans( const Muon::CscPrepData_p2 *persObj, Muon::CscPrepData *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p2::persToTrans" << endreq;
    // Fill localposition
    transObj->m_localPos[Trk::locX] = persObj->m_locX; 
    transObj->m_localPos[Trk::locY] = 0.0;
    // Update specifics
    transObj->m_rdoList.resize(1);
    transObj->m_rdoList[0]=transObj->m_clusId;
    
    // Error Matrix
    Amg::MatrixX* cmat = new  Amg::MatrixX(1,1);
    (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);
    transObj->m_localCovariance     = cmat;
    
    //CscPrepData - specific
    transObj->m_charge         = persObj->m_charge;
    transObj->m_time           = persObj->m_time;
    transObj->m_status         = static_cast<const Muon::CscClusterStatus>((persObj->m_status)&0xFF); // First 8 bits reserved for ClusterStatus.
    transObj->m_timeStatus     = static_cast<const Muon::CscTimeStatus>((persObj->m_status)>>8);
}

void CscPrepDataCnv_p2::
transToPers( const Muon::CscPrepData *transObj, Muon::CscPrepData_p2 *persObj, MsgStream & log)
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p2::transToPers" << endreq;
    persObj->m_locX           = transObj->m_localPos[Trk::locX];
    persObj->m_errorMat       = (*transObj->m_localCovariance)(0,0);
    
    //CscPrepData - specific
    persObj->m_charge         = transObj->m_charge;
    persObj->m_time           = transObj->m_time;
    persObj->m_status         = (transObj->m_timeStatus<<8); // First 8 bits reserved for ClusterStatus.
    persObj->m_status         += transObj->m_status;
}


