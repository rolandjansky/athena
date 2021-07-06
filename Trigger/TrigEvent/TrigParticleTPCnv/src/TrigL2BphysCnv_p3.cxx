/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticleTPCnv/TrigL2BphysCnv_p3.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<TrigInDetTrackCollection> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p3::persToTrans( const TrigL2Bphys_p3 *persObj,
				     TrigL2Bphys    *transObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p3::persToTrans called " << endmsg;


  ElementLink<TrigL2BphysContainer> secondaryDecay;
  m_elementLinkL2BphysCnv.persToTrans(&persObj->m_secondaryDecay, &secondaryDecay, log);

  *transObj = TrigL2Bphys (persObj->m_allInts[0],    // roiID
                           persObj->m_allFloats[0],  // eta
                           persObj->m_allFloats[1],  // phi
                           static_cast<TrigL2Bphys::pType>(persObj->m_allInts[1]), // particleType
                           persObj->m_allFloats[2],  // mass
                           secondaryDecay); 

  transObj->fitndof  (persObj->m_allInts[2]);
  transObj->fitmass  (persObj->m_allFloats[3]);
  transObj->fitchi2  (persObj->m_allFloats[4]);
  transObj->fitx     (persObj->m_allFloats[5]);
  transObj->fity     (persObj->m_allFloats[6]);
  transObj->fitz     (persObj->m_allFloats[7]);

  ElementLinkVector<TrigInDetTrackCollection> trackVector;
  trackELVCnv.persToTrans(&persObj->m_trackVector, &trackVector, log);
  for (const ElementLink<TrigInDetTrackCollection> el : trackVector)
    transObj->addTrack (el);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p3::transToPers( const TrigL2Bphys    *transObj,
				     TrigL2Bphys_p3 *persObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p3::transToPers called " << endmsg;

  persObj->m_allInts[0] = transObj->roiId()    ;
  persObj->m_allInts[1] = transObj->particleType();
  persObj->m_allInts[2] = transObj->fitndof()  ;

  persObj->m_allFloats[0] = transObj->eta()      ;
  persObj->m_allFloats[1] = transObj->phi()      ;
  persObj->m_allFloats[2] = transObj->mass()     ;
  persObj->m_allFloats[3] = transObj->fitmass()  ;
  persObj->m_allFloats[4] = transObj->fitchi2() ;
  persObj->m_allFloats[5] = transObj->fitx()  ;
  persObj->m_allFloats[6] = transObj->fity()  ;
  persObj->m_allFloats[7] = transObj->fitz()  ;
 
  trackELVCnv.transToPers(&transObj->trackVector(), &persObj->m_trackVector, log);

  m_elementLinkL2BphysCnv.transToPers(&transObj->secondaryDecayLink(), &persObj->m_secondaryDecay, log);
}
