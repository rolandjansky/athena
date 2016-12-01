/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MMPrepDataCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataCnv_p1.h"
#include "CxxUtils/make_unique.h"

Muon::MMPrepData
MMPrepDataCnv_p1::
createMMPrepData ( const Muon::MMPrepData_p1 *persObj,
                   const MuonGM::MMReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
  //log << MSG::DEBUG << "MMPrepDataCnv_p3::persToTrans" << endmsg;
  // Fill localposition
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locX; 
  localPos[Trk::locY] = 0.0; 
    
  std::vector<Identifier> rdoList(1);
  rdoList[0]=Identifier(persObj->m_id);
    
  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::MMPrepData data (Identifier (persObj->m_id), //FIXME - remove!
                         0, //collectionHash
                         localPos,
                         std::move(rdoList),
                         cmat.release(),
                         detEl);
  return data;
}

void MMPrepDataCnv_p1::
persToTrans( const Muon::MMPrepData_p1 *persObj, Muon::MMPrepData *transObj,MsgStream & log ) 
{
  *transObj = createMMPrepData (persObj,
                                transObj->detectorElement(),
                                log);
}

void MMPrepDataCnv_p1::
transToPers( const Muon::MMPrepData *transObj, Muon::MMPrepData_p1 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "MMPrepDataCnv_p3::transToPers" << endmsg;
    persObj->m_locX     = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat = (transObj->localCovariance())(0,0);
    persObj->m_id       = transObj->identify().get_identifier32().get_compact(); // FIXME - remove when diff issue understood.
}


