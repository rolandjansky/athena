/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p3.h"
#undef private
#undef protected
 
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p3.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p3::persToTrans( const TrigT2Jet_p3 *persObj,
					     TrigT2Jet    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p3::persToTrans called " << endreq;

  transObj->m_e		 = persObj->m_allTheFloats[0]	 ;
  transObj->m_ehad0	 = persObj->m_allTheFloats[1]	 ;
  transObj->m_eem0	 = persObj->m_allTheFloats[2]	 ;
  transObj->m_eta	 = persObj->m_allTheFloats[3]	 ;
  transObj->m_phi  	 = persObj->m_allTheFloats[4]  	 ;
  //transObj->m_coneRadius = persObj->m_coneRadius ;
  transObj->m_roiWord    = persObj->m_roiWord    ;
  // Cleaning:
  transObj->m_nLeadingCells = persObj->m_nLeadingCells;
  transObj->m_hecf          = persObj->m_allTheFloats[5];
  transObj->m_jetQuality    = persObj->m_allTheFloats[6];
  transObj->m_emf           = persObj->m_allTheFloats[7];
  transObj->m_jetTimeCells  = persObj->m_allTheFloats[8];

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigT2JetCnv_p3::transToPers( const TrigT2Jet    *transObj,
					     TrigT2Jet_p3 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigT2JetCnv_p3::transToPers called " << endreq;

  persObj->m_allTheFloats[0]  = transObj->m_e	  ;
  persObj->m_allTheFloats[1]  = transObj->m_ehad0	  ;
  persObj->m_allTheFloats[2]  = transObj->m_eem0	  ;
  persObj->m_allTheFloats[3]  = transObj->m_eta	  ;
  persObj->m_allTheFloats[4]  = transObj->m_phi  	  ;
  //persObj->m_coneRadius  = transObj->m_coneRadius ;
  persObj->m_roiWord          = transObj->m_roiWord    ;

 // Cleaning:
  persObj->m_nLeadingCells   = transObj->m_nLeadingCells;
  persObj->m_allTheFloats[5] = transObj->m_hecf;
  persObj->m_allTheFloats[6] = transObj->m_jetQuality;
  persObj->m_allTheFloats[7] = transObj->m_emf;
  persObj->m_allTheFloats[8] = transObj->m_jetTimeCells;

}
