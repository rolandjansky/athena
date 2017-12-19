/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef ATHENAKERNEL_CONDCONTMAKER_H
#define ATHENAKERNEL_CONDCONTMAKER_H 1

#include "AthenaKernel/ICondContMaker.h"
#include "AthenaKernel/CondContFactory.h"
#include "AthenaKernel/StorableConversions.h"
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
    
    virtual
    SG::DataObjectSharedPtr<DataObject>
    Create(const CLID& clid, const std::string& key)  const {
      DataObjID id(clid,key);
      
      return SG::DataObjectSharedPtr<DataObject>
        (SG::asStorable (new CondCont<T>(id)));
    }
  };
}


#endif

