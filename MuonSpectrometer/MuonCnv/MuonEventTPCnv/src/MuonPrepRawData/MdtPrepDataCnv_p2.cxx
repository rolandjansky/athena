/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/MdtPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p2.h"

void MdtPrepDataCnv_p2::
persToTrans( const Muon::MdtPrepData_p2 *persObj, Muon::MdtPrepData *transObj,MsgStream & /**log*/ ) 
{
    //log << MSG::DEBUG << "MdtPrepDataCnv_p2::persToTrans" << endreq;

    // Fill localposition
    transObj->m_localPos[Trk::locX] = persObj->m_driftRadius; 
    transObj->m_localPos[Trk::locY] = 0.0; 
    
    // Update specifics
    transObj->m_rdoList.resize(1);
    transObj->m_rdoList[0]=transObj->m_clusId;
    
    // Error Matrix
    Amg::MatrixX* cmat = new  Amg::MatrixX(1,1);
    (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);
    transObj->m_localCovariance     = cmat;
    
    //MdtPrepData - specific
    transObj->m_tdc             = persObj->m_tdc;
    transObj->m_adc             = persObj->m_adc;
    transObj->m_status          = static_cast<const Muon::MdtDriftCircleStatus>(persObj->m_status);
}

void MdtPrepDataCnv_p2::
transToPers( const Muon::MdtPrepData *transObj, Muon::MdtPrepData_p2 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "MdtPrepDataCnv_p2::transToPers" << endreq;
    persObj->m_driftRadius      = transObj->m_localPos[Trk::locX];
    persObj->m_errorMat         = (*transObj->m_localCovariance)(0,0);
    persObj->m_tdc              = transObj->m_tdc;
    persObj->m_adc              = transObj->m_adc;
    persObj->m_status           = transObj->m_status;
}


