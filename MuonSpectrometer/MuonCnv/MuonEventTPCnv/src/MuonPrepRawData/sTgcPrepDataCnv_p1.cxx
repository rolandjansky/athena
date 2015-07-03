/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/TgcPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataCnv_p1.h"

void sTgcPrepDataCnv_p1::
persToTrans( const Muon::sTgcPrepData_p1 *persObj, Muon::sTgcPrepData *transObj,MsgStream & /**log*/ ) 
{
    //log << MSG::DEBUG << "sTgcPrepDataCnv_p1::persToTrans" << endreq;
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
    
    transObj->m_clusId          = persObj->m_id; //FIXME - remove!
    
}

void sTgcPrepDataCnv_p1::
transToPers( const Muon::sTgcPrepData *transObj, Muon::sTgcPrepData_p1 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "sTgcPrepDataCnv_p1::transToPers" << endreq;
    persObj->m_locX     = transObj->m_localPos[Trk::locX];
    persObj->m_errorMat = (*transObj->m_localCovariance)(0,0);
    persObj->m_id       = transObj->m_clusId.get_identifier32().get_compact(); // FIXME - remove when diff issue understood.
    
}


