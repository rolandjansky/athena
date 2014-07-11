///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv_p1.cxx 
// Implementation file for class McEventCollectionCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <utility>

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p1.h"
#include "GeneratorObjectsTPCnv/GenEventCnv_p1.h"
#include "HepMcDataPool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McEventCollectionCnv_p1::McEventCollectionCnv_p1() :
  Base_t( ),
  m_pool( new HepMC::DataPool )
{}

McEventCollectionCnv_p1::McEventCollectionCnv_p1( const McEventCollectionCnv_p1& rhs ) :
  Base_t( rhs ),
  m_pool( new HepMC::DataPool )
{}

McEventCollectionCnv_p1& 
McEventCollectionCnv_p1::operator=( const McEventCollectionCnv_p1& rhs )
{
  if ( this != &rhs ) {
    Base_t::operator=( rhs );
    m_pool = new HepMC::DataPool;
  } 
  return *this;
}

// Destructor
///////////////

McEventCollectionCnv_p1::~McEventCollectionCnv_p1()
{
  delete m_pool; m_pool = 0;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void McEventCollectionCnv_p1::persToTrans( const McEventCollection_p1* persObj,
					   McEventCollection* transObj, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading McEventCollection from persistent state..."
      << endreq;

  // elements are managed by DataPool
  transObj->clear(SG::VIEW_ELEMENTS);

  static bool first = true;
  if ( first ) {
    first = false;
    m_pool->evt.reserve (   20 );
    m_pool->vtx.reserve ( 1000 );
    m_pool->part.reserve( 1000 );
  }

  // create a T/P converter for the GenEvent class
  static GenEventCnv_p1 genEvtCnv;
  genEvtCnv.setDataPool( m_pool );

  const std::size_t nEvts = persObj->size();
  if ( m_pool->evt.capacity() - m_pool->evt.allocated() < nEvts ) {
    m_pool->evt.reserve( m_pool->evt.allocated() + nEvts );
  }
  transObj->reserve( nEvts );
  for ( std::vector<GenEvent_p1>::const_iterator 
	  itr    = persObj->begin(),
	  itrEnd = persObj->end();
	itr != itrEnd;
	++itr ) {
    const GenEvent_p1& persEvt = *itr;
    HepMC::GenEvent * genEvt   = m_pool->evt.nextElementPtr();
    genEvtCnv.persToTrans( &persEvt, genEvt, msg );
    transObj->push_back( genEvt );
    
  } //> end loop over genEvents

  msg << MSG::DEBUG << "Loaded McEventCollection from persistent state [OK]"
      << endreq;

  return;
}

void McEventCollectionCnv_p1::transToPers( const McEventCollection*, 
					   McEventCollection_p1*, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Creating persistent state of McEventCollection..."
      << endreq;

  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endreq
      << "You are not supposed to end-up here ! Go away !"
      << endreq;

  throw std::runtime_error( "Retired McEventCollectionCnv_p1::transToPers() !!" );
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

