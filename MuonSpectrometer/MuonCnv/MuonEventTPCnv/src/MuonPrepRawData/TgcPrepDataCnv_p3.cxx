/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/TgcPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataCnv_p3.h"

void TgcPrepDataCnv_p3::
persToTrans( const Muon::TgcPrepData_p3 *persObj, Muon::TgcPrepData *transObj,MsgStream & /**log*/ ) 
{
    //log << MSG::DEBUG << "TgcPrepDataCnv_p3::persToTrans" << endreq;
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
    
    // bcBitMap, 4:Previous, 2:Current, 1:Next
    transObj->m_bcBitMap = persObj->m_bcBitMap;
}

void TgcPrepDataCnv_p3::
transToPers( const Muon::TgcPrepData *transObj, Muon::TgcPrepData_p3 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "TgcPrepDataCnv_p3::transToPers" << endreq;
    persObj->m_locX     = transObj->m_localPos.x();
    persObj->m_errorMat         = (*transObj->m_localCovariance)(0,0);
    persObj->m_bcBitMap = transObj->m_bcBitMap;
}


