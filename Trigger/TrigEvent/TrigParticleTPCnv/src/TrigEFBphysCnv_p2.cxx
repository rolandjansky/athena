/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticleTPCnv/TrigEFBphys_p2.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigEFBphysCnv_p2.h"
 

static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p2::persToTrans( const TrigEFBphys_p2 *persObj,
					     TrigEFBphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p2::persToTrans called " << endreq;

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
  
  transObj->m_particleType    = static_cast<TrigEFBphys::pType>(persObj->m_particleType);  

  trackELVCnv.persToTrans(&persObj->m_trackVector, &transObj->m_trackVector, log);

  m_elementLinkEFBphysCnv.persToTrans(&persObj->m_secondaryDecay, &transObj->m_secondaryDecay, log);

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p2::transToPers( const TrigEFBphys    *transObj,
					     TrigEFBphys_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p2::transToPers called " << endreq;

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

  persObj->m_particleType   = static_cast<TrigEFBphys_p2::pType_p1>(transObj->m_particleType);

  trackELVCnv.transToPers(&transObj->m_trackVector, &persObj->m_trackVector, log);

  m_elementLinkEFBphysCnv.transToPers(&transObj->m_secondaryDecay, &persObj->m_secondaryDecay, log);

  
}
