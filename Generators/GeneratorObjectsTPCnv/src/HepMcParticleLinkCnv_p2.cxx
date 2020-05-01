///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  HepMcParticleLink::PositionFlag flag = HepMcParticleLink::IS_INDEX;

  if (persObj->m_mcEvtIndex == 0) {
    flag = HepMcParticleLink::IS_POSITION;
  }

  transObj->setExtendedBarCode
    ( HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                          persObj->m_mcEvtIndex,
                                          HepMcParticleLink::ExtendedBarCode::eventCollectionFromChar(persObj->m_evtColl),
                                          flag) );
  return;
}

void HepMcParticleLinkCnv_p2::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p2* persObj,
                                           MsgStream &/*msg*/ )
{
  // In the case that the standard production workflow is being used
  // then the first event in the McEventCollection is the only one
  // where the eventIndex could exceed the maximum value of unsigned
  // short. In this case we can work around the issue by using an
  // m_mcEvtIndex of zero as a special case, in which m_mcEvtIndex
  // should be interpreted as the position in the McEventCollection
  // rather than the value of GenEvent::event_number().
  unsigned short index{0};
  if (transObj->getEventPositionInCollection(SG::CurrentEventStore::store())!=0) {
    index = transObj->eventIndex();
  }
  persObj->m_mcEvtIndex = index;
  persObj->m_barcode    = transObj->barcode();
  persObj->m_evtColl    = transObj->getEventCollectionAsChar();
  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
