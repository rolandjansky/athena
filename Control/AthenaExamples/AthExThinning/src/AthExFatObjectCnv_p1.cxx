///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObjectCnv_p1.cxx 
// Implementation file for class AthExFatObjectCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// AthExThinning includes
#include "AthExThinning/AthExFatObject.h"

// AthExThinning includes
#include "AthExFatObjectCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
AthExFatObjectCnv_p1::persToTrans (const AthExFatObject_p1* pers, 
				   AthExFatObject* trans, 
				   MsgStream& msg)
{
  msg << MSG::DEBUG 
      << "Loading FatObject from persistent state..."
      << endreq;
  
  m_cnv.persToTrans( &pers->m_particleLink, &trans->m_particleLink, msg );
  // restore the raw-pointer too
  if (trans->m_particleLink.isValid()) {
    trans->m_particle = *trans->m_particleLink;
  } else {
    trans->m_particle = 0;
  }

  msg << MSG::DEBUG 
      << "Loaded FatObject from persistent state [OK]"
      << endreq;
  return;
}

void 
AthExFatObjectCnv_p1::transToPers( const AthExFatObject* trans, 
				     AthExFatObject_p1* pers, 
				     MsgStream& msg )
{
  msg << MSG::DEBUG 
      << "Creating persistent state of FatObject..."
      << endreq;

  m_cnv.transToPers( &trans->m_particleLink, &pers->m_particleLink, msg );

  msg << MSG::DEBUG 
      << "Created persistent state of FatObject [OK]"
      << endreq;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

