/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigL2Bphys_p2.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticleTPCnv/TrigL2BphysCnv_p2.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<TrigInDetTrackCollection> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p2::persToTrans( const TrigL2Bphys_p2 *persObj,
					     TrigL2Bphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p2::persToTrans called " << endmsg;

  
  ElementLink<TrigL2BphysContainer> secondaryDecay;
  m_elementLinkL2BphysCnv.persToTrans(&persObj->m_secondaryDecay, &secondaryDecay, log);

  *transObj = TrigL2Bphys (persObj->m_roiID,
                           persObj->m_eta,
                           persObj->m_phi,
                           static_cast<TrigL2Bphys::pType>(persObj->m_particleType),
                           persObj->m_mass,
                           secondaryDecay); 

  transObj->fitmass (persObj->m_fitmass)     ;
  transObj->fitchi2 (persObj->m_fitchi2)     ;
  transObj->fitndof (persObj->m_fitndof)     ;
  transObj->fitx    (persObj->m_fitx)     ;
  transObj->fity    (persObj->m_fity)     ;
  transObj->fitz    (persObj->m_fitz)     ;

  ElementLinkVector<TrigInDetTrackCollection> trackVector;
  trackELVCnv.persToTrans(&persObj->m_trackVector, &trackVector, log);
  for (const ElementLink<TrigInDetTrackCollection> el : trackVector)
    transObj->addTrack (el);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p2::transToPers( const TrigL2Bphys    *transObj,
					     TrigL2Bphys_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p2::transToPers called " << endmsg;

  persObj->m_roiID    = transObj->roiId()    ;
  persObj->m_eta      = transObj->eta()      ;
  persObj->m_phi      = transObj->phi()      ;
  persObj->m_mass     = transObj->mass()     ;
  persObj->m_fitmass  = transObj->fitmass()  ;
  persObj->m_fitchi2  = transObj->fitchi2()  ;
  persObj->m_fitndof  = transObj->fitndof()  ;
  persObj->m_fitx  = transObj->fitx()  ;
  persObj->m_fity  = transObj->fity()  ;
  persObj->m_fitz  = transObj->fitz()  ;
 
  persObj->m_particleType   = static_cast<TrigL2Bphys_p2::pType_p1>(transObj->particleType());
  
  

  trackELVCnv.transToPers(&transObj->trackVector(), &persObj->m_trackVector, log);

  m_elementLinkL2BphysCnv.transToPers(&transObj->secondaryDecayLink(), &persObj->m_secondaryDecay, log);
}
