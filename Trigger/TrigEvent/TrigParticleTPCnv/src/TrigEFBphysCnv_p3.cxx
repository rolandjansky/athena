/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticleTPCnv/TrigEFBphys_p3.h"
#include "TrigParticleTPCnv/TrigEFBphysCnv_p3.h"
 

static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p3::persToTrans( const TrigEFBphys_p3 *persObj,
				     TrigEFBphys    *transObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p3::persToTrans called " << endmsg;

  ElementLink<TrigEFBphysContainer> secondaryDecay;
  m_elementLinkEFBphysCnv.persToTrans(&persObj->m_secondaryDecay, &secondaryDecay, log);
  
  *transObj = TrigEFBphys (persObj->m_allInts[0], // roiID
                           persObj->m_allFloats[0], // eta
                           persObj->m_allFloats[1], // phi
                           static_cast<TrigEFBphys::pType>(persObj->m_allInts[1]), // particleType
                           persObj->m_allFloats[2], // mass
                           secondaryDecay);
  transObj->fitmass (persObj->m_allFloats[3]);
  transObj->fitchi2 (persObj->m_allFloats[4]);
  transObj->fitndof (persObj->m_allInts[2]);
  transObj->fitx    (persObj->m_allFloats[5]);
  transObj->fity    (persObj->m_allFloats[6]);
  transObj->fitz    (persObj->m_allFloats[7]);

  ElementLinkVector<Rec::TrackParticleContainer> trackVector;
  trackELVCnv.persToTrans(&persObj->m_trackVector, &trackVector, log);
  for (const ElementLink<Rec::TrackParticleContainer> el : trackVector)
    transObj->addTrack (el);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p3::transToPers( const TrigEFBphys    *transObj,
				     TrigEFBphys_p3 *persObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p3::transToPers called " << endmsg;

  persObj->m_allInts[0]    = transObj->roiId()    ;
  persObj->m_allInts[1]    = transObj->particleType();
  persObj->m_allInts[2]    = transObj->fitndof()  ;

  persObj->m_allFloats[0]  = transObj->eta()      ;
  persObj->m_allFloats[1]  = transObj->phi()      ;
  persObj->m_allFloats[2]  = transObj->mass()     ;
  persObj->m_allFloats[3]  = transObj->fitmass()  ;
  persObj->m_allFloats[4]  = transObj->fitchi2()  ;
  persObj->m_allFloats[5]  = transObj->fitx()  ;
  persObj->m_allFloats[6]  = transObj->fity()  ;
  persObj->m_allFloats[7]  = transObj->fitz()  ;

  trackELVCnv.transToPers(&transObj->trackVector(), &persObj->m_trackVector, log);

  m_elementLinkEFBphysCnv.transToPers(&transObj->secondaryDecayLink(), &persObj->m_secondaryDecay, log);
}
