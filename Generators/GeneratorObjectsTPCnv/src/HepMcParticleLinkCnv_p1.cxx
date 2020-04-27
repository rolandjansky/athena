///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// In the 21.0 branch an eventIndex of zero means that the transient
// HepMcParticleLink looks at the first event in the
// McEventCollection, otherwise the eventIndex represents the
// GenEvent::event_number of a GenEvent in the McEventCollection and
// this is used to find the appropriate GenEvent.
// NB There is a weakness here that if more than one GenEvent has
// the same event number all links will point to the first GenEvent
// which matches.

void HepMcParticleLinkCnv_p1::persToTrans( const HepMcParticleLink_p1* persObj,
                                           HepMcParticleLink* transObj,
                                           MsgStream &/*msg*/ )
{
  EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  HepMcParticleLink::PositionFlag flag = HepMcParticleLink::IS_INDEX;

  if (persObj->m_mcEvtIndex == 0) {
    flag = HepMcParticleLink::IS_POSITION;
  }

  transObj->setExtendedBarCode
    ( HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                          persObj->m_mcEvtIndex,
                                          evColl,
                                          flag) );
  return;
}

void HepMcParticleLinkCnv_p1::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p1* persObj,
                                           MsgStream &/*msg*/ )
{
  // NB This method assumes that there all GenEvents are stored in a
  // single McEventCollection, as running with split
  // McEventCollections is not supported in 21.0.
  unsigned short index{0};
  if (transObj->getEventPositionInCollection(SG::CurrentEventStore::store())!=0) {
    index = transObj->eventIndex();
  }
  persObj->m_mcEvtIndex = index;
  persObj->m_barcode    = transObj->barcode();
  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
