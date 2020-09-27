///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcParticleLinkCnv_p1.cxx
// Implementation file for class HepMcParticleLinkCnv_p1
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

///////////////////////////////////////////////////////////////////
/// Public methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Const methods:
///////////////////////////////////////////////////////////////////

#undef NDEBUG

void HepMcParticleLinkCnv_p1::persToTrans( const HepMcParticleLink_p1* persObj,
                                           HepMcParticleLink* transObj,
                                           MsgStream &/*msg*/ )
{
  bool flag = false;
  if (persObj->m_mcEvtIndex == 0) {
    flag = true;
  }
  transObj->m_extBarcode =
    HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                        persObj->m_mcEvtIndex,
                                        EBC_MAINEVCOLL,
                                        flag);
  transObj->m_have_particle = false;
  transObj->m_particle = nullptr;

  return;
}

void HepMcParticleLinkCnv_p1::transToPers( const HepMcParticleLink* transObj,
                                           HepMcParticleLink_p1* persObj,
                                           MsgStream &msg )
{
  unsigned short index{0};
  const HepMcParticleLink::index_type position =
    transObj->getEventPositionInCollection();
  if (position!=0) {
    index = transObj->eventIndex();
    if(transObj->eventIndex()!=static_cast<HepMcParticleLink::index_type>(index)) {
      msg << MSG::WARNING << "Attempting to persistify an eventIndex larger than max unsigned short!" << endmsg;
    }
  }
  persObj->m_mcEvtIndex = index;
  persObj->m_barcode    = transObj->m_extBarcode.barcode();

  return;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
