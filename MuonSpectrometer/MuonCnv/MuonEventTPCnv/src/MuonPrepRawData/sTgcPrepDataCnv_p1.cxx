/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataCnv_p1.h"
#include "CxxUtils/make_unique.h"

Muon::sTgcPrepData
sTgcPrepDataCnv_p1::
createsTgcPrepData( const Muon::sTgcPrepData_p1 *persObj,
                    const Identifier& /*id*/,
                    const MuonGM::sTgcReadoutElement* detEl,
                    MsgStream & /**log*/ ) 
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locX; 
  localPos[Trk::locY] = 0.0;

  std::vector<Identifier> rdoList(1);
  rdoList[0] = Identifier (persObj->m_id);
    
  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);
    
  Muon::sTgcPrepData data (Identifier (persObj->m_id), //FIXME - remove!
                           0, // collectionHash
                           localPos,
                           std::move(rdoList),
                           cmat.release(),
                           detEl);
  return data;
}

void sTgcPrepDataCnv_p1::
persToTrans( const Muon::sTgcPrepData_p1 *persObj, Muon::sTgcPrepData *transObj,MsgStream & log ) 
{
  *transObj = createsTgcPrepData (persObj,
                                  transObj->identify(),
                                  transObj->detectorElement(),
                                  log);
}

void sTgcPrepDataCnv_p1::
transToPers( const Muon::sTgcPrepData *transObj, Muon::sTgcPrepData_p1 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "sTgcPrepDataCnv_p1::transToPers" << endmsg;
    persObj->m_locX     = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat = transObj->localCovariance()(0,0);
    persObj->m_id       = transObj->identify().get_identifier32().get_compact(); // FIXME - remove when diff issue understood.
    
}


