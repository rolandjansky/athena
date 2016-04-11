/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOODATABASEFACTORY_H
#define IOODATABASEFACTORY_H

#include "Reflex/PluginService.h"
#include "StorageSvc/IOODatabase.h"

#include <new>

// Reflex object factory definition
namespace {
  template <typename P> class Factory<P,pool::IOODatabase*(void*)> {
  public: static void Func( void* ret, void* mem, const std::vector<void*>& arg, void*)     {
    void* p = (pool::IOODatabase*)(mem ? ::new(mem) P(arg[0]) : ::new P(arg[0]));
    if (ret) *(void**)ret = p;
  }
  };
}

#define DECLARE_OODATABASE(x) PLUGINSVC_FACTORY(x,pool::IOODatabase*(void*))

#endif
