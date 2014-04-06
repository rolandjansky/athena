/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#undef private
#undef protected
 
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

  transObj->m_roiID    = persObj->m_roiID    ;
  transObj->m_eta      = persObj->m_eta      ;
  transObj->m_phi      = persObj->m_phi      ;
  transObj->m_mass     = persObj->m_mass     ;
  
  transObj->m_particleType    = static_cast<TrigEFBphys::pType>(persObj->m_particleType);  
  //  transObj->m_secondaryDecay = createTransFromPStore( &m_EFBphysCnv, persObj->m_secondaryDecay, log );
  transObj->m_secondaryDecay.reset();


}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigEFBphysCnv_p1::transToPers( const TrigEFBphys    *transObj,
					     TrigEFBphys_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigEFBphysCnv_p1::transToPers called " << endreq;

  persObj->m_roiID    = transObj->m_roiID    ;

  persObj->m_eta      = transObj->m_eta      ;
  persObj->m_phi      = transObj->m_phi      ;
  persObj->m_valid    = 0    ;
  persObj->m_mass     = transObj->m_mass     ;
  persObj->m_particleType   = static_cast<TrigEFBphys_p1::pType_p1>(transObj->m_particleType);
  //  persObj->m_secondaryDecay  = toPersistent( &m_EFBphysCnv, transObj->m_secondaryDecay, log );

  
}
