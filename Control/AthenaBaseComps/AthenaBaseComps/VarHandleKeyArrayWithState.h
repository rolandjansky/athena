// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABASECOMPS_VARHANDLEKEYARRAYWITHSTATE_H
#define ATHENABASECOMPS_VARHANDLEKEYARRAYWITHSTATE_H

#include <vector>
#include <functional>
#include <GaudiKernel/DataHandle.h>
#include "StoreGate/VarHandleKeyArray.h"

class VarHandleKeyArrayWithState {
public:
  template <class Component >
  void declare( Component* owner) {
    if ( renounced() ) 
      return;
    for ( SG::VarHandleKey* k : m_vhka->keys() ) {
      owner->declare ( *k );
      k->setOwner( owner );
    }
  }

  void appendIfOutput( std::vector<Gaudi::DataHandle*>& output ) const {
    if ( renounced() ) 
      return;
    for( SG::VarHandleKey* k: m_vhka->keys() ) {
      if ( k->mode() & Gaudi::DataHandle::Writer ) {
	output.push_back( k );
      } 
    }
  }

  void appendIfInput( std::vector<Gaudi::DataHandle*>& output ) const {
    if ( renounced() ) 
      return;
    for( SG::VarHandleKey* k: m_vhka->keys() ) {
      if ( k->mode() & Gaudi::DataHandle::Reader ) {
	output.push_back( k );
      } 
    }
  }


  VarHandleKeyArrayWithState( SG::VarHandleKeyArray* a ) 
    : m_vhka( a ) {}  
  
  SG::VarHandleKeyArray* operator ->() { return m_vhka; }
  const SG::VarHandleKeyArray* operator ->() const{ return m_vhka; }
  bool operator==( const SG::VarHandleKeyArray* a ) const { return m_vhka == a; }
  void renounce() { m_renounced = true; }
  bool renounced() const { return m_renounced; }
  
  

private:
  SG::VarHandleKeyArray* m_vhka;
  bool m_renounced{ false };
};



#endif
