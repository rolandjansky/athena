/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainerCnv_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_p1.h"

void FTK_RawTrackContainerCnv_p1::persToTrans(const FTK_RawTrackContainer_p1* persObj, 
						 FTK_RawTrackContainer* transObj, 
						 MsgStream &log)
{
  log << MSG::DEBUG << "FTK_RawTrackContainerCnv_p1::persToTrans called " << endreq;
  
  m_FTK_RawTrackVectorCnv.persToTrans( &(persObj->m_FTK_RawTrackVector), transObj, log );
}


void FTK_RawTrackContainerCnv_p1::transToPers(const FTK_RawTrackContainer* transObj, 
						 FTK_RawTrackContainer_p1* persObj, 
						 MsgStream &log)
{
  log << MSG::DEBUG << "FTK_RawTrackContainerCnv_p1::transToPers called " << endreq;
  
  m_FTK_RawTrackVectorCnv.transToPers( transObj, &(persObj->m_FTK_RawTrackVector), log );
  
}
