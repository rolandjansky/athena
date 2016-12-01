/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p2.h"
#include "CxxUtils/make_unique.h"

Muon::MdtPrepData
MdtPrepDataCnv_p2::
createMdtPrepData( const Muon::MdtPrepData_p2 *persObj,
                   const Identifier& id,
                   const MuonGM::MdtReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_driftRadius; 
  localPos[Trk::locY] = 0.0; 
    
  std::vector<Identifier> rdoList(1);
  rdoList[0] = id;
    
  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);
    
  Muon::MdtPrepData data(id,
                         0, // collectionHash
                         localPos,
                         std::move(cmat),
                         std::move(rdoList),
                         detEl,
                         persObj->m_tdc,
                         persObj->m_adc,
                         static_cast<const Muon::MdtDriftCircleStatus>(persObj->m_status));

  return data;
}

void MdtPrepDataCnv_p2::
persToTrans( const Muon::MdtPrepData_p2 *persObj, Muon::MdtPrepData *transObj,MsgStream & log ) 
{
  *transObj = createMdtPrepData (persObj,
                                 transObj->identify(),
                                 transObj->detectorElement(),
                                 log);
}

void MdtPrepDataCnv_p2::
transToPers( const Muon::MdtPrepData *transObj, Muon::MdtPrepData_p2 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "MdtPrepDataCnv_p2::transToPers" << endmsg;
    persObj->m_driftRadius      = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat         = transObj->localCovariance()(0,0);
    persObj->m_tdc              = transObj->tdc();
    persObj->m_adc              = transObj->adc();
    persObj->m_status           = transObj->status();
}


