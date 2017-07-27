///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  transObj->m_extBarcode =
    HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                        persObj->m_mcEvtIndex,
                                        persObj->m_evtColl,
                                        false);
  transObj->m_have_particle = false;
  transObj->m_particle = nullptr;

  return;
}

void HepMcParticleLinkCnv_p2::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p2* persObj,
                                           MsgStream &/*msg*/ )
{
  persObj->m_mcEvtIndex = transObj->eventIndex();
  persObj->m_barcode    = transObj->m_extBarcode.barcode();
  persObj->m_evtColl    = transObj->m_extBarcode.m_evtColl;

  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
