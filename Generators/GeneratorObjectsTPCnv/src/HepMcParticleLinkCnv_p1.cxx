///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "GeneratorObjects/McEventCollection.h"

///////////////////////////////////////////////////////////////////
/// Public methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Const methods:
///////////////////////////////////////////////////////////////////


void HepMcParticleLinkCnv_p1::persToTrans( const HepMcParticleLink_p1* persObj,
                                           HepMcParticleLink* transObj,
                                           MsgStream &/*msg*/ )
{
  EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  if (persObj->m_mcEvtIndex>0) {
    // HACK
    const CLID clid = ClassID_traits<McEventCollection>::ID();
    if (SG::CurrentEventStore::store()->proxy (clid, "TruthEvent_PU") != nullptr) {
      evColl = EBC_FIRSTPUEVCOLL;
    }
  }
  transObj->setExtendedBarCode
    ( HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                          persObj->m_mcEvtIndex,
                                          evColl,
                                          HepMcParticleLink::IS_POSITION) );
  return;
}

void HepMcParticleLinkCnv_p1::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p1* persObj,
                                           MsgStream &/*msg*/ )
{
  persObj->m_mcEvtIndex = transObj->getEventPositionInCollection(SG::CurrentEventStore::store());
  persObj->m_barcode    = transObj->barcode();
  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
