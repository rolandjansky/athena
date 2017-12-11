/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */


#ifndef ATHENAKERNEL_ICONDCONTMAKER_H
#define ATHENAKERNEL_ICONDCONTMAKER_H 1

#include <string>
#include "AthenaKernel/DataObjectSharedPtr.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"


namespace CondContainer {

  class ICondContMaker {
  public:
    virtual SG::DataObjectSharedPtr<DataObject> Create(const CLID& clid, const std::string& key) const = 0;
    virtual ~ICondContMaker() {}
  };
}

#endif
