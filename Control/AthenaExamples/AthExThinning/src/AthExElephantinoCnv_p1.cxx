///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExElephantinoCnv_p1.cxx 
// Implementation file for class AthExElephantinoCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// AthExThinning includes
#include "AthExThinning/AthExElephantino.h"

// AthExThinning includes
#include "AthExElephantinoCnv_p1.h"

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
AthExElephantinoCnv_p1::persToTrans( const AthExElephantino_p1* pers, 
				     AthExElephantino* trans, 
				     MsgStream& msg )
{
  msg << MSG::DEBUG 
      << "Loading Elephantino from persistent state..."
      << endreq;
  
  m_cnv.persToTrans( &pers->m_legs, &trans->m_legs, msg );
  // known to be thinned away...
  m_cnv.persToTrans( &pers->m_ears, &trans->m_ears, msg );

  msg << MSG::DEBUG 
      << "Loaded Elephantino from persistent state [OK]"
      << endreq;
  return;
}

void 
AthExElephantinoCnv_p1::transToPers( const AthExElephantino* trans, 
				     AthExElephantino_p1* pers, 
				     MsgStream& msg )
{
  msg << MSG::DEBUG 
      << "Creating persistent state of Elephantino..."
      << endreq;

  m_cnv.transToPers( &trans->m_legs, &pers->m_legs, msg );
  // known to be thinned away...
  m_cnv.transToPers( &trans->m_ears, &pers->m_ears, msg );

  msg << MSG::DEBUG 
      << "Created persistent state of Elephantino [OK]"
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

