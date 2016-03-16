/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTrackCounts.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p2.h"

void TrigTrackCountsCnv_p2::persToTrans( const TrigTrackCounts_p2 *persObj, 
                                         TrigTrackCounts *transObj, 
					 MsgStream &log)
{
  log << MSG::DEBUG << "TrigTrackCountsCnv_p2::persToTrans called " << endreq;

  TrigHisto2D z0_pt;
  m_trigHistoCnv.persToTrans(&persObj->m_z0_pt, &z0_pt, log);
  TrigHisto2D eta_phi;
  m_trigHistoCnv.persToTrans(&persObj->m_eta_phi, &eta_phi, log);
  *transObj = TrigTrackCounts (std::move(z0_pt), std::move(eta_phi));
}


void TrigTrackCountsCnv_p2::transToPers(const TrigTrackCounts* transObj, 
	 			        TrigTrackCounts_p2* persObj, 
				        MsgStream &log)
{
  log << MSG::DEBUG << "TrigTrackCountsCnv_p2::transToPers called " << endreq;
 
  m_trigHistoCnv.transToPers(&transObj->z0_pt(), &persObj->m_z0_pt, log);
  m_trigHistoCnv.transToPers(&transObj->eta_phi(), &persObj->m_eta_phi, log); 
}
