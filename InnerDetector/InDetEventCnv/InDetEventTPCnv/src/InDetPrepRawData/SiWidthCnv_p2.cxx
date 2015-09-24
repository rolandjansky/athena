/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/SiWidth.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p2.h"

void InDet::SiWidthCnv_p2::persToTrans( const InDet::SiWidth_p2 *persObj,
				 InDet::SiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "in SiWidthCnv_p2::persToTrans" << endreq;
  transObj->m_colrow[Trk::locX] = (persObj->m_size >> 8);
  transObj->m_colrow[Trk::locY] = (persObj->m_size & 0xFF );
  m_localPosCnv.persToTrans(&persObj->m_phirzWidth, &transObj->m_phirzWidth, log);
}


void InDet::SiWidthCnv_p2::transToPers( const InDet::SiWidth *transObj,
				 InDet::SiWidth_p2 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE)  log<< MSG::VERBOSE << "in SiWidthCnv_p2::transToPERS" << endreq;
    persObj->m_size = ( (unsigned int) transObj->m_colrow[Trk::locX] << 8) | ( (unsigned int) transObj->m_colrow[Trk::locY] );
    m_localPosCnv.transToPers(&transObj->m_phirzWidth, &persObj->m_phirzWidth, log);
}
