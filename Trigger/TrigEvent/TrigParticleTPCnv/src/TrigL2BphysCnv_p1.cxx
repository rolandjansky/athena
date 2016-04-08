/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#include "TrigParticleTPCnv/TrigL2BphysCnv_p1.h"
 
//-----------------------------------------------------------------------------
// virt_toPersistent: allows recursive invocation of the converter
//-----------------------------------------------------------------------------

TPObjRef TrigL2BphysCnv_p1::virt_toPersistent( const TrigL2Bphys *trans, MsgStream &log ){
  size_t size( this->m_pStorage->size() );
  this->m_pStorage->resize( size+1 );
  TrigL2Bphys_p1      pers;
  transToPers( trans, &pers, log);
  this->m_pStorage->operator[]( size ) = pers;
  return TPObjRef( this->m_pStorageTID, size );
}


//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p1::persToTrans( const TrigL2Bphys_p1 *persObj,
					     TrigL2Bphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p1::persToTrans called " << endreq;

  *transObj = TrigL2Bphys (persObj->m_roiID,
                           persObj->m_eta,
                           persObj->m_phi,
                           static_cast<TrigL2Bphys::pType>(persObj->m_particleType),
                           persObj->m_mass); 
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p1::transToPers( const TrigL2Bphys    *transObj,
					     TrigL2Bphys_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p1::transToPers called " << endreq;

  persObj->m_roiID    = transObj->roiId()    ;
  persObj->m_eta      = transObj->eta()      ;
  persObj->m_phi      = transObj->phi()      ;
  persObj->m_valid    = 0    ;
  persObj->m_mass     = transObj->mass()     ;
  persObj->m_dist     = 0     ;
 
  persObj->m_particleType   = static_cast<TrigL2Bphys_p1::pType_p1>(transObj->particleType());
  
}
