///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthenaKernel/ExtendedEventContext.h"

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
                                           MsgStream &msg )
{
  // In the case that the standard production workflow is being used
  // then the first event in the McEventCollection is the only one
  // where the eventIndex could exceed the maximum value of unsigned
  // short. In this case we can work around the issue by using an
  // m_mcEvtIndex of zero as a special case, in which m_mcEvtIndex
  // should be interpreted as the position in the McEventCollection
  // rather than the value of GenEvent::event_number().
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const IProxyDict* proxy = Atlas::getExtendedEventContext(ctx).proxy();
  unsigned short index{0};
  const HepMcParticleLink::index_type position =
    HepMcParticleLink::getEventPositionInCollection(transObj->eventIndex(),
                                                    transObj->getEventCollection(),
                                                    proxy).at(0);
  if (position!=0) {
    index = transObj->eventIndex();
    if(transObj->eventIndex()!=static_cast<HepMcParticleLink::index_type>(index)) {
      msg << MSG::WARNING << "Attempting to persistify an eventIndex larger than max unsigned short!" << endmsg;
    }
  }
  persObj->m_mcEvtIndex = index;
  persObj->m_barcode    = transObj->barcode();
  persObj->m_evtColl    = transObj->getEventCollectionAsChar();
  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
