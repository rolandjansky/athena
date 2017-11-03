/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef ATHENAKERNEL_CONDCONTMAKER_H
#define ATHENAKERNEL_CONDCONTMAKER_H 1

#include "AthenaKernel/ICondContMaker.h"
#include "AthenaKernel/CondContFactory.h"
#include "AthenaKernel/ClassID_traits.h"
#include "GaudiKernel/DataObjID.h"

class CondContBase;

namespace CondContainer {
  template <typename T>
  class CondContMaker : public ICondContMaker {
  public:
    CondContMaker() {
      CondContFactory::Instance().regMaker( ClassID_traits<T>::ID() , this );
    }
    
    virtual CondContBase* Create(const CLID& clid, const std::string& key)  const {
      DataObjID id(clid,key);
      
      return new CondCont<T>(id);
    }
  };
}


#endif

