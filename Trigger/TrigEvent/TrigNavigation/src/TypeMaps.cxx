/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TypeMaps.h"
namespace HLT {
  TypeMaps::CLIDtoTypeProxyMap& TypeMaps::proxies()
  {
    static CLIDtoTypeProxyMap map;
    return map;
  }
  
  TypeMaps::CLIDtoHolderMap& TypeMaps::holders()
  {
    static CLIDtoHolderMap map;  
    return map;
  }
  
  
  TypeMaps::NametoCLIDMap& TypeMaps::type2clid()
  {
    static NametoCLIDMap type2clid;
    return type2clid;
  }
}


namespace HLTNavDetails {
  class TypeMapDeleter {
  public:
    ~TypeMapDeleter() {
      /*
      {
	HLT::TypeMaps::CLIDtoTypeProxyMap::iterator i;
	for ( i = HLT::TypeMaps::proxies().begin(); i != HLT::TypeMaps::proxies().end(); ++i) {
	  delete i->second; i->second = 0;
	}
	HLT::TypeMaps::proxies().clear();
      }
      {	
	HLT::TypeMaps::CLIDtoHolderMap::iterator i;
	for ( i = HLT::TypeMaps::holders().begin(); i != HLT::TypeMaps::holders().end(); ++i) {
	  delete i->second; i->second = 0;
	}	
	HLT::TypeMaps::holders().clear();
      }
      */
    }
  };
  TypeMapDeleter typeMapDeleter; // this objects is created in the file scope so at the deletion it can clean up the static types map
}

