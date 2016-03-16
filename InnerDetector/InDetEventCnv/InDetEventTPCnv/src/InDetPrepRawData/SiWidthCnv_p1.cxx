/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiWidth.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p1.h"

void SiWidthCnv_p1::persToTrans( const InDet::SiWidth_p1 *persObj,
				 InDet::SiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "in SiWidthCnv_p1::persToTrans" << endreq;
  transObj->setColumn (persObj->x);
  transObj->setRow    (persObj->y);
  Amg::Vector2D phirzwidth;
  fillTransFromPStore( &m_localPosCnv, persObj->m_phirzWidth, &phirzwidth, log );
  transObj->setPhirzWidth (phirzwidth);
}


void SiWidthCnv_p1::transToPers( const InDet::SiWidth *transObj,
				 InDet::SiWidth_p1 *persObj, MsgStream &log )
{
//     if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "in SiWidthCnv_p1::transToPERS" << endreq;
  persObj->x = (float) transObj->colRow()[Trk::locX];
  persObj->y = (float) transObj->colRow()[Trk::locY];
  persObj->m_phirzWidth = toPersistent( &m_localPosCnv, &transObj->widthPhiRZ(), log );
}
