/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef ATHENAKERNEL_CONDCONTFACT_H
#define ATHENAKERNEL_CONDCONTFACT_H 1

#include "AthenaKernel/ICondContMaker.h"

#include <map>
#include <string>
#include <mutex>

class CondContBase;

namespace CondContainer {
  class CondContFactory {
  public:
    static CondContFactory& Instance();
    void regMaker(const CLID& clid, ICondContMaker* maker);
    CondContBase* Create(const CLID& clid, const std::string& key) const;
    
  private:
    CondContFactory(){}
    
    CondContFactory(const CondContFactory&);
    CondContFactory& operator=(const CondContFactory&);
    
    std::map<CLID, ICondContMaker*> m_makers;
    mutable std::mutex m_mapMutex;
  };
}

#endif
