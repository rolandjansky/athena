/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#include "TrigParticleTPCnv/TrigEFBphysCnv_p1.h"
 
//-----------------------------------------------------------------------------
// virt_toPersistent: allows recursive invocation of the converter
//-----------------------------------------------------------------------------

TPObjRef TrigEFBphysCnv_p1::virt_toPersistent( const TrigEFBphys *trans, MsgStream &log ){
  size_t size( this->m_pStorage->size() );
  this->m_pStorage->resize( size+1 );
  TrigEFBphys_p1      pers;
  transToPers( trans, &pers, log);
  this->m_pStorage->operator[]( size ) = pers;
  return TPObjRef( this->m_pStorageTID, size );
}


//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p1::persToTrans( const TrigEFBphys_p1 *persObj,
					     TrigEFBphys    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p1::persToTrans called " << endreq;

  *transObj = TrigEFBphys (persObj->m_roiID,
                           persObj->m_eta,
                           persObj->m_phi,
                           static_cast<TrigEFBphys::pType>(persObj->m_particleType),
                           persObj->m_mass);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p1::transToPers( const TrigEFBphys    *transObj,
					     TrigEFBphys_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p1::transToPers called " << endreq;

  persObj->m_roiID    = transObj->roiId()    ;

  persObj->m_eta      = transObj->eta()      ;
  persObj->m_phi      = transObj->phi()      ;
  persObj->m_valid    = 0    ;
  persObj->m_mass     = transObj->mass()     ;
  persObj->m_particleType   = static_cast<TrigEFBphys_p1::pType_p1>(transObj->particleType());
  //  persObj->m_secondaryDecay  = toPersistent( &m_EFBphysCnv, transObj->m_secondaryDecay, log );
}
