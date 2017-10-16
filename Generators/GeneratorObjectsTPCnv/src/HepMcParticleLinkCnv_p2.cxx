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
  //   msg << MSG::DEBUG << "Loading HepMcParticleLink from persistent state..."
  //       << endreq;
  if (transObj->m_ptrs.m_dict == nullptr)
    transObj->init_dict();
  transObj->m_extBarcode =
    HepMcParticleLink::ExtendedBarCode( persObj->m_barcode,
                                        persObj->m_mcEvtIndex );


  //   msg << MSG::DEBUG << "Loaded HepMcParticleLink from persistent state [OK]"
  //       << endreq;
  return;
}

void HepMcParticleLinkCnv_p2::transToPers( const HepMcParticleLink*,
                                           HepMcParticleLink_p2*,
                                           MsgStream &/*msg*/ )
{
  throw std::runtime_error("HepMcParticleLinkCnv_p2::transToPers is not supported in this release!");
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////
