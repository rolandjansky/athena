/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TYPEMAPS_H
#define TRIGNAVIGATION_TYPEMAPS_H

#include <map>
#include <string>
#include <memory>

#include "GaudiKernel/ClassID.h"

#include "CxxUtils/checker_macros.h"

#include "TrigNavigation/Holder.h"
#include "TrigNavigation/TypeProxy.h"

namespace HLT {
  /**
   * Type registration of holder, proxies and name/CLID mappings.
   */
  class TypeMaps {
  public:
    typedef std::map<CLID, std::unique_ptr<const HLTNavDetails::ITypeProxy>> CLIDtoTypeProxyMap;
    typedef std::map<CLID, std::unique_ptr<const HLTNavDetails::IHolder>>    CLIDtoHolderMap;
    typedef std::map<std::string, CLID> NametoCLIDMap;

    static const CLIDtoTypeProxyMap& proxies() { return m_proxies; }
    static const CLIDtoHolderMap&    holders() { return m_holders; }
    static const NametoCLIDMap&      type2clid() { return m_type2clid; }

    // The registration methods are invoked at compile-time and we use
    // a const_cast to fill the otherwise read-only type maps.
    template<class T> 
    static constexpr void registerType ATLAS_NOT_THREAD_SAFE () {
      auto& nc_proxies = const_cast<CLIDtoTypeProxyMap&>(m_proxies);
      auto& nc_types = const_cast<NametoCLIDMap&>(m_type2clid);
      nc_proxies[ClassID_traits<T>::ID()] = std::make_unique<HLTNavDetails::TypeProxy<T>>();
      nc_types[ClassID_traits<T>::typeName()] = ClassID_traits<T>::ID();
    }

    template<class T, class C>
    static constexpr void registerFeatureContainer ATLAS_NOT_THREAD_SAFE () {
      auto& nc_holders = const_cast<CLIDtoHolderMap&>(m_holders);
      nc_holders[ClassID_traits<T>::ID()] = std::make_unique<HLTNavDetails::HolderImp<T, C>>();
      HLT::TypeMaps::registerType<T>();
      HLT::TypeMaps::registerType<C>();
    }

  private:
    // statically filled by the above registration methods, otherwise const:
    inline static const CLIDtoTypeProxyMap m_proxies;
    inline static const CLIDtoHolderMap    m_holders;
    inline static const NametoCLIDMap      m_type2clid;
  };
}

#endif
