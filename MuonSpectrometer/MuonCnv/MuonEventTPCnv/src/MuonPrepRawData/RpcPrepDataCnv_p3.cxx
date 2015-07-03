/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/RpcPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p3.h"

void RpcPrepDataCnv_p3::
persToTrans( const Muon::RpcPrepData_p3 *persObj, Muon::RpcPrepData *transObj,MsgStream & /**log*/ ) 
{
    //log << MSG::DEBUG << "RpcPrepDataCnv_p3::persToTrans" << endreq;
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
    
    //RpcPrepData - specific
    transObj->m_time           = persObj->m_time;
    transObj->m_triggerInfo    = persObj->m_triggerInfo;
    transObj->m_ambiguityFlag  = persObj->m_ambiguityFlag;
}

void RpcPrepDataCnv_p3::
transToPers( const Muon::RpcPrepData *transObj, Muon::RpcPrepData_p3 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "RpcPrepDataCnv_p3::transToPers" << endreq;
    persObj->m_locX           = transObj->m_localPos[Trk::locX];
    persObj->m_errorMat       = (*transObj->m_localCovariance)(0,0);
    
    //RpcPrepData - specific
    persObj->m_time           = transObj->m_time;
    persObj->m_triggerInfo    = transObj->m_triggerInfo;
    persObj->m_ambiguityFlag  = transObj->m_ambiguityFlag;
}


