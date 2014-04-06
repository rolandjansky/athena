/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticleTPCnv/TrigL2Bphys_p2.h"
#include "TrigParticle/TrigL2Bphys.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigL2BphysCnv_p2.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<TrigInDetTrackCollection> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p2::persToTrans( const TrigL2Bphys_p2 *persObj,
					     TrigL2Bphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p2::persToTrans called " << endreq;

  transObj->m_roiID    = persObj->m_roiID    ;
  transObj->m_eta      = persObj->m_eta      ;
  transObj->m_phi      = persObj->m_phi      ;
  transObj->m_mass     = persObj->m_mass     ;
  transObj->m_fitmass  = persObj->m_fitmass     ;
  transObj->m_fitchi2  = persObj->m_fitchi2     ;
  transObj->m_fitndof  = persObj->m_fitndof     ;
  transObj->m_fitx  = persObj->m_fitx     ;
  transObj->m_fity  = persObj->m_fity     ;
  transObj->m_fitz  = persObj->m_fitz     ;
 
  transObj->m_particleType    = static_cast<TrigL2Bphys::pType>(persObj->m_particleType);

  trackELVCnv.persToTrans(&persObj->m_trackVector, &transObj->m_trackVector, log);


  m_elementLinkL2BphysCnv.persToTrans(&persObj->m_secondaryDecay, &transObj->m_secondaryDecay, log);

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p2::transToPers( const TrigL2Bphys    *transObj,
					     TrigL2Bphys_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p2::transToPers called " << endreq;

  persObj->m_roiID    = transObj->m_roiID    ;
  persObj->m_eta      = transObj->m_eta      ;
  persObj->m_phi      = transObj->m_phi      ;
  persObj->m_mass     = transObj->m_mass     ;
  persObj->m_fitmass  = transObj->m_fitmass  ;
  persObj->m_fitchi2  = transObj->m_fitchi2  ;
  persObj->m_fitndof  = transObj->m_fitndof  ;
  persObj->m_fitx  = transObj->m_fitx  ;
  persObj->m_fity  = transObj->m_fity  ;
  persObj->m_fitz  = transObj->m_fitz  ;
 
  persObj->m_particleType   = static_cast<TrigL2Bphys_p2::pType_p1>(transObj->m_particleType);
  
  

  trackELVCnv.transToPers(&transObj->m_trackVector, &persObj->m_trackVector, log);

  m_elementLinkL2BphysCnv.transToPers(&transObj->m_secondaryDecay, &persObj->m_secondaryDecay, log);

}
