/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/SiWidth.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p1.h"

void SiWidthCnv_p1::persToTrans( const InDet::SiWidth_p1 *persObj,
				 InDet::SiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "in SiWidthCnv_p1::persToTrans" << endreq;
   transObj->m_colrow[Trk::locX] = persObj->x; 
   transObj->m_colrow[Trk::locY] = persObj->y;
   fillTransFromPStore( &m_localPosCnv, persObj->m_phirzWidth, &transObj->m_phirzWidth, log );
}


void SiWidthCnv_p1::transToPers( const InDet::SiWidth *transObj,
				 InDet::SiWidth_p1 *persObj, MsgStream &log )
{
//     if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "in SiWidthCnv_p1::transToPERS" << endreq;
    persObj->x = (float) transObj->m_colrow[Trk::locX];
    persObj->y = (float) transObj->m_colrow[Trk::locY];
    persObj->m_phirzWidth = toPersistent( &m_localPosCnv, &transObj->m_phirzWidth, log );
}
