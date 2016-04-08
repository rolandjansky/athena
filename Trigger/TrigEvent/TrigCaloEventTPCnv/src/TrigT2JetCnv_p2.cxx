/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p2.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p2::persToTrans( const TrigT2Jet_p2 *persObj,
					     TrigT2Jet    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p2::persToTrans called " << endreq;

  transObj->setE	 (persObj->m_allTheFloats[0])	 ;
  transObj->setEhad0	 (persObj->m_allTheFloats[1])	 ;
  transObj->setEem0	 (persObj->m_allTheFloats[2])	 ;
  transObj->setEta	 (persObj->m_allTheFloats[3])	 ;
  transObj->setPhi  	 (persObj->m_allTheFloats[4])  	 ;
  //transObj->m_coneRadius = persObj->m_coneRadius ;
  transObj->set_RoIword  (persObj->m_roiWord)    ;
  // Cleaning:
  transObj->setNLeadingCells (persObj->m_nLeadingCells);
  transObj->setHecf          (persObj->m_allTheFloats[5]);
  transObj->setJetQuality    (persObj->m_allTheFloats[6]);
  transObj->setEmf           (persObj->m_allTheFloats[7]);
  transObj->setJetTimeCells  (persObj->m_allTheFloats[8]);

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p2::transToPers( const TrigT2Jet    *transObj,
					     TrigT2Jet_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p2::transToPers called " << endreq;

  persObj->m_allTheFloats[0]  = transObj->e()	  ;
  persObj->m_allTheFloats[1]  = transObj->ehad0()	  ;
  persObj->m_allTheFloats[2]  = transObj->eem0()	  ;
  persObj->m_allTheFloats[3]  = transObj->eta()	  ;
  persObj->m_allTheFloats[4]  = transObj->phi()  	  ;
  //persObj->m_coneRadius  = transObj->m_coneRadius ;
  persObj->m_roiWord          = transObj->RoIword()    ;

 // Cleaning:
  persObj->m_nLeadingCells   = transObj->nLeadingCells();
  persObj->m_allTheFloats[5] = transObj->hecf();
  persObj->m_allTheFloats[6] = transObj->jetQuality();
  persObj->m_allTheFloats[7] = transObj->emf();
  persObj->m_allTheFloats[8] = transObj->jetTimeCells();
}
