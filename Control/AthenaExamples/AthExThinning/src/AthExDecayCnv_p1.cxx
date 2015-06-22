///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExDecayCnv_p1.cxx 
// Implementation file for class AthExDecayCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// AthExThinning includes
#include "AthExThinning/AthExDecay.h"

// AthExThinning includes
#include "AthExDecayCnv_p1.h"

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
AthExDecayCnv_p1::persToTrans( const AthExDecay_p1* persObj, 
			       AthExDecay* transObj, 
			       MsgStream &msg )
{
  msg << MSG::DEBUG 
      << "Loading Decay from persistent state..."
      << endreq;
  
  // convert the ElementLink<Particles>
  m_partLinkCnv.persToTrans( &persObj->m_p1,
			     &transObj->m_p1,
			     msg );
  m_partLinkCnv.persToTrans( &persObj->m_p2,
			     &transObj->m_p2,
			     msg );

  // convert the ElementLink<Particles> (known to be thinned away)
  m_partLinkCnv.persToTrans( &persObj->m_l1,
			     &transObj->m_l1,
			     msg );
  m_partLinkCnv.persToTrans( &persObj->m_l2,
			     &transObj->m_l2,
			     msg );

  msg << MSG::DEBUG 
      << "Loaded Decay from persistent state [OK]"
      << endreq;
  return;
}

void 
AthExDecayCnv_p1::transToPers( const AthExDecay* transObj, 
			       AthExDecay_p1* persObj, 
			       MsgStream &msg )
{
  msg << MSG::DEBUG 
      << "Creating persistent state of Decay..."
      << endreq;

  // convert the ElementLink<Particles>
  m_partLinkCnv.transToPers( &transObj->m_p1,
			     &persObj->m_p1,
			     msg );
  m_partLinkCnv.transToPers( &transObj->m_p2,
			     &persObj->m_p2,
			     msg );

  // convert the ElementLink<Particles> (known to be thinned away)
  m_partLinkCnv.transToPers( &transObj->m_l1,
			     &persObj->m_l1,
			     msg );
  m_partLinkCnv.transToPers( &transObj->m_l2,
			     &persObj->m_l2,
			     msg );
  msg << MSG::DEBUG 
      << "Created persistent state of Decay [OK]"
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

