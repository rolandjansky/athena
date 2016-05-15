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


void HepMcParticleLinkCnv_p1::persToTrans( const HepMcParticleLink_p1* persObj,
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

void HepMcParticleLinkCnv_p1::transToPers( const HepMcParticleLink* transObj, 
					   HepMcParticleLink_p1* persObj, 
					   MsgStream &/*msg*/ ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of HepMcParticleLink..."
//       << endreq;
  persObj->m_mcEvtIndex = transObj->eventIndex();
  persObj->m_barcode    = transObj->m_extBarcode.barcode();

//   msg << MSG::DEBUG << "Created persistent state of HepMcGenParticleLink [OK]"
//       << endreq;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

