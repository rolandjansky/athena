/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p3.h"
#include "CxxUtils/make_unique.h"

Muon::RpcPrepData
RpcPrepDataCnv_p3::
createRpcPrepData( const Muon::RpcPrepData_p3 *persObj,
                   const Identifier& id,
                   const MuonGM::RpcReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locX; 
  localPos[Trk::locY] = 0.0;

  std::vector<Identifier> rdoList(1);
  rdoList[0]=id;
    
  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::RpcPrepData data (id,
                          0, // collectionHash
                          localPos,
                          rdoList,
                          cmat.release(),
                          detEl,
                          persObj->m_time,
                          persObj->m_triggerInfo,
                          persObj->m_ambiguityFlag);
  return data;
}

void RpcPrepDataCnv_p3::
persToTrans( const Muon::RpcPrepData_p3 *persObj, Muon::RpcPrepData *transObj,MsgStream & log ) 
{
  *transObj = createRpcPrepData (persObj,
                                 transObj->identify(),
                                 transObj->detectorElement(),
                                 log);
}

void RpcPrepDataCnv_p3::
transToPers( const Muon::RpcPrepData *transObj, Muon::RpcPrepData_p3 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "RpcPrepDataCnv_p3::transToPers" << endmsg;
    persObj->m_locX           = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat       = transObj->localCovariance()(0,0);
    
    //RpcPrepData - specific
    persObj->m_time           = transObj->time();
    persObj->m_triggerInfo    = transObj->triggerInfo();
    persObj->m_ambiguityFlag  = transObj->ambiguityFlag();
}


