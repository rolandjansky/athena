/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticleTPCnv/TrigEFBphys_p2.h"
#include "TrigParticleTPCnv/TrigEFBphysCnv_p2.h"
 

static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p2::persToTrans( const TrigEFBphys_p2 *persObj,
					     TrigEFBphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p2::persToTrans called " << endmsg;

  ElementLink<TrigEFBphysContainer> secondaryDecay;
  m_elementLinkEFBphysCnv.persToTrans(&persObj->m_secondaryDecay, &secondaryDecay, log);
  
  *transObj = TrigEFBphys (persObj->m_roiID,
                           persObj->m_eta,
                           persObj->m_phi,
                           static_cast<TrigEFBphys::pType>(persObj->m_particleType),
                           persObj->m_mass,
                           secondaryDecay);
  transObj->fitmass (persObj->m_fitmass)     ;
  transObj->fitchi2 (persObj->m_fitchi2)     ;
  transObj->fitndof (persObj->m_fitndof)     ;
  transObj->fitx    (persObj->m_fitx)     ;
  transObj->fity    (persObj->m_fity)     ;
  transObj->fitz    (persObj->m_fitz)     ;

  ElementLinkVector<Rec::TrackParticleContainer> trackVector;
  trackELVCnv.persToTrans(&persObj->m_trackVector, &trackVector, log);
  for (const ElementLink<Rec::TrackParticleContainer> el : trackVector)
    transObj->addTrack (el);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p2::transToPers( const TrigEFBphys    *transObj,
					     TrigEFBphys_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p2::transToPers called " << endmsg;

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

  persObj->m_particleType   = static_cast<TrigEFBphys_p2::pType_p1>(transObj->particleType());

  trackELVCnv.transToPers(&transObj->trackVector(), &persObj->m_trackVector, log);

  m_elementLinkEFBphysCnv.transToPers(&transObj->secondaryDecayLink(), &persObj->m_secondaryDecay, log);
}
