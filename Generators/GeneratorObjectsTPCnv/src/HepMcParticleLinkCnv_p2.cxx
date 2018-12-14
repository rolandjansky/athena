///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

///////////////////////////////////////////////////////////////////
/// Public methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Const methods:
///////////////////////////////////////////////////////////////////


void HepMcParticleLinkCnv_p2::persToTrans( const HepMcParticleLink_p2* persObj,
                                           HepMcParticleLink* transObj,
                                           MsgStream &/*msg*/ )
{
  if (transObj->m_ptrs.m_dict == nullptr)
    transObj->init_dict();
  transObj->m_extBarcode =
    HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                        persObj->m_mcEvtIndex );
  return;
}

void HepMcParticleLinkCnv_p2::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p2* persObj,
                                           MsgStream &/*msg*/ )
{
  persObj->m_mcEvtIndex = transObj->eventIndex();
  persObj->m_barcode    = transObj->m_extBarcode.barcode();
  persObj->m_evtColl    = 'a'; // hard-coded until we change the transient class implementation
  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
