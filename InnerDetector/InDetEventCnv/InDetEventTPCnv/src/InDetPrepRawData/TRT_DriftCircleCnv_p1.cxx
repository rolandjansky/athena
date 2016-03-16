/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"


void TRT_DriftCircleCnv_p1::persToTrans( const InDet::TRT_DriftCircle_p1 *persObj,
					 InDet::TRT_DriftCircle    *transObj,
					 MsgStream                   &log )
{
   Amg::Vector2D localPos;
   localPos.setZero();

   unsigned int trtdata  = persObj->m_driftTimeValid ?  0x08000000 : 0x0;
   if(persObj->m_highLevel)
     trtdata |=0x04020100;

   *transObj = InDet::TRT_DriftCircle (Identifier(),
                                         localPos,
                                         std::vector<Identifier>(), // rdoList
                                         std::unique_ptr<const Amg::MatrixX>(), // cmat
                                         nullptr, // detEl
                                         trtdata
                                         );

   fillTransFromPStore( &m_rawDataCnv, persObj->m_rawData, transObj, log );
}

void TRT_DriftCircleCnv_p1::transToPers( const InDet::TRT_DriftCircle    *transObj,
					 InDet::TRT_DriftCircle_p1 *persObj,
					 MsgStream                   &log)
{
   persObj->m_rawData = baseToPersistent( &m_rawDataCnv, transObj, log );

   persObj->m_driftTimeValid    = transObj->getWord() & 0x08000000;
   persObj->m_highLevel         = transObj->getWord() & 0x04020100; 
}

