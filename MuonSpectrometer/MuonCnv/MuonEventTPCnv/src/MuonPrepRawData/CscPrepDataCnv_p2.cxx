/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p2.h"
#include "CxxUtils/make_unique.h"

Muon::CscPrepData
CscPrepDataCnv_p2::createCscPrepData( const Muon::CscPrepData_p2 *persObj,
                                      const Identifier& clusId,
                                      const MuonGM::CscReadoutElement* detEl,
                                      MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p2::persToTrans" << endmsg;

    Amg::Vector2D localPos;
    localPos[Trk::locX] = persObj->m_locX; 
    localPos[Trk::locY] = 0.0;

    std::vector<Identifier> rdoList(1);
    rdoList[0]=clusId;

    auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
    (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

    Muon::CscPrepData data (clusId,
                            0, // idDE,
                            localPos,
                            std::move(rdoList),
                            cmat.release(),
                            detEl,
                            persObj->m_charge,
                            persObj->m_time,
                            static_cast<const Muon::CscClusterStatus>((persObj->m_status)&0xFF), // First 8 bits reserved for ClusterStatus.
                            static_cast<const Muon::CscTimeStatus>((persObj->m_status)>>8)
                            );
    return data;
}

void CscPrepDataCnv_p2::
persToTrans( const Muon::CscPrepData_p2 *persObj, Muon::CscPrepData *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p2::persToTrans" << endmsg;

    *transObj = createCscPrepData (persObj,
                                   transObj->identify(),
                                   transObj->detectorElement(),
                                   log);
}

void CscPrepDataCnv_p2::
transToPers( const Muon::CscPrepData *transObj, Muon::CscPrepData_p2 *persObj, MsgStream & log)
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p2::transToPers" << endmsg;
    persObj->m_locX           = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat       = transObj->localCovariance()(0,0);
    
    //CscPrepData - specific
    persObj->m_charge         = transObj->charge();
    persObj->m_time           = transObj->time();
    persObj->m_status         = (transObj->timeStatus()<<8); // First 8 bits reserved for ClusterStatus.
    persObj->m_status         += transObj->status();
}


