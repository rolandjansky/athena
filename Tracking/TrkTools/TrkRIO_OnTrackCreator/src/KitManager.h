/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRKRIO_ONTRACKCREATOR_KITMANAGER_H
#define TRKRIO_ONTRACKCREATOR_KITMANAGER_H

#include <ostream>
#include <map>
#include <memory>
#include <mutex>
#include "CxxUtils/checker_macros.h"

class KitManagerBase
{
protected:
  KitManagerBase();
  virtual ~KitManagerBase();

  const void *kitPtr(const std::string &name) const;

  bool registerKit(const std::string& name, const void *a_kit);

  mutable std::mutex m_mutex;
  std::map<std::string, const void * > m_registry;
public:
  void dumpKits(std::ostream &out) const;

};

template <class T_KitInterface>
class KitManager : public KitManagerBase {
public:
  ~KitManager() {
    for( std::pair<const std::string, const void *> &elm : m_registry ) {
      const T_KitInterface *ptr=reinterpret_cast<const T_KitInterface *>(elm.second);
      delete ptr;
      elm.second = nullptr;
    }
  }

  bool registerKit(const std::string& name, const T_KitInterface *a_kit) {
    return KitManagerBase::registerKit(name, static_cast< const void *>(a_kit));
  }

  const T_KitInterface &kit(const std::string &name) const {
    return *(reinterpret_cast< const T_KitInterface *>(KitManagerBase::kitPtr(name)));
  }

  static
  KitManager<T_KitInterface>& instance() {
    /* in C++11 this is to happen once the issue is that we return a ref
     * rather than const ref. To be thread safe we need to have static const */
    // Map is protected with a lock.
    static KitManager<T_KitInterface> s_instance ATLAS_THREAD_SAFE;
    return s_instance;
  }
};


#endif
