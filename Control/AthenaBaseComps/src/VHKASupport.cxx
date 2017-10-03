///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./VHKASupport.h"



namespace VHKASupport {
  namespace { 
    void insert ( const std::vector<SG::VarHandleKeyArray*>& src,  std::vector<Gaudi::DataHandle*>& dest, Gaudi::DataHandle::Mode requiredMode ) {
      for ( const SG::VarHandleKeyArray* vhka : src ) {
	if ( not vhka->renounced() ) {
	  for ( SG::VarHandleKey* key: vhka->keys() ) {
	    if ( not ( key->mode()  & requiredMode ) )  break;
	    dest.push_back( key );
	    
	  }
	}
      }    
    }
  }

  void insertInput ( const std::vector<SG::VarHandleKeyArray*>& src,  std::vector<Gaudi::DataHandle*>& dest ) {
    insert( src, dest, Gaudi::DataHandle::Reader );
  }

  void insertOutput ( const std::vector<SG::VarHandleKeyArray*>& src,  std::vector<Gaudi::DataHandle*>& dest ) {
    insert( src, dest, Gaudi::DataHandle::Writer );
  }  
}
