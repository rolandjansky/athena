/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TgcPrepDataCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepDataCnv_p3.h"
#include "CxxUtils/make_unique.h"

Muon::TgcPrepData
TgcPrepDataCnv_p3::
createTgcPrepData( const Muon::TgcPrepData_p3 *persObj,
                   const Identifier& id,
                   const MuonGM::TgcReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locX; 
  localPos[Trk::locY] = 0.0; 
        
  std::vector<Identifier> rdoList(1);
  rdoList[0]=id;
    
  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::TgcPrepData data (id,
                          0, // collectionHash
                          localPos,
                          std::move(rdoList),
                          cmat.release(),
                          detEl,
                          persObj->m_bcBitMap);
  return data;
}

void TgcPrepDataCnv_p3::
persToTrans( const Muon::TgcPrepData_p3 *persObj, Muon::TgcPrepData *transObj,MsgStream & log ) 
{
  *transObj = createTgcPrepData (persObj,
                                 transObj->identify(),
                                 transObj->detectorElement(),
                                 log);
}

void TgcPrepDataCnv_p3::
transToPers( const Muon::TgcPrepData *transObj, Muon::TgcPrepData_p3 *persObj, MsgStream & /**log*/ )
{
  persObj->m_locX     = transObj->localPosition().x();
  persObj->m_errorMat = transObj->localCovariance()(0,0);
  persObj->m_bcBitMap = transObj->getBcBitMap();
}
