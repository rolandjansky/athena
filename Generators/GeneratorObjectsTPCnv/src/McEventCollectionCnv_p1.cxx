///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
// Constructors
///////////////////////////////////////////////////////////////////

McEventCollectionCnv_p1::McEventCollectionCnv_p1() :
  Base_t( )
{}

McEventCollectionCnv_p1::McEventCollectionCnv_p1( const McEventCollectionCnv_p1& rhs ) :
  Base_t( rhs )
{}

McEventCollectionCnv_p1&
McEventCollectionCnv_p1::operator=( const McEventCollectionCnv_p1& rhs )
{
  if ( this != &rhs ) {
    Base_t::operator=( rhs );
  }
  return *this;
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

McEventCollectionCnv_p1::~McEventCollectionCnv_p1()
{
}


void McEventCollectionCnv_p1::persToTrans( const McEventCollection_p1* persObj,
                                           McEventCollection* transObj,
                                           MsgStream& msg )
{
  msg << MSG::DEBUG << "Loading McEventCollection from persistent state..."
      << endmsg;

  // elements are managed by DataPool
  transObj->clear(SG::VIEW_ELEMENTS);

  HepMC::DataPool datapool = HepMC::DataPool();
  datapool.evt.reserve (   20 );
  datapool.vtx.reserve ( 1000 );
  datapool.part.reserve( 1000 );

  // create a T/P converter for the GenEvent class
  static GenEventCnv_p1 genEvtCnv;
  genEvtCnv.setDataPool( &datapool );

  const std::size_t nEvts = persObj->size();
  if ( datapool.evt.capacity() - datapool.evt.allocated() < nEvts ) {
    datapool.evt.reserve( datapool.evt.allocated() + nEvts );
  }
  transObj->reserve( nEvts );
  for ( std::vector<GenEvent_p1>::const_iterator
          itr    = persObj->begin(),
          itrEnd = persObj->end();
        itr != itrEnd;
        ++itr ) {
    const GenEvent_p1& persEvt = *itr;
    HepMC::GenEvent * genEvt   = datapool.evt.nextElementPtr();
    genEvtCnv.persToTrans( &persEvt, genEvt, msg );
    transObj->push_back( genEvt );

  } //> end loop over genEvents

  msg << MSG::DEBUG << "Loaded McEventCollection from persistent state [OK]"
      << endmsg;

  return;
}

void McEventCollectionCnv_p1::transToPers( const McEventCollection*,
                                           McEventCollection_p1*,
                                           MsgStream& msg )
{
  msg << MSG::DEBUG << "Creating persistent state of McEventCollection..."
      << endmsg;

  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endmsg
      << "You are not supposed to end-up here ! Go away !"
      << endmsg;

  throw std::runtime_error( "Retired McEventCollectionCnv_p1::transToPers() !!" );
  return;
}
