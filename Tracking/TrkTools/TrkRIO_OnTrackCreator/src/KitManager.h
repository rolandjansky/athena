/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _KitManager_H_
#define _KitManager_H_

#include <ostream>
#include <map>
#include <memory>
#include "CxxUtils/checker_macros.h"

class KitManagerBase
{
protected:
  KitManagerBase();
  virtual ~KitManagerBase();

  const void *kitPtr(const std::string &name) const;

  bool registerKit(std::string name, const void *a_kit);

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

  bool registerKit(std::string name, const T_KitInterface *a_kit) {
    return KitManagerBase::registerKit(name, static_cast< const void *>(a_kit));
  }

  const T_KitInterface &kit(const std::string &name) const {
    return *(reinterpret_cast< const T_KitInterface *>(KitManagerBase::kitPtr(name)));
  }

  static
  KitManager<T_KitInterface>& instance ATLAS_NOT_THREAD_SAFE () {
    /* in C++11 this is to happen once the issue is that we return a ref
     * rather than const ref. To be thread safe we need to have static const */
    static std::unique_ptr<KitManager<T_KitInterface>> s_instance = std::make_unique<KitManager<T_KitInterface>>();
    return *s_instance;
  }
};


#endif
