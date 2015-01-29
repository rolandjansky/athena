/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#undef private
#undef protected
 
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

  transObj->m_roiID    = persObj->m_roiID    ;
  transObj->m_eta      = persObj->m_eta      ;
  transObj->m_phi      = persObj->m_phi      ;
  transObj->m_mass     = persObj->m_mass     ;
 
  transObj->m_particleType    = static_cast<TrigL2Bphys::pType>(persObj->m_particleType);

  //  transObj->m_pVertex        = createTransFromPStore( &m_VertexCnv, persObj->m_pVertex, log );
  //  transObj->m_pVertex        = 0;

  //  transObj->m_ownsVertex = true;

  //  transObj->m_secondaryDecay = createTransFromPStore( &m_L2BphysCnv, persObj->m_secondaryDecay, log );
  transObj->m_secondaryDecay.reset();

  //transObj->m_ownsSecondary = true;

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigL2BphysCnv_p1::transToPers( const TrigL2Bphys    *transObj,
					     TrigL2Bphys_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigL2BphysCnv_p1::transToPers called " << endreq;

  persObj->m_roiID    = transObj->m_roiID    ;
  persObj->m_eta      = transObj->m_eta      ;
  persObj->m_phi      = transObj->m_phi      ;
  persObj->m_valid    = 0    ;
  persObj->m_mass     = transObj->m_mass     ;
  persObj->m_dist     = 0     ;
 
  persObj->m_particleType   = static_cast<TrigL2Bphys_p1::pType_p1>(transObj->m_particleType);
  
  //  persObj->m_pVertex        = NULL;
  //toPersistent( &m_VertexCnv, transObj->m_pVertex, log );
 
  //persObj->m_secondaryDecay  = NULL;
  //toPersistent( &m_L2BphysCnv, transObj->m_secondaryDecay, log );
  
}
