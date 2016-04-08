/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p1::persToTrans( const TrigT2Jet_p1 *persObj,
					     TrigT2Jet    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p1::persToTrans called " << endreq;

  transObj->setE	 (persObj->m_e)		 ;
  transObj->setEhad0	 (persObj->m_ehad0)	 ;
  transObj->setEem0	 (persObj->m_eem0)	 ;
  transObj->setEta	 (persObj->m_eta)	 ;
  transObj->setPhi  	 (persObj->m_phi)  	 ;
  //transObj->m_coneRadius = persObj->m_coneRadius ;
  transObj->set_RoIword  (persObj->m_roiWord)    ;

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p1::transToPers( const TrigT2Jet    */*transObj*/,
                                         TrigT2Jet_p1 */*persObj*/,
					 MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p1::transToPers called " << endreq;
  log << MSG::WARNING << "TrigT2JetCnv_p1::transToPers : Trying to persist obsolete TrigT2JetCnv_p1! I won't write anything!" << endreq;
/*
  persObj->m_e		 = transObj->m_e	  ;
  persObj->m_ehad0	 = transObj->m_ehad0	  ;
  persObj->m_eem0	 = transObj->m_eem0	  ;
  persObj->m_eta	 = transObj->m_eta	  ;
  persObj->m_phi  	 = transObj->m_phi  	  ;
  //persObj->m_coneRadius  = transObj->m_coneRadius ;
  persObj->m_roiWord     = transObj->m_roiWord    ;
*/
}

