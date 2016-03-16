/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiWidth.h"

#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p2.h"

void InDet::SiWidthCnv_p2::persToTrans( const InDet::SiWidth_p2 *persObj,
				 InDet::SiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "in SiWidthCnv_p2::persToTrans" << endreq;
  transObj->setColumn (persObj->m_size >> 8);
  transObj->setRow    (persObj->m_size & 0xFF );
  Amg::Vector2D phirzwidth;
  m_localPosCnv.persToTrans(&persObj->m_phirzWidth, &phirzwidth, log);
  transObj->setPhirzWidth (phirzwidth);
}


void InDet::SiWidthCnv_p2::transToPers( const InDet::SiWidth *transObj,
				 InDet::SiWidth_p2 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE)  log<< MSG::VERBOSE << "in SiWidthCnv_p2::transToPERS" << endreq;
  persObj->m_size = ( (unsigned int) transObj->colRow()[Trk::locX] << 8) | ( (unsigned int) transObj->colRow()[Trk::locY] );
  m_localPosCnv.transToPers(&transObj->widthPhiRZ(), &persObj->m_phirzWidth, log);
}
