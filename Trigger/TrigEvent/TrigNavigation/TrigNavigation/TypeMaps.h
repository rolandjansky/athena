/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#include <map>
#include <string>

#include "GaudiKernel/ClassID.h"

#include "TrigNavigation/Holder.h"
#include "TrigNavigation/TypeProxy.h"

namespace HLT {
  class TypeMaps {
  public:
  typedef std::map<CLID,  HLTNavDetails::ITypeProxy*> CLIDtoTypeProxyMap;
    static CLIDtoTypeProxyMap& proxies();
    
    typedef std::map<CLID,  HLTNavDetails::IHolder*> CLIDtoHolderMap;
    static CLIDtoHolderMap& holders();
    
    typedef std::map<std::string, CLID> NametoCLIDMap;
    static NametoCLIDMap& type2clid();   //!< translate class name to CLID
    
    template<class T> 
    static void registerType() {
      HLT::TypeMaps::proxies()[ClassID_traits<T>::ID()] = new HLTNavDetails::TypeProxy<T>();
      HLT::TypeMaps::type2clid()[ ClassID_traits<T>::typeName() ] = ClassID_traits<T>::ID();        
    }


    template<class T, class C>
    static void registerFeatureContainer() {
      HLT::TypeMaps::holders()[ClassID_traits<T>::ID()] = new HLTNavDetails::HolderImp<T, C>();
      HLT::TypeMaps::registerType<T>();
      HLT::TypeMaps::registerType<C>();
    }
    
  };
}
