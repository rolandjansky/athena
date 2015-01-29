/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigTrackCounts.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p2.h"

void TrigTrackCountsCnv_p2::persToTrans( const TrigTrackCounts_p2 *persObj, 
                                         TrigTrackCounts *transObj, 
					 MsgStream &log)
{
  log << MSG::DEBUG << "TrigTrackCountsCnv_p2::persToTrans called " << endreq;
  
  m_trigHistoCnv.persToTrans(&persObj->m_z0_pt, &transObj->m_z0_pt, log);
  m_trigHistoCnv.persToTrans(&persObj->m_eta_phi, &transObj->m_eta_phi, log);
}


void TrigTrackCountsCnv_p2::transToPers(const TrigTrackCounts* transObj, 
	 			        TrigTrackCounts_p2* persObj, 
				        MsgStream &log)
{
  log << MSG::DEBUG << "TrigTrackCountsCnv_p2::transToPers called " << endreq;
 
  m_trigHistoCnv.transToPers(&transObj->m_z0_pt, &persObj->m_z0_pt, log);
  m_trigHistoCnv.transToPers(&transObj->m_eta_phi, &persObj->m_eta_phi, log); 
}
