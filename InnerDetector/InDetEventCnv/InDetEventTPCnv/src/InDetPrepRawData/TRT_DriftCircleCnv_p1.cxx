/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/TRT_DriftCircle.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"


void TRT_DriftCircleCnv_p1::persToTrans( const InDet::TRT_DriftCircle_p1 *persObj,
					 InDet::TRT_DriftCircle    *transObj,
					 MsgStream                   &log )
{
   fillTransFromPStore( &m_rawDataCnv, persObj->m_rawData, transObj, log );
   
   transObj->m_word  = 0;
   transObj->m_word  = persObj->m_driftTimeValid ?  0x08000000 : 0x0;

   if(persObj->m_highLevel){
     transObj->m_word |=0x04020100;
   }
}

void TRT_DriftCircleCnv_p1::transToPers( const InDet::TRT_DriftCircle    *transObj,
					 InDet::TRT_DriftCircle_p1 *persObj,
					 MsgStream                   &log)
{
   persObj->m_rawData = baseToPersistent( &m_rawDataCnv, transObj, log );

   persObj->m_driftTimeValid    = transObj->m_word & 0x08000000;
   persObj->m_highLevel         = transObj->m_word & 0x04020100; 
}

