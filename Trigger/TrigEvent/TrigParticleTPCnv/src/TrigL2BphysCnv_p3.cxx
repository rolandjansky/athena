/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"
#include "TrigParticle/TrigL2Bphys.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigL2BphysCnv_p3.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<TrigInDetTrackCollection> > trackELVCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p3::persToTrans( const TrigL2Bphys_p3 *persObj,
				     TrigL2Bphys    *transObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p3::persToTrans called " << endreq;

  transObj->m_roiID    = persObj->m_allInts[0];
  transObj->m_particleType    = static_cast<TrigL2Bphys::pType>(persObj->m_allInts[1]);
  transObj->m_fitndof  = persObj->m_allInts[2];
  transObj->m_eta      = persObj->m_allFloats[0];
  transObj->m_phi      = persObj->m_allFloats[1];
  transObj->m_mass     = persObj->m_allFloats[2];
  transObj->m_fitmass  = persObj->m_allFloats[3];
  transObj->m_fitchi2  = persObj->m_allFloats[4];
  transObj->m_fitx     = persObj->m_allFloats[5];
  transObj->m_fity     = persObj->m_allFloats[6];
  transObj->m_fitz     = persObj->m_allFloats[7];
 
  trackELVCnv.persToTrans(&persObj->m_trackVector, &transObj->m_trackVector, log);

  m_elementLinkL2BphysCnv.persToTrans(&persObj->m_secondaryDecay, &transObj->m_secondaryDecay, log);

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p3::transToPers( const TrigL2Bphys    *transObj,
				     TrigL2Bphys_p3 *persObj,
				     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p3::transToPers called " << endreq;

  persObj->m_allInts[0] = transObj->m_roiID    ;
  persObj->m_allInts[1] = transObj->m_particleType;
  persObj->m_allInts[2] = transObj->m_fitndof  ;

  persObj->m_allFloats[0] = transObj->m_eta      ;
  persObj->m_allFloats[1] = transObj->m_phi      ;
  persObj->m_allFloats[2] = transObj->m_mass     ;
  persObj->m_allFloats[3] = transObj->m_fitmass  ;
  persObj->m_allFloats[4] = transObj->m_fitchi2  ;
  persObj->m_allFloats[5] = transObj->m_fitx  ;
  persObj->m_allFloats[6] = transObj->m_fity  ;
  persObj->m_allFloats[7] = transObj->m_fitz  ;
 
  trackELVCnv.transToPers(&transObj->m_trackVector, &persObj->m_trackVector, log);

  m_elementLinkL2BphysCnv.transToPers(&transObj->m_secondaryDecay, &persObj->m_secondaryDecay, log);

}
